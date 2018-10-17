/**
 * @file
 * @copyright defined in yosemite-public-blockchain/LICENSE
 */

#include "yx.nftex.hpp"
#include "../yx.nft/yx.nft.hpp"
#include <yosemitelib/yx_asset.hpp>
#include <yosemitelib/system_accounts.hpp>
#include <yosemitelib/hash_fnv1a.hpp>

namespace yosemite {

   void nft_exchange::buy(account_name buyer, const yx_nft &nft, const yx_asset &price, const string &memo) {
      check_param(buyer, nft, price, memo);

      uint64_t sellbook_scope = make_scope(nft.ysymbol);
      sellbook sell_book{get_self(), sellbook_scope};

      const auto &sell_order = sell_book.get(nft.id, "NFT with the specified id is not in the sell order book");
      auto _now = now();
      if (sell_order.expiration <= time_point_sec(_now)) {
         sell_book.erase(sell_order);
         eosio_assert(static_cast<uint32_t>(false), "sell order is expired");
      }
      eosio_assert(static_cast<uint32_t>(sell_order.price == price), "price is different");
      eosio_assert(static_cast<uint32_t>(sell_order.seller != buyer), "seller and buyer are the same");

      // transfer NFT to buyer
      vector<id_type> ids{sell_order.id};
      INLINE_ACTION_SENDER(yosemite::non_native_token::nft, transferid)
            (YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT, {{get_self(),              N(active)},
                                                   {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
             {get_self(), buyer, sell_order.ysymbol.issuer, ids, memo});

      // transfer price to seller
      transfer_token_as_inline(buyer, sell_order.seller, price, memo);
   }

   void nft_exchange::buynt(account_name buyer, const yx_nft &nft, const asset &price, const string &memo) {
      buy(buyer, nft, yx_asset{price, account_name{}}, memo);
   }

   void nft_exchange::sell(account_name seller, const yx_nft &nft,
                           const yx_asset &price, const time_point_sec &expiration, const string &memo) {
      eosio_assert(static_cast<uint32_t>(expiration.utc_seconds > 0), "expiration must be set");
      check_param(seller, nft, price, memo, expiration);

      uint64_t sellbook_scope = make_scope(nft.ysymbol);
      sellbook sell_book{get_self(), sellbook_scope};

      eosio_assert(static_cast<uint32_t>(sell_book.find(nft.id) == sell_book.end()), "already exists");

      sell_book.emplace(get_self(), [&](auto &order) {
         order.id = nft.id;
         order.ysymbol = nft.ysymbol;
         order.seller = seller;
         order.price = price;
         order.expiration = expiration;
      });

      // transfer NFT from seller
      vector<id_type> ids{nft.id};
      INLINE_ACTION_SENDER(yosemite::non_native_token::nft, transferid)
            (YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT, {{seller,                  N(active)},
                                                   {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
             {seller, get_self(), nft.ysymbol.issuer, ids, memo});
   }

   void
   nft_exchange::sellnt(account_name seller, const yx_nft &nft, const asset &price, const time_point_sec &expiration,
                        const string &memo) {
      sell(seller, nft, yx_asset{price, account_name{}}, expiration, memo);
   }

   void nft_exchange::check_param(const account_name &order_account, const yx_nft &nft,
                                  const yx_asset &price, const string &memo,
                                  const time_point_sec &expiration) const {
      require_auth(order_account);
      eosio_assert(static_cast<uint32_t>(is_account(nft.ysymbol.issuer)), "invalid issuer account");
      eosio_assert(static_cast<uint32_t>(price.is_valid()), "invalid price");
      eosio_assert(static_cast<uint32_t>(price.amount > 0), "only positive price is allowed");
      eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

      if (expiration.utc_seconds != 0) {
         uint32_t _now = now();
         eosio_assert(static_cast<uint32_t>(expiration > time_point_sec(_now + 60)), "wrong expiration : already expired or too close to now");
         eosio_assert(static_cast<uint32_t>(expiration <= time_point_sec(_now + MAX_EXPIRATION_SECONDS)),
                      "expiration is too long");
      }
   }

   uint64_t nft_exchange::make_scope(const yx_symbol &nft_symbol) const {
      string sym = nft_symbol.to_string();
      return static_cast<uint64_t>(hash_32_fnv1a(sym.c_str(), static_cast<int>(sym.size())));
   }

   void nft_exchange::cancelsell(const yx_nft &nft) {
      uint64_t scope = make_scope(nft.ysymbol);

      sellbook sell_book(get_self(), scope);
      const auto &order = sell_book.get(nft.id, "NFT with the specified id is not in the sell book");
      require_auth(order.seller);

      sell_book.erase(order);

      // transfer back NFT to seller
      vector<id_type> ids{nft.id};
      INLINE_ACTION_SENDER(yosemite::non_native_token::nft, transferid)
            (YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT, {{YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
             {get_self(), order.seller, nft.ysymbol.issuer, ids, ""});
   }

}

EOSIO_ABI(yosemite::nft_exchange, (buy)(sell)(buynt)(sellnt)(cancelsell))
