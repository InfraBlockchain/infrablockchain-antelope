/**
 * @file
 * @copyright defined in yosemite-public-blockchain/LICENSE
 */
#include "yx.escrow.hpp"
#include <yosemitelib/system_accounts.hpp>
#include <yosemitelib/transaction_fee.hpp>

namespace yosemite {

   void
   token_escrow_contract::escrow(account_name thirdparty, uint64_t id, account_name from, account_name to,
                                 const yx_asset &token, const string &memo) {
      eosio_assert(static_cast<uint32_t>(token.is_valid()), "invalid token");
      eosio_assert(static_cast<uint32_t>(token.amount > 0), "must transfer positive token");
      eosio_assert(static_cast<uint32_t>(token.get_yx_symbol().precision() >= 2), "only tokens issued via yx.ntoken or yx.token are supported");
      eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");
      eosio_assert(static_cast<uint32_t>(from != thirdparty), "from and thirdparty account cannot be the same");
      eosio_assert(static_cast<uint32_t>(to != thirdparty), "to and thirdparty account cannot be the same");
      eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");
      require_auth(thirdparty);
      require_auth(from);
      eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");

      escrow_info_index escrow_idx{get_self(), thirdparty};
      eosio_assert(static_cast<uint32_t>(escrow_idx.find(id) == escrow_idx.end()),
                   "escrow info with the id already exists");

      escrow_idx.emplace(get_self(), [&](auto &i) {
         i.id = id;
         i.token = token;
         i.from = from;
         i.to = to;
         i.memo = memo;
      });

      transfer_token_as_inline(from, thirdparty, token, memo);

      native_token::charge_transaction_fee(thirdparty, YOSEMITE_TX_FEE_OP_NAME_ESCROW_ESCROW);
   }

   void token_escrow_contract::escrownt(account_name thirdparty, uint64_t id, account_name from, account_name to,
                                        const eosio::asset &amount, const string &memo) {
      eosio_assert(static_cast<uint32_t>(amount.symbol.value == YOSEMITE_NATIVE_TOKEN_SYMBOL), "only native token is supported; use escrow operation instead");
      escrow(thirdparty, id, from, to, yx_asset{amount, account_name{}}, memo);
   }

   void token_escrow_contract::proceed(account_name thirdparty, uint64_t id) {
      require_auth(thirdparty);

      escrow_info_index escrow_idx{get_self(), thirdparty};
      const auto &info = escrow_idx.get(id, "escrow info does not exist");

      transfer_token_as_inline(thirdparty, info.to, info.token, info.memo);

      escrow_idx.erase(info);

      native_token::charge_transaction_fee(thirdparty, YOSEMITE_TX_FEE_OP_NAME_ESCROW_PROCEED);
   }

   void token_escrow_contract::cancel(account_name thirdparty, uint64_t id) {
      require_auth(thirdparty);

      escrow_info_index escrow_idx{get_self(), thirdparty};
      const auto &info = escrow_idx.get(id, "escrow info does not exist");

      // return back to 'from' account
      transfer_token_as_inline(thirdparty, info.from, info.token, info.memo);

      escrow_idx.erase(info);

      native_token::charge_transaction_fee(thirdparty, YOSEMITE_TX_FEE_OP_NAME_ESCROW_CANCEL);
   }

   void token_escrow_contract::transfer_token_as_inline(account_name from, account_name to, const yx_asset &token, const string &memo) {
      if (token.is_native()) {
         if (token.issuer == 0) {
            INLINE_ACTION_SENDER(yosemite::native_token::ntoken, transfer)
                  (YOSEMITE_NATIVE_TOKEN_ACCOUNT, {{from,                    N(active)},
                                                   {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                   {from, to, token, memo});
         } else {
            INLINE_ACTION_SENDER(yosemite::native_token::ntoken, ntransfer)
                  (YOSEMITE_NATIVE_TOKEN_ACCOUNT, {{from,                    N(active)},
                                                   {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                   {from, to, token, memo});
         }
      } else {
         INLINE_ACTION_SENDER(yosemite::non_native_token::token, transfer)
               (YOSEMITE_USER_TOKEN_ACCOUNT, {{from,                    N(active)},
                                              {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                {from, to, token, memo});
      }
   }
}

EOSIO_ABI(yosemite::token_escrow_contract, (escrow)(escrownt)(proceed)(cancel))
