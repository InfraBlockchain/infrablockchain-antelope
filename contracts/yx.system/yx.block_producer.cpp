#include "yx.system.hpp"

#include <yosemitelib/native_token.hpp>
#include <yosemitelib/yx_asset.hpp>
#include <yosemitelib/system_accounts.hpp>
#include <yosemitelib/transaction_fee.hpp>
#include <yosemitelib/transaction_vote.h>

#include <eosiolib/system.h>
#include <eosiolib/memory.hpp>
#include <eosiolib/dispatcher.hpp>

#include <cmath>

namespace yosemitesys {

    using yosemite::yx_asset;

//    const uint32_t blocks_per_year       = 52*7*24*2*3600;   // half seconds per year
//    const uint32_t seconds_per_year      = 52*7*24*3600;
//    const uint32_t blocks_per_day        = 2 * 24 * 3600;
//    const uint32_t blocks_per_hour       = 2 * 3600;
    const uint64_t useconds_per_day      = 24 * 3600 * uint64_t(1000000);
//    const uint64_t useconds_per_year     = seconds_per_year*1000000ll;

    const int64_t    vote_weight_timestamp_epoch = 1514764800ll;  // 01/01/2018 @ 12:00am (UTC)
    const uint32_t   seconds_per_week = 24 * 3600 * 7;


    double weighted_vote( uint32_t vote ) {
        double weight = int64_t( now() - vote_weight_timestamp_epoch ) / double( seconds_per_week * 52 );
        return double(vote) * std::pow( 2, weight );
    }

    void system_contract::onblock( block_timestamp timestamp, account_name producer ) {
        using namespace eosio;

        require_auth(YOSEMITE_SYSTEM_ACCOUNT);

        /**
         * At startup the initial producer may not be one that is registered / elected
         * and therefore there may be no producer object for them.
         */
        auto prod = _producers.find(producer);
        if ( prod != _producers.end() ) {
            _gstate.total_unpaid_blocks++;
            _producers.modify( prod, 0, [&](auto& p ) {
                p.unpaid_blocks++;
            });
        }

        /// read tran transaction votes sum data for each vote-to(candidate) account,
        /// accumulated from the transactions in the previous block right before current pending block
        uint32_t tx_votes_packed_size = read_head_block_trx_votes_data(nullptr, 0);

        if (tx_votes_packed_size > 0) {

            int cnt = tx_votes_packed_size / sizeof(struct yosemite_transaction_vote);

            /// allocate buffer memory through memory manager
            char* buf_tx_votes_data_packed = static_cast<char*>(malloc(tx_votes_packed_size));
            eosio_assert( buf_tx_votes_data_packed != nullptr, "malloc failed for tx votes packed data" );

            read_head_block_trx_votes_data(buf_tx_votes_data_packed, tx_votes_packed_size);
            yosemite_transaction_vote* votes_arr = reinterpret_cast<struct yosemite_transaction_vote *>(buf_tx_votes_data_packed);

            for (int i = 0; i < cnt; i++) {
                yosemite_transaction_vote &trx_vote = votes_arr[i];

                auto prod_vote = _producers.find( trx_vote.to );

                if (prod_vote != _producers.end()) {
                    _producers.modify( prod_vote, 0, [&]( producer_info& info ){
                        double weighted_vote_amount = weighted_vote(trx_vote.amount);
                        info.total_votes += weighted_vote_amount;
                        _gstate.total_producer_vote_weight += weighted_vote_amount;
                    });
                }
            }

            free(buf_tx_votes_data_packed);
        }

        /// only update block producers once every minute, block_timestamp is in half seconds
        if( timestamp.slot - _gstate.last_producer_schedule_update.slot > 120 ) {
            update_elected_producers( timestamp );
        }
    }

