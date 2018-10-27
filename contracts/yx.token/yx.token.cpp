#include <yosemitelib/system_accounts.hpp>
#include <yosemitelib/transaction_fee.hpp>
#include <yosemitelib/native_token.hpp>
#include "yx.token.hpp"

namespace yosemite { namespace non_native_token {

   void yx_token::inner_check_create_parameters(const yx_symbol &ysymbol, uint16_t /*can_set_options*/) {
      eosio_assert(static_cast<uint32_t>(ysymbol.precision() >= 2), "token precision must be equal or larger than 2");
   }

   void yx_token::grantissue(const account_name &to, const yx_asset &limit) {
      eosio_assert(static_cast<uint32_t>(limit.is_valid()), "invalid limit");
      eosio_assert(static_cast<uint32_t>(limit.amount >= 0), "must be non-negative limit");
      eosio_assert(static_cast<uint32_t>(!limit.is_native(false)), "cannot grant issue with the native token");
      eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");
      require_auth(limit.issuer);

      delegated_issue_table delissue_tbl{get_self(), limit.symbol.value};
      auto second_index = delissue_tbl.get_index<N(secondary)>();
      const uint128_t &second_key = to_uint128(limit.get_yx_symbol().issuer, to);
      const auto &issue_info = second_index.find(second_key);

      if (issue_info == second_index.end()) {
         delissue_tbl.emplace(get_self(), [&](auto &holder) {
            holder.id = delissue_tbl.available_primary_key();
            holder.account = to;
            holder.limit = limit;
         });
      } else {
         if (limit.amount == 0 && issue_info->issued.amount == 0) {
            second_index.erase(issue_info);
         } else {
            second_index.modify(issue_info, 0, [&](auto &holder) {
               holder.limit = limit;
            });
         }
      }

      charge_fee(limit.issuer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_GRANT_ISSUE);
   }

   void yx_token::issuebyuser(const account_name &user, const yx_asset &token, const string &memo) {
      check_issue_parameters(user, token, memo);
      check_transfer_rules(user, user, token);
      eosio_assert(static_cast<uint32_t>(user != token.issuer), "user and token issuer must be different");
      require_auth(user);

      delegated_issue_table delissue_tbl{get_self(), token.symbol.value};
      auto second_index = delissue_tbl.get_index<N(secondary)>();
      const uint128_t &second_key = to_uint128(token.get_yx_symbol().issuer, user);
      const auto &issue_info = second_index.find(second_key);

      eosio_assert(static_cast<uint32_t>(issue_info != second_index.end()), "user issue is not granted by the token issuer");
      eosio_assert(static_cast<uint32_t>(issue_info->limit.amount - issue_info->issued.amount >= token.amount), "issue limit is reached");

      second_index.modify(issue_info, 0, [&](auto &holder) {
         holder.issued.amount += token.amount;
      });

      increase_supply(token);
      add_token_balance(user, token);

      charge_fee(token.issuer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_ISSUE_BY_USER);
   }

   void yx_token::changeissued(const account_name &user, const yx_asset &delta, bool decrease) {
      check_issue_parameters(user, delta, "");
      eosio_assert(static_cast<uint32_t>(user != delta.issuer), "user and token issuer must be different");
      require_auth(delta.issuer);

      delegated_issue_table delissue_tbl{get_self(), delta.symbol.value};
      auto second_index = delissue_tbl.get_index<N(secondary)>();
      const uint128_t &second_key = to_uint128(delta.get_yx_symbol().issuer, user);
      const auto &issue_info = second_index.find(second_key);

      eosio_assert(static_cast<uint32_t>(issue_info != second_index.end()), "user issue is not granted by the token issuer");

      bool erase = false;

      second_index.modify(issue_info, 0, [&](auto &holder) {
         if (decrease) {
            holder.issued.amount -= delta.amount;
            eosio_assert(static_cast<uint32_t>(holder.issued.amount >= 0), "wrong decrease delta");
            erase = (holder.limit.amount == 0) && (holder.issued.amount == 0);
         } else {
            holder.issued.amount += delta.amount;
            eosio_assert(
                  static_cast<uint32_t>(holder.issued.amount >= 0 && holder.issued.amount <= holder.limit.amount),
                  "wrong decrease delta");
         }
      });
      if (erase) {
         second_index.erase(issue_info);
      }

      charge_fee(delta.issuer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_CHANGE_ISSUED);
   }

   void yx_token::increase_supply(const yx_asset &token) const {
      stats stats_table{get_self(), token.symbol.value};
      const auto &tstats = stats_table.get(token.issuer, "token is not yet created");

      stats_table.modify(tstats, 0, [&](auto &s) {
         s.supply.amount += token.amount;
         eosio_assert(static_cast<uint32_t>(s.supply.amount > 0 && s.supply.amount <= asset::max_amount),
                      "token amount cannot be more than 2^62 - 1");
      });
   }

   void yx_token::issue(const account_name &to, const yx_asset &token, const string &memo) {
      check_issue_parameters(to, token, memo);
      require_auth(token.issuer);

      increase_supply(token);
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
         charge_fee(from, YOSEMITE_TX_FEE_OP_NAME_TOKEN_TRANSFER);
      }
   }

}}

EOSIO_ABI(yosemite::non_native_token::yx_token, (create)(issue)(redeem)(transfer)(setkycrule)(setoptions)(freezeacc)(grantissue)(issuebyuser)(changeissued)
)
