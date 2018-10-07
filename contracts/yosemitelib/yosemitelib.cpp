#include <yosemitelib/system_accounts.hpp>
#include <yosemitelib/transaction_fee.hpp>
#include <yosemitelib/native_token.hpp>
#include <yosemitelib/token.hpp>
#include "token.hpp"

namespace yosemite { namespace non_native_token {

   void token::create(const yx_symbol &ysymbol, uint16_t can_set_options) {
      eosio_assert(static_cast<uint32_t>(ysymbol.is_valid()), "invalid ysymbol name");
      eosio_assert(static_cast<uint32_t>(!ysymbol.is_native(false)), "cannot create the native token with this action");
      eosio_assert(static_cast<uint32_t>(can_set_options <= TOKEN_CAN_SET_OPTIONS_MAX), "invalid can_set_options");
      inner_check_create_parameters(ysymbol, can_set_options);

      require_auth(ysymbol.issuer);

      stats stats_table(get_self(), ysymbol.value);
      const auto &tstats = stats_table.find(ysymbol.issuer);
      eosio_assert(static_cast<uint32_t>(tstats == stats_table.end()), "already created");

      stats_table.emplace(get_self(), [&](auto &s) {
         s.supply = yx_asset{0, ysymbol};
         s.can_set_options = can_set_options;
         s.kyc_rules.clear();
         s.kyc_rule_flags.clear();
      });
   }

   void token::add_token_balance(const account_name &owner, const yx_asset &token) {
      accounts accounts_table(get_self(), owner);
      auto sym_index = accounts_table.get_index<N(yxsymbol)>();
      const uint128_t &yx_symbol_s = token.get_yx_symbol().to_uint128();
      const auto &balance_holder = sym_index.find(yx_symbol_s);

      if (balance_holder == sym_index.end()) {
         accounts_table.emplace(get_self(), [&](auto &holder) {
            holder.id = accounts_table.available_primary_key();
            holder.token = token;
         });
      } else {
         eosio_assert(static_cast<uint32_t>((balance_holder->options & TOKEN_ACCOUNT_OPTIONS_FREEZE_ACCOUNT) !=
                                            TOKEN_ACCOUNT_OPTIONS_FREEZE_ACCOUNT),
                      "account is frozen by token issuer");

         sym_index.modify(balance_holder, 0, [&](auto &holder) {
            holder.token += token;
            eosio_assert(
                  static_cast<uint32_t>(holder.token.amount > 0 && holder.token.amount <= asset::max_amount),
                  "token amount cannot be more than 2^62 - 1");
         });
      }
   }

   void token::sub_token_balance(const account_name &owner, const yx_asset &token) {
      accounts accounts_table(get_self(), owner);
      auto sym_index = accounts_table.get_index<N(yxsymbol)>();
      const uint128_t &yx_symbol_s = token.get_yx_symbol().to_uint128();
      const auto &balance_holder = sym_index.find(yx_symbol_s);

      eosio_assert(static_cast<uint32_t>(balance_holder != sym_index.end()), "account doesn't have token");
      eosio_assert(static_cast<uint32_t>(balance_holder->token >= token), "insufficient token balance");
      eosio_assert(static_cast<uint32_t>((balance_holder->options & TOKEN_ACCOUNT_OPTIONS_FREEZE_ACCOUNT) !=
                                         TOKEN_ACCOUNT_OPTIONS_FREEZE_ACCOUNT),
                   "account is frozen by token issuer");

      bool erase;
      sym_index.modify(balance_holder, 0, [&](auto &holder) {
         holder.token -= token;
         erase = holder.token.amount == 0;
      });
      if (erase) {
         sym_index.erase(balance_holder);
      }
   }

   void token::charge_fee(const account_name &payer, uint64_t operation) {
      native_token::charge_transaction_fee(payer, operation);
   }

   void token::setkycrule(const yx_symbol &ysymbol, token_rule_t type, identity::identity_kyc_t kyc) {
      eosio_assert(static_cast<uint32_t>(type < TOKEN_KYC_RULE_TYPE_MAX), "invalid type");
      require_auth(ysymbol.issuer);

      stats stats_table(get_self(), ysymbol.value);
      const auto &tstats = stats_table.get(ysymbol.issuer, "token is not yet created");
      eosio_assert(static_cast<uint32_t>((tstats.can_set_options & TOKEN_CAN_SET_OPTIONS_SET_KYC_RULE) ==
                                         TOKEN_CAN_SET_OPTIONS_SET_KYC_RULE),
                   "cannot set KYC rule");

      auto itr = std::find(tstats.kyc_rules.begin(), tstats.kyc_rules.end(), type);
      if (itr == tstats.kyc_rules.end()) {
         stats_table.modify(tstats, 0, [&](auto &s) {
            s.kyc_rules.push_back(type);
            s.kyc_rule_flags.push_back(kyc);
         });
      } else {
         auto index = std::distance(tstats.kyc_rules.begin(), itr);
         stats_table.modify(tstats, 0, [&](auto &s) {
            s.kyc_rules[static_cast<size_t>(index)] = type;
            s.kyc_rule_flags[static_cast<size_t>(index)] = kyc;
         });
      }

      charge_fee(ysymbol.issuer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_SETKYCRULE);
   }

