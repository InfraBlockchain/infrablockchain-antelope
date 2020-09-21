#include <infrablockchainlib/system_accounts.hpp>
#include <infrablockchainlib/transaction_fee.hpp>
#include <infrablockchainlib/native_token.hpp>
#include "yx.token.hpp"

using namespace infrablockchain::identity;

namespace yosemite { namespace non_native_token {

   void yx_token::inner_check_create_parameters(const yx_symbol &ysymbol, uint16_t /*can_set_options*/) {
      eosio_assert(static_cast<uint32_t>(ysymbol.precision() >= 2), "token precision must be equal or larger than 2");
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
         SEND_INLINE_ACTION(*this, transfer, {{token.issuer, N(active)}, {INFRABLOCKCHAIN_SYSTEM_ACCOUNT, N(active)}},
             {token.issuer, to, token, memo});
      }

      charge_fee(token.issuer, INFRABLOCKCHAIN_TX_FEE_OP_NAME_TOKEN_ISSUE);
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

      charge_fee(token.issuer, INFRABLOCKCHAIN_TX_FEE_OP_NAME_TOKEN_REDEEM);
   }

   void yx_token::transfer(account_name from, account_name to, const yx_asset &token, const string &memo) {
      bool is_auth_by_sysaccount = has_auth(INFRABLOCKCHAIN_SYSTEM_ACCOUNT);
      if (!is_auth_by_sysaccount) {
         check_transfer_parameters(from, to, token, memo);
      }
      check_transfer_rules(from, to, token);

      require_recipient(from);
      require_recipient(to);

      sub_token_balance(from, token);
      add_token_balance(to, token);

      if (!is_auth_by_sysaccount) {
         charge_fee(from, INFRABLOCKCHAIN_TX_FEE_OP_NAME_TOKEN_TRANSFER);
      }
   }

   void yx_token::setuilimit(const account_name &to, const yx_asset &limit) {
      eosio_assert(static_cast<uint32_t>(limit.is_valid()), "invalid limit");
      eosio_assert(static_cast<uint32_t>(limit.amount >= 0), "must be non-negative limit");
      eosio_assert(static_cast<uint32_t>(!limit.is_native(false)), "cannot grant issue with the native token");
      eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");
      eosio_assert(static_cast<uint32_t>(
            does_token_exist(INFRABLOCKCHAIN_SYS_USER_TOKEN_ACCOUNT, limit.get_yx_symbol())), "token does not exist");
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
            holder.issued = yx_asset{0, limit.get_yx_symbol()};
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

      charge_fee(limit.issuer, INFRABLOCKCHAIN_TX_FEE_OP_NAME_TOKEN_SET_USER_ISSUE_LIMIT);
   }

   void yx_token::issuebyuser(const account_name &user, const account_name &to, const yx_asset &token, const string &memo) {
      check_issue_parameters(user, token, memo);
      check_transfer_rules(user, user, token);
      eosio_assert(static_cast<uint32_t>(user != token.issuer), "user and token issuer must be different");
      eosio_assert(static_cast<uint32_t>(user != to), "self-issuance is not allowed");
      eosio_assert(static_cast<uint32_t>(is_account(to)), "to account is invalid");

      delegated_issue_table delissue_tbl{get_self(), token.symbol.value};
      auto second_index = delissue_tbl.get_index<N(secondary)>();
      const uint128_t &second_key = to_uint128(token.get_yx_symbol().issuer, user);
      const auto &issue_info = second_index.find(second_key);

      eosio_assert(static_cast<uint32_t>(issue_info != second_index.end()), "the limit of user issue is not set by the token issuer");
      eosio_assert(static_cast<uint32_t>(issue_info->limit.amount - issue_info->issued.amount >= token.amount), "issue limit is reached");

      // even if the entrusted issuer is set, the user should issue by himself or herself
      if (issue_info->entrusted_issuer == 0 || !has_auth(issue_info->entrusted_issuer)) {
         require_auth(user);
      }

      second_index.modify(issue_info, 0, [&](auto &holder) {
         holder.issued.amount += token.amount;
      });

      increase_supply(token);
      add_token_balance(user, token);
      SEND_INLINE_ACTION(*this, transfer, {{user, N(active)}, {INFRABLOCKCHAIN_SYSTEM_ACCOUNT, N(active)}}, {user, to, token, memo});

      charge_fee(user, INFRABLOCKCHAIN_TX_FEE_OP_NAME_TOKEN_ISSUE_BY_USER);
   }

   void yx_token::entrustui(const account_name &user, const account_name &to, const yx_symbol &ysymbol) {
      eosio_assert(static_cast<uint32_t>(ysymbol.is_valid()), "invalid token symbol");
      eosio_assert(static_cast<uint32_t>(!ysymbol.is_native(false)), "cannot apply the native token with this action");
      eosio_assert(static_cast<uint32_t>(to == ysymbol.issuer || user == to), "currently only the token issuer is allowed for to");

      require_auth(user);

      delegated_issue_table delissue_tbl{get_self(), ysymbol.value};
      auto second_index = delissue_tbl.get_index<N(secondary)>();
      const uint128_t &second_key = to_uint128(ysymbol.issuer, user);
      const auto &issue_info = second_index.find(second_key);

      eosio_assert(static_cast<uint32_t>(issue_info != second_index.end()), "the limit of user issue is not set by the token issuer");
      if (issue_info->entrusted_issuer != 0) {
         eosio_assert(static_cast<uint32_t>(issue_info->entrusted_issuer != to), "entrusted issuer is already set");
      } else {
         eosio_assert(static_cast<uint32_t>(user != to), "entrusted issuer is already unset");
      }

      second_index.modify(issue_info, 0, [&](auto &holder) {
         if (user == to) {
            holder.entrusted_issuer = 0;
         } else {
            holder.entrusted_issuer = to;
         }
      });

      charge_fee(user, INFRABLOCKCHAIN_TX_FEE_OP_NAME_TOKEN_ENTRUST_USER_ISSUE_TO);
   }

   void yx_token::changeissued(const account_name &user, const yx_asset &delta, bool decrease) {
      check_issue_parameters(user, delta, "");
      eosio_assert(static_cast<uint32_t>(user != delta.issuer), "user and token issuer must be different");
      require_auth(delta.issuer);

      delegated_issue_table delissue_tbl{get_self(), delta.symbol.value};
      auto second_index = delissue_tbl.get_index<N(secondary)>();
      const uint128_t &second_key = to_uint128(delta.get_yx_symbol().issuer, user);
      const auto &issue_info = second_index.find(second_key);

      eosio_assert(static_cast<uint32_t>(issue_info != second_index.end()), "the limit of user issue is not set by the token issuer");

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

      charge_fee(delta.issuer, INFRABLOCKCHAIN_TX_FEE_OP_NAME_TOKEN_CHANGE_ISSUED);
   }
}}

EOSIO_ABI(yosemite::non_native_token::yx_token,
      (create)(issue)(redeem)(transfer)(setkycrule)(setoptions)(freezeacc)(setuilimit)(issuebyuser)(entrustui)(changeissued)
)
