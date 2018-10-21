/**
 * @file
 * @copyright defined in yosemite-public-blockchain/LICENSE
 */

#include "yx.nftauct.hpp"
#include "../yx.nft/yx.nft.hpp"
#include <eosiolib/transaction.hpp>
#include <yosemitelib/native_token.hpp>
#include <yosemitelib/token.hpp>
#include <yosemitelib/transaction_fee.hpp>

namespace yosemite {
   using namespace yosemite::native_token;

   static const uint32_t MIN_EXPIRATION_IN_SEC = 10 * 60; // 10 minutes
   static const uint32_t MAX_EXPIRATION_IN_SEC = 30 * 24 * 3600; // 30 days
   static const uint32_t MIN_EXPECTED_BID_TURNS = 1;

   void nft_auction::create(const yx_nft &nft,
                            account_name nft_owner,
                            const yx_asset &start_price,
                            const asset &end_price, // expected winning price which completes the auction automatically
                            const asset &min_inc_price,
                            const string &iteminfo,
                            const time_point_sec &expiration,
                            uint8_t options) {
      require_auth(nft_owner);
      eosio_assert(static_cast<uint32_t>(start_price.is_valid()), "invalid start_price");
      eosio_assert(static_cast<uint32_t>(start_price.amount > 0), "start_price must be positive");
      if (!start_price.is_native(false)) {
         eosio_assert(static_cast<uint32_t>(
                            yosemite::non_native_token::does_token_exist(YOSEMITE_USER_TOKEN_ACCOUNT,
                                                                         start_price.get_yx_symbol())),
                      "token does not exist");
      }
      eosio_assert(static_cast<uint32_t>(end_price.is_valid()), "invalid end_price");
      eosio_assert(static_cast<uint32_t>(end_price.amount >= 0), "end_price must be non-negative");
      eosio_assert(static_cast<uint32_t>(min_inc_price.is_valid()), "invalid min_inc_price");
      eosio_assert(static_cast<uint32_t>(min_inc_price.amount >= 0), "min_inc_price must be non-negative");
      if (end_price.amount > 0) {
         int64_t amount_diff = end_price.amount - start_price.amount;
         if (min_inc_price.amount > 0) {
            eosio_assert(static_cast<uint32_t>(amount_diff >= min_inc_price.amount * MIN_EXPECTED_BID_TURNS),
                         "end_price must be the certain amount");
         } else {
            eosio_assert(static_cast<uint32_t>(end_price.amount > start_price.amount),
                         "end_price must be bigger than start_price");
         }
      }
      //check that creator can receive the token
      if (start_price.is_native(true)) {
         check_identity_auth_for_transfer(nft_owner, NTOKEN_KYC_RULE_TYPE_TRANSFER_RECEIVE);
      } else {
         eosio_assert(static_cast<uint32_t>(non_native_token::can_account_receive_token(
                            YOSEMITE_USER_TOKEN_ACCOUNT, start_price.get_yx_symbol(), nft_owner)),
                      "creator cannot receive the token");
      }

      eosio_assert(static_cast<uint32_t>(iteminfo.size() <= 256), "iteminfo is too long");

      auto _now = now();
      auto time_diff = expiration.utc_seconds - _now;
      eosio_assert(static_cast<uint32_t>(time_diff >= MIN_EXPIRATION_IN_SEC),
                   "auction already is expired or expiration time is too close to now");
      eosio_assert(static_cast<uint32_t>(time_diff <= MAX_EXPIRATION_IN_SEC), "expiration time is too long");
      eosio_assert(static_cast<uint32_t>(options == 0), "options by creator is currently reserved");

      auction_items_index aucitems_idx{get_self(), nft.ysymbol.issuer};
      eosio_assert(static_cast<uint32_t>(aucitems_idx.find(nft.id) == aucitems_idx.end()),
                   "NFT already exists");

      aucitems_idx.emplace(get_self(), [&](auto &i) {
         i.id = nft.id;
         i.nft_owner = nft_owner;
         i.start_price = start_price;
         i.end_price = end_price;
         i.min_inc_price = min_inc_price;
         i.expiration = expiration;
         i.iteminfo = iteminfo;
         i.options = options;
      });

      // transfer NFT from seller
      vector<id_type> ids{nft.id};
      INLINE_ACTION_SENDER(yosemite::non_native_token::nft, transferid)
            (YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT, {{nft_owner,               N(active)},
                                                   {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
             {nft_owner, get_self(), nft.ysymbol.issuer, ids, ""});

      // create deferred transaction to call complete() of this contract
      action deferred_act{};
      deferred_act.account = get_self();
      deferred_act.name = N(complete);
      deferred_act.authorization = vector<permission_level>{permission_level{get_self(), N(active)}};

      transaction tx(expiration + 600);
      tx.actions.emplace_back(deferred_act);
      tx.delay_sec = time_diff - 1;
      tx.send(nft.get_orderkey(), get_self());

      charge_transaction_fee(nft_owner, YOSEMITE_TX_FEE_OP_NAME_AUCTION_CREATE);
   }

   void nft_auction::bid(const yx_nft &nft, account_name bidder, const yx_asset &price) {
      require_auth(bidder);
      eosio_assert(static_cast<uint32_t>(price.is_valid()), "invalid price");
      eosio_assert(static_cast<uint32_t>(price.amount > 0), "price must be positive");

      auction_items_index aucitems_idx{get_self(), nft.ysymbol.issuer};
      const auto &info = aucitems_idx.get(nft.id, "auction item does not exist");

      const time_point_sec &bidtime = time_point_sec(now());
      eosio_assert(static_cast<uint32_t>(info.expiration > bidtime), "auction has been expired");

      eosio_assert(static_cast<uint32_t>(info.start_price.get_yx_symbol() == price.get_yx_symbol()),
                   "different type of token is used for price");
      int64_t increased_amount = price.amount - info.last_bid_price.amount;
      if (info.min_inc_price.amount > 0) {
         eosio_assert(static_cast<uint32_t>(increased_amount >= info.min_inc_price.amount),
                      "bid price is not increased properly");
      } else {
         eosio_assert(static_cast<uint32_t>(increased_amount > 0),
                      "bid price is not increased from previous one");
      }

      if (info.last_bidder != 0) {
         //transfer bid_price back to the previous bidder
         transfer_token_as_inline(get_self(), info.last_bidder, yx_asset{info.last_bid_price, price.issuer});
      }

      if (price.amount >= info.end_price.amount) {
         // auction is completed!
         do_complete(nft, info, bidder, bidder);

         aucitems_idx.erase(info);
         cancel_deferred(nft.get_orderkey()); // cancel deferrred complete action
      } else {
         aucitems_idx.modify(info, 0, [&](auto &i) {
            i.last_bidder = bidder;
            i.last_bid_price = asset{price.amount, price.symbol};
            i.last_bid_time = bidtime;
         });

         //escrow bid_price from bidder to this contract
         transfer_token_as_inline(bidder, get_self(), price);
      }

      charge_transaction_fee(bidder, YOSEMITE_TX_FEE_OP_NAME_AUCTION_BID);
   }

   void nft_auction::do_complete(const yx_nft &nft, const auction_item_info &info, account_name last_bidder, account_name from) {
      // transfer NFT to the last bidder who is a winner
      vector<id_type> ids{nft.id};
      INLINE_ACTION_SENDER(yosemite::non_native_token::nft, transferid)
            (YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT, {{get_self(),              N(active)},
                                                   {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
             {get_self(), last_bidder, nft.ysymbol.issuer, ids, ""});

      // send escrowed token to the previous NFT owner
      yx_asset last_bid_token{info.last_bid_price, info.get_price_token_issuer()};
      transfer_token_as_inline(from, info.nft_owner, last_bid_token);
   }

   // called as deferred transaction
   void nft_auction::complete(const yx_nft &nft) {
      require_auth(get_self());

      auction_items_index aucitems_idx{get_self(), nft.ysymbol.issuer};
      const auto &info = aucitems_idx.get(nft.id, "auction item does not exist");

      do_complete(nft, info, info.last_bidder, get_self());
      aucitems_idx.erase(info);
   }

   void nft_auction::remove(const yx_nft &nft) {
      auction_items_index aucitems_idx{get_self(), nft.ysymbol.issuer};
      const auto &info = aucitems_idx.get(nft.id, "auction item does not exist");

      eosio_assert(static_cast<uint32_t>(info.last_bidder == 0), "bidding is already started");
      require_auth(info.nft_owner);

      aucitems_idx.erase(info);

      charge_transaction_fee(info.nft_owner, YOSEMITE_TX_FEE_OP_NAME_AUCTION_REMOVE);
   }
}

EOSIO_ABI(yosemite::nft_auction, (create)(bid)(complete)(remove)
)
