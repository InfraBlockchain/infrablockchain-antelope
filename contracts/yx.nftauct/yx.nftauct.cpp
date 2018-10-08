/**
 * @copyright defined in yosemite-public-blockchain/LICENSE
 */

#include "yx.nftauct.hpp"
#include <eosiolib/transaction.hpp>
#include <yosemitelib/native_token.hpp>
#include <yosemitelib/token.hpp>
#include <yosemitelib/transaction_fee.hpp>

namespace yosemite {
   using namespace yosemite::native_token;

    static const uint32_t MIN_EXPIRATION_IN_SEC = 10 * 60; // 10 minutes
    static const uint32_t MAX_EXPIRATION_IN_SEC = 30 * 24 * 3600; // 30 days
    static const uint32_t MIN_EXPECTED_BID_TURNS = 10;

    void ntf_auction::create(const itemid &item_id,
                             const yx_asset &start_price,
                             const asset &end_price, // expected winning price which completes the auction automatically
                             const asset &min_inc_price,
                             const string &iteminfo,
                             const time_point_sec &expiration, uint8_t options) {
        require_auth(item_id.creator);
        eosio_assert(static_cast<uint32_t>(start_price.is_valid()), "invalid start_price");
        eosio_assert(static_cast<uint32_t>(start_price.amount > 0), "start_price must be positive");
        if (!start_price.is_native(false)) {
            eosio_assert(static_cast<uint32_t>(
                  yosemite::non_native_token::does_token_exist(YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT, start_price.get_yx_symbol())),
                  "token does not exist");
        }
        eosio_assert(static_cast<uint32_t>(end_price.is_valid()), "invalid end_price");
        eosio_assert(static_cast<uint32_t>(end_price.amount >= 0), "end_price must be non-negative");
        eosio_assert(static_cast<uint32_t>(min_inc_price.is_valid()), "invalid min_inc_price");
        eosio_assert(static_cast<uint32_t>(min_inc_price.amount >= 0), "min_inc_price must be positive");
        if (end_price.amount > 0) {
            int64_t amount_diff = end_price.amount - start_price.amount;
            if (min_inc_price.amount > 0) {
                eosio_assert(static_cast<uint32_t>(amount_diff > min_inc_price.amount * MIN_EXPECTED_BID_TURNS),
                             "complete_price must be the certain amount");
            } else {
                eosio_assert(static_cast<uint32_t>(amount_diff > MIN_EXPECTED_BID_TURNS),
                             "end_price must be bigger than start_price with the certain amount");
            }
        }
        //TODO:check that creator can receive the token

        eosio_assert(static_cast<uint32_t>(iteminfo.size() <= 256), "iteminfo is too long");

        auto _now = now();
        auto time_diff = expiration.utc_seconds - _now;
        eosio_assert(static_cast<uint32_t>(time_diff >= MIN_EXPIRATION_IN_SEC),
                     "auction already is expired or expiration time is too close to now");
        eosio_assert(static_cast<uint32_t>(time_diff <= MAX_EXPIRATION_IN_SEC), "expiration time is too long");
        eosio_assert(static_cast<uint32_t>(options == 0), "options by creator is currently reserved");

        auction_items_index aucitems_idx{get_self(), item_id.creator};
        eosio_assert(static_cast<uint32_t>(aucitems_idx.find(item_id.sequence) == aucitems_idx.end()),
                     "auction item with the sequence already exists");

        aucitems_idx.emplace(get_self(), [&](auto &i) {
            i.sequence = item_id.sequence;
            i.start_price = start_price;
            i.min_inc_amount = min_inc_price.amount;
            i.end_amount = end_price.amount;
            i.iteminfo = iteminfo;
            i.expiration = expiration;
            i.options = options;
        });

        charge_transaction_fee(item_id.creator, YOSEMITE_TX_FEE_OP_NAME_AUCTION_CREATE);

        // create deferred transaction to call complete
        action deferred_act{};
        deferred_act.account = get_self();
        deferred_act.name = N(complete);
        deferred_act.authorization = vector<permission_level>{permission_level{get_self(), N(active)}};

        transaction tx(expiration + 600);
        tx.actions.emplace_back(deferred_act);
        tx.delay_sec = time_diff - 1;
        tx.send(item_id.to_uint128(), get_self());
    }

