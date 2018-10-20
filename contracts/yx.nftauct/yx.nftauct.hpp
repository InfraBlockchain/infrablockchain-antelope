/**
 *  @copyright defined in LICENSE
 */

#pragma once

#include <eosiolib/time.hpp>
#include <string>
#include <yosemitelib/yx_asset.hpp>
#include <yosemitelib/yx_contract.hpp>

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
    };

    class nft_auction : public yx_contract {
    public:
        explicit nft_auction(account_name self) : yx_contract(self) {
        }

        void create(const itemid &item_id, const yx_asset &start_price, const asset &end_price, const asset &min_inc_price,
                    const string &iteminfo, const time_point_sec &expiration, uint8_t options);
        void bid(const itemid &item_id, account_name bidder, const yx_asset &token);
        void complete(const itemid &item_id);
        void remove(const itemid &item_id);
    };

    /* scope = creator */
    struct auction_item_info {
        uint64_t sequence = 0; /* provided sequence by the item creator */
        yx_asset start_price{};
        int64_t end_amount = 0;
        int64_t min_inc_amount = 0;
        time_point_sec expiration{};
        string iteminfo{};
        uint8_t options = 0;
        account_name last_bidder{};
        int64_t last_bid_amount{};
        time_point_sec last_bid_time{};

        uint64_t primary_key() const { return sequence; }
    };

    using auction_items_index = eosio::multi_index<N(nftauctitems), auction_item_info>;
}
