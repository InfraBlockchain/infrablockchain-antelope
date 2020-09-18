/**
 *  @file contracts/yx.system/yx.block_producer.cpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */

#include "yx.system.hpp"

#include <infrablockchainlib/system_accounts.hpp>
#include <infrablockchainlib/transaction_vote.h>

#include <eosiolib/system.h>
#include <eosiolib/memory.hpp>

#include <cmath>

namespace yosemitesys {

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

        /// only update block producers once every minute, block_timestamp is in half seconds
        if( timestamp.slot - _gstate.last_producer_schedule_update.slot > 120 ) {
            update_elected_producers( timestamp );
        }
    }

    const uint32_t top_tx_vote_receiver_list_fetch_count = 30;

    void system_contract::update_elected_producers( block_timestamp block_time ) {
       _gstate.last_producer_schedule_update = block_time;

       std::vector< std::pair<eosio::producer_key,uint16_t> > top_producers;
       top_producers.reserve( YOSEMITE_MAX_ELECTED_BLOCK_PRODUCER_COUNT );

       uint32_t offset_rank = 0;
       uint32_t fetched_list_packed_size = get_top_transaction_vote_receivers(nullptr, 0, offset_rank, top_tx_vote_receiver_list_fetch_count);

       while ( fetched_list_packed_size > 0 /*&& top_producers.size() < YOSEMITE_MAX_ELECTED_BLOCK_PRODUCER_COUNT*/ ) {
          uint32_t fetched_cnt = fetched_list_packed_size / sizeof(struct yosemite_tx_vote_stat_for_account);

          /// allocate buffer memory through memory manager
          char *buf_top_tx_vote_receivers_data_packed = static_cast<char *>(malloc(fetched_list_packed_size));
          eosio_assert(buf_top_tx_vote_receivers_data_packed != nullptr, "malloc failed for top tx vote receiver list packed data");

          // fetch top tx vote account list from chain core
          get_top_transaction_vote_receivers(buf_top_tx_vote_receivers_data_packed, fetched_list_packed_size, offset_rank, top_tx_vote_receiver_list_fetch_count);

          yosemite_tx_vote_stat_for_account *tx_vote_receiver_stat_arr = reinterpret_cast<struct yosemite_tx_vote_stat_for_account *>(buf_top_tx_vote_receivers_data_packed);

          for (uint32_t i = 0; i < fetched_cnt; i++) {
             yosemite_tx_vote_stat_for_account &tx_vote_stat_for_account = tx_vote_receiver_stat_arr[i];

             auto producer_itr = _producers.find( tx_vote_stat_for_account.account );
             if (producer_itr != _producers.cend()) {
                if ( top_producers.size() < YOSEMITE_MAX_ELECTED_BLOCK_PRODUCER_COUNT && producer_itr->active() && producer_itr->is_trusted_seed ) {
                   top_producers.emplace_back(std::pair<eosio::producer_key, uint16_t>({{producer_itr->owner, producer_itr->producer_key}, producer_itr->location}));

                   if ( top_producers.size() == YOSEMITE_MAX_ELECTED_BLOCK_PRODUCER_COUNT ) {
                      break;
                   }
                }
             }
          }

          /// deallocate buffer memory
          free(buf_top_tx_vote_receivers_data_packed);

          if ( top_producers.size() < YOSEMITE_MAX_ELECTED_BLOCK_PRODUCER_COUNT && fetched_cnt == top_tx_vote_receiver_list_fetch_count ) {
             offset_rank += fetched_cnt;
             fetched_list_packed_size = get_top_transaction_vote_receivers(nullptr, 0, offset_rank, top_tx_vote_receiver_list_fetch_count);
          } else {
             // if the fetched item count is less than requested fetch count, there is no more transaction vote receiver list items on blockchain core,
             // and if max number of block producers are already selected as top producers, then there is no need to fetch additional transaction vote receiver list.
             fetched_list_packed_size = 0;
          }
       }

       if ( top_producers.size() < _gstate.last_producer_schedule_size ) {
          return;
       }

//       if (top_producers.empty()) {
//          return;
//       }

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
                info.total_votes_weight = 0;
                info.producer_key    = producer_key;
                info.is_active       = true;
                info.is_trusted_seed = false;
                info.url             = url;
                info.location        = location;
            });
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

        eosio_assert( false, "claimrewards not yet supported");
    }

} //namespace yosemitesys