    void ntf_auction::bid(const itemid &item_id, account_name bidder, const yx_asset &bid_price) {
        require_auth(bidder);
        eosio_assert(static_cast<uint32_t>(bid_price.is_valid()), "invalid bid_price");
        eosio_assert(static_cast<uint32_t>(bid_price.amount > 0), "bid_price must be positive");

        auction_items_index aucitems_idx{get_self(), item_id.creator};
        const auto &info = aucitems_idx.get(item_id.sequence, "auction item does not exist");
        eosio_assert(static_cast<uint32_t>(!info.completed), "auction has been completed");

        const time_point_sec &bidtime = time_point_sec(now());
        eosio_assert(static_cast<uint32_t>(info.expiration > bidtime), "auction has been expired");

        eosio_assert(static_cast<uint32_t>(info.start_price.get_yx_symbol() == bid_price.get_yx_symbol()), "different type of token is used");
        int64_t increased_amount = bid_price.amount - info.last_bid_amount;
        if (info.min_inc_amount > 0) {
            eosio_assert(static_cast<uint32_t>(increased_amount >= info.min_inc_amount),
                         "bid amount is not increased properly");
        } else {
            eosio_assert(static_cast<uint32_t>(increased_amount > 0),
                         "bid amount is not increased properly");
        }

        //TODO:check complete

        aucitems_idx.modify(info, 0, [&](auto &i) {
            i.last_bidder = bidder;
            i.last_bid_amount = bid_price.amount;
            i.last_bid_time = bidtime;
        });

        //escrow bid_price from bidder to this contract
        transfer_token_as_inline(bidder, get_self(), bid_price);

        charge_transaction_fee(item_id.creator, YOSEMITE_TX_FEE_OP_NAME_AUCTION_BID);
    }

    void ntf_auction::transfer_token_as_inline(account_name from, account_name to, const yx_asset &token) {
        if (token.is_native()) {
            if (token.issuer == 0) {
                INLINE_ACTION_SENDER(yosemite::ntoken, transfer)
                        (YOSEMITE_NATIVE_TOKEN_ACCOUNT, {{from, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                         {from, to, token, ""});
            } else {
                INLINE_ACTION_SENDER(yosemite::ntoken, ntransfer)
                        (YOSEMITE_NATIVE_TOKEN_ACCOUNT, {{from, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                         {from, to, token, ""});
            }
        } else {
            INLINE_ACTION_SENDER(yosemite::non_native_token::token, transfer)
                    (YOSEMITE_USER_TOKEN_ACCOUNT, {{from, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                     {from, to, token, ""});
        }
    }

    void ntf_auction::complete(const itemid &item_id) {
        require_auth(get_self());

        auction_items_index aucitems_idx{get_self(), item_id.creator};
        const auto &info = aucitems_idx.get(item_id.sequence, "auction item does not exist");
        eosio_assert(static_cast<uint32_t>(!info.completed), "auction has been completed");

        aucitems_idx.modify(info, 0, [&](auto &i) {
            i.completed = true;
        });

        // send escrowed token to the auction item creator
        //TODO:give bidder confirmation time
        //TODO:if bidder does not confirm in the time, give penalty to bidder
        yx_asset last_bid_token{info.last_bid_amount, info.start_price.get_yx_symbol()};
        transfer_token_as_inline(get_self(), item_id.creator, last_bid_token);
    }

    void ntf_auction::remove(const itemid &item_id) {
        require_auth(item_id.creator);

        auction_items_index aucitems_idx{get_self(), item_id.creator};
        const auto &info = aucitems_idx.get(item_id.sequence, "auction item does not exist");
        eosio_assert(static_cast<uint32_t>(info.completed), "auction has not been completed");
        eosio_assert(static_cast<uint32_t>(info.expiration < time_point_sec(now())), "auction has not been expired");

        aucitems_idx.erase(info);

        charge_transaction_fee(item_id.creator, YOSEMITE_TX_FEE_OP_NAME_AUCTION_REMOVE);
    }
}

EOSIO_ABI(yosemite::ntf_auction, (create)(bid)(complete)(remove)
)
