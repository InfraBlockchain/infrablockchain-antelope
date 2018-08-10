/**
 *  @copyright defined in LICENSE
 */

#pragma once

#include <eosiolib/time.hpp>
#include <string>
#include <yosemitelib/yx_asset.hpp>

namespace yosemite {

    using namespace eosio;
    using std::string;
    using boost::container::flat_set;

    /* auction item id used as abi input parameter */
    struct itemid {
        account_name creator;
        uint64_t sequence;

        uint128_t to_uint128() const {
            uint128_t result(creator);
            result <<= 64;
            return result | sequence;
        }

        EOSLIB_SERIALIZE(itemid, (creator)(sequence))
    };

    class auction : public contract {
    public:
        explicit auction(account_name self) : contract(self) {
        }

        void create(const itemid &item_id, const yx_asset &start_price, const asset &end_price, const asset &min_inc_price,
                    const string &iteminfo, const time_point_sec &expiration, uint8_t options);
        void wpcreate(const itemid &item_id, const yx_asset &start_price, const asset &end_price, const asset &min_inc_price,
                      const string &iteminfo, const time_point_sec &expiration, uint8_t options, account_name payer);
        void bid(const itemid &item_id, account_name bidder, const yx_asset &token);
        void wpbid(const itemid &item_id, account_name bidder, const yx_asset &bid_price, account_name payer);
        void complete(const itemid &item_id);
        void remove(const itemid &item_id);

    private:
        void transfer_token_as_inline(account_name from, account_name to, const yx_asset &token);
    };

    /* scope = creator */
    struct auction_item_info {
        uint64_t sequence = 0; /* provided sequence by the item creator */
        yx_asset start_price{};
        int64_t end_amount = 0;
        int64_t min_inc_amount = 0;
        uint16_t confirm_wait_duration; // default: days
        time_point_sec expiration{};
        string iteminfo{};
        uint8_t options = 0;
        account_name last_bidder{};
        int64_t last_bid_amount{};
        time_point_sec last_bid_time{}; // includes the indices to signers vector
        bool completed = false;

        uint64_t primary_key() const { return sequence; }
    };

    typedef eosio::multi_index<N(aucitems), auction_item_info> auction_items_index;
}