    void system_contract::update_elected_producers( block_timestamp block_time ) {
        _gstate.last_producer_schedule_update = block_time;

        auto idx = _producers.get_index<N(prototalvote)>();

        std::vector< std::pair<eosio::producer_key,uint16_t> > top_producers;
        top_producers.reserve(YOSEMITE_MAX_ELECTED_BLOCK_PRODUCER_COUNT);

        for ( auto it = idx.cbegin(); it != idx.cend() && top_producers.size() < YOSEMITE_MAX_ELECTED_BLOCK_PRODUCER_COUNT && 0 <= it->total_votes && it->active() && it->is_trusted_seed; ++it ) {
            top_producers.emplace_back( std::pair<eosio::producer_key,uint16_t>({{it->owner, it->producer_key}, it->location}) );
        }

//       if ( top_producers.size() < _gstate.last_producer_schedule_size ) {
//          return;
//       }

        if (top_producers.empty()) {
            return;
        }

        /// sort by producer name
        std::sort( top_producers.begin(), top_producers.end() );

        std::vector<eosio::producer_key> producers;

        producers.reserve(top_producers.size());
        for( const auto& item : top_producers )
            producers.push_back(item.first);

        bytes packed_schedule = pack(producers);

        if( set_proposed_producers( packed_schedule.data(),  packed_schedule.size() ) >= 0 ) {
            _gstate.last_producer_schedule_size = static_cast<decltype(_gstate.last_producer_schedule_size)>( top_producers.size() );
        }
    }

    void system_contract::regproducer( const account_name producer, const eosio::public_key& producer_key, const std::string& url, uint16_t location ) {
        eosio_assert( url.size() < 512, "url too long" );
        eosio_assert( producer_key != eosio::public_key(), "public key should not be the default value" );
        require_auth( producer );

        auto prod = _producers.find( producer );

        if ( prod != _producers.end() ) {
            _producers.modify( prod, producer, [&]( producer_info& info ){
                info.producer_key = producer_key;
                info.is_active    = true;
                info.url          = url;
                info.location     = location;
            });
        } else {
            _producers.emplace( producer, [&]( producer_info& info ){
                info.owner           = producer;
                info.total_votes     = 0;
                info.producer_key    = producer_key;
                info.is_active       = true;
                info.is_trusted_seed = false;
                info.url             = url;
                info.location        = location;
            });
        }

        // pay transaction fee if not signed by system contract owner
        if (!has_auth(_self)) {
            yosemite::native_token::charge_transaction_fee(producer, YOSEMITE_TX_FEE_OP_NAME_SYSTEM_REG_PRODUCER);
        }
    }

    void system_contract::authproducer( const account_name producer ) {
        require_auth( _self );

        auto prod = _producers.find( producer );

        eosio_assert( prod != _producers.end(), "not found registered block producer" );

        _producers.modify( prod, 0, [&]( producer_info& info ){
            info.is_trusted_seed = true;
        });
    }

    void system_contract::unregprod( const account_name producer ) {
        require_auth( producer );

        const auto& prod = _producers.get( producer, "producer not found" );

        _producers.modify( prod, 0, [&]( producer_info& info ){
            info.deactivate();
        });
    }

    void system_contract::rmvproducer( account_name producer ) {
        require_auth( _self );
        auto prod = _producers.find( producer );
        eosio_assert( prod != _producers.end(), "producer not found" );
        _producers.modify( prod, 0, [&](producer_info& info) {
            info.deactivate();
            info.is_trusted_seed = false;
        });
    }

    using namespace eosio;
    using namespace yosemite;
    void system_contract::claimrewards( const account_name& owner ) {
        require_auth(owner);

        const auto& prod = _producers.get( owner );
        eosio_assert( prod.active(), "producer does not have an active key" );
        eosio_assert( prod.is_trusted_seed, "producer is not trusted seed producer" );

        int64_t tx_fee_accumulated = native_token::get_total_native_token_balance(YOSEMITE_TX_FEE_ACCOUNT);
        eosio_assert( tx_fee_accumulated > 0, "no tx fee accumulated");

        auto ct = current_time();

        eosio_assert( ct - prod.last_claim_time > useconds_per_day, "already claimed rewards within past day" );

        int64_t producer_per_block_pay = 0;
        if( _gstate.total_unpaid_blocks > 0 ) {
            producer_per_block_pay = (tx_fee_accumulated * prod.unpaid_blocks) / _gstate.total_unpaid_blocks;
        }

        _gstate.total_unpaid_blocks -= prod.unpaid_blocks;

        _producers.modify( prod, 0, [&](auto& p) {
            p.last_claim_time = ct;
            p.unpaid_blocks = 0;
        });

        if( producer_per_block_pay > 0 ) {
            INLINE_ACTION_SENDER(yosemite::native_token::ntoken, transfer)
                    (YOSEMITE_NATIVE_TOKEN_ACCOUNT, {YOSEMITE_SYSTEM_ACCOUNT, N(active)},
                     { YOSEMITE_TX_FEE_ACCOUNT, owner, asset(producer_per_block_pay, YOSEMITE_NATIVE_TOKEN_SYMBOL), "producer pay" });
        }
    }

} //namespace yosemitesys
