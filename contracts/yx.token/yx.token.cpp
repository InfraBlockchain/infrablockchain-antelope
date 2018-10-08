#include <yosemitelib/system_accounts.hpp>
#include <yosemitelib/transaction_fee.hpp>
#include <yosemitelib/native_token.hpp>
#include "yx.token.hpp"

namespace yosemite { namespace non_native_token {

   void yx_token::inner_check_create_parameters(const yx_symbol &ysymbol, uint16_t /*can_set_options*/) {
      eosio_assert(static_cast<uint32_t>(ysymbol.precision() >= 2), "token precision must be equal or larger than 2");
   }

   void yx_token::issue(const account_name &to, const yx_asset &token, const string &memo) {
      check_issue_parameters(to, token, memo);

      stats stats_table(get_self(), token.symbol.value);
      const auto &tstats = stats_table.get(token.issuer, "token is not yet created");

      stats_table.modify(tstats, 0, [&](auto &s) {
         s.supply.amount += token.amount;
         eosio_assert(static_cast<uint32_t>(s.supply.amount > 0 && s.supply.amount <= asset::max_amount),
                      "token amount cannot be more than 2^62 - 1");
      });

      add_token_balance(token.issuer, token);

      if (to != token.issuer) {
         INLINE_ACTION_SENDER(token, transfer)
            (get_self(),
             {{token.issuer, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
             {token.issuer, to, token, memo});
      }

      charge_fee(token.issuer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_ISSUE);
   }

   void yx_token::redeem(const yx_asset &token, const string &memo) {
      eosio_assert(static_cast<uint32_t>(token.is_valid()), "invalid token");
      eosio_assert(static_cast<uint32_t>(token.amount > 0), "must be positive token");
      eosio_assert(static_cast<uint32_t>(!token.is_native(false)), "cannot redeem the native token with this action");
      eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");
      require_auth(token.issuer);

      stats stats_table(get_self(), token.symbol.value);
      const auto &tstats = stats_table.get(token.issuer, "token is not yet created");
      eosio_assert(static_cast<uint32_t>(token.amount <= tstats.supply.amount),
                   "redeem token exceeds supply amount");

      stats_table.modify(tstats, 0, [&](auto &s) {
         s.supply.amount -= token.amount;
      });

      sub_token_balance(token.issuer, token);

      charge_fee(token.issuer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_REDEEM);
   }

   void yx_token::transfer(account_name from, account_name to, const yx_asset &token, const string &memo) {
      wptransfer(from, to, token, from, memo);
   }

   void yx_token::wptransfer(account_name from, account_name to, const yx_asset &token, account_name payer, const string &memo) {
      bool is_auth_by_sysaccount = has_auth(YOSEMITE_SYSTEM_ACCOUNT);
      if (!is_auth_by_sysaccount) {
         check_transfer_parameters(from, to, token, memo);
      }
      check_transfer_rules(from, to, token);

      require_recipient(from);
      require_recipient(to);

      sub_token_balance(from, token);
      add_token_balance(to, token);

      if (!is_auth_by_sysaccount) {
         charge_fee(payer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_TRANSFER);
      }
   }

}}

EOSIO_ABI(yosemite::non_native_token::yx_token, (create)(issue)(redeem)(transfer)(wptransfer)(setkycrule)(setoptions)(freezeacc)
)