   bool token::check_identity_auth_for_transfer(account_name account, const token_rule_type &kycrule_type,
                                                const token_stats &tstats) {
      eosio_assert(static_cast<uint32_t>(!identity::has_account_state(account, YOSEMITE_ID_ACC_STATE_BLACKLISTED)),
                   "account is blacklisted by identity authority");

      auto itr = std::find(tstats.kyc_rules.begin(), tstats.kyc_rules.end(), kycrule_type);
      if (itr == tstats.kyc_rules.end()) return true;

      auto index = std::distance(tstats.kyc_rules.begin(), itr);
      auto kyc_flags = tstats.kyc_rule_flags[static_cast<size_t>(index)];
      return identity::has_all_kyc_status(account, kyc_flags);
   }

   void token::setoptions(const yx_symbol &ysymbol, uint16_t options, bool reset) {
      eosio_assert(static_cast<uint32_t>(options <= TOKEN_OPTIONS_MAX), "invalid options");
      require_auth(ysymbol.issuer);

      stats stats_table(get_self(), ysymbol.value);
      const auto &tstats = stats_table.get(ysymbol.issuer, "token is not yet created");
      if ((options & TOKEN_OPTIONS_FREEZE_TOKEN_TRANSFER) == TOKEN_OPTIONS_FREEZE_TOKEN_TRANSFER) {
         eosio_assert(
               static_cast<uint32_t>((tstats.can_set_options & TOKEN_CAN_SET_OPTIONS_FREEZE_TOKEN_TRANSFER) ==
                                     TOKEN_CAN_SET_OPTIONS_FREEZE_TOKEN_TRANSFER),
               "cannot set TOKEN_OPTIONS_FREEZE_TOKEN_TRANSFER");
      }

      stats_table.modify(tstats, 0, [&](auto &s) {
         if (reset) {
            s.options = options;
         } else {
            s.options |= options;
         }
      });

      charge_fee(ysymbol.issuer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_SETOPTIONS);
   }

   void token::freezeacc(const yx_symbol &ysymbol, const vector<account_name> &accs, bool freeze) {
      eosio_assert(static_cast<uint32_t>(accs.size() <= 32 && !accs.empty()), "too many or empty accs");
      require_auth(ysymbol.issuer);

      stats stats_table(get_self(), ysymbol.value);
      const auto &tstats = stats_table.get(ysymbol.issuer, "token is not yet created");
      eosio_assert(static_cast<uint32_t>((tstats.can_set_options & TOKEN_CAN_SET_OPTIONS_FREEZE_ACCOUNT) ==
                                         TOKEN_CAN_SET_OPTIONS_FREEZE_ACCOUNT),
                   "cannot freeze or unfreeze accounts");

      const uint128_t &yx_symbol_s = ysymbol.to_uint128();
      for (auto owner : accs) {
         accounts accounts_table{get_self(), owner};
         auto sym_index = accounts_table.get_index<N(yxsymbol)>();
         const auto &balance_holder = sym_index.find(yx_symbol_s);

         eosio_assert(static_cast<uint32_t>(balance_holder != sym_index.end()), "account doesn't have token");
         sym_index.modify(balance_holder, 0, [&](auto &holder) {
            if (freeze) {
               holder.options |= TOKEN_ACCOUNT_OPTIONS_FREEZE_ACCOUNT;
            } else {
               holder.options &= ~TOKEN_ACCOUNT_OPTIONS_FREEZE_ACCOUNT;
            }
         });
      }

      charge_fee(ysymbol.issuer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_FREEZEACC);
   }

   void token::check_issue_parameters(const account_name &to, const yx_asset &token, const string &memo) {
      eosio_assert(static_cast<uint32_t>(token.is_valid()), "invalid token");
      eosio_assert(static_cast<uint32_t>(token.amount > 0), "must be positive token");
      eosio_assert(static_cast<uint32_t>(!token.is_native(false)), "cannot issue the native token with this action");
      eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");
      require_auth(token.issuer);
   }

   void token::check_transfer_parameters(account_name from, account_name to, const yx_asset &token, const string &memo) {
      eosio_assert(static_cast<uint32_t>(token.is_valid()), "invalid token");
      eosio_assert(static_cast<uint32_t>(token.amount > 0), "must transfer positive token");
      eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");
      eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");
      eosio_assert(static_cast<uint32_t>(!token.is_native(false)),
                   "cannot transfer native token with this contract; use yx.ntoken");
      require_auth(from);
      eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");
   }

   void token::check_transfer_rules(account_name from, account_name to, const yx_asset &token) {
      stats stats_table(get_self(), token.symbol.value);
      const auto &tstats = stats_table.get(token.issuer, "token is not yet created");

      if (tstats.options > 0) {
         eosio_assert(static_cast<uint32_t>((tstats.options & TOKEN_OPTIONS_FREEZE_TOKEN_TRANSFER) !=
                                            TOKEN_OPTIONS_FREEZE_TOKEN_TRANSFER),
                      "token transfer is frozen");
      }
      if (!tstats.kyc_rules.empty()) {
         eosio_assert(
               static_cast<uint32_t>(check_identity_auth_for_transfer(from, TOKEN_KYC_RULE_TYPE_TRANSFER_SEND,
                                                                      tstats)),
               "KYC authentication for from account is failed");
         eosio_assert(
               static_cast<uint32_t>(check_identity_auth_for_transfer(to, TOKEN_KYC_RULE_TYPE_TRANSFER_RECEIVE,
                                                                      tstats)),
               "KYC authentication for to account is failed");
      }
   }

}}
