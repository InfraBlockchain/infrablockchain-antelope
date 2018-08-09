#include "yx.token.hpp"
#include <yosemitelib/identity.hpp>
#include <yosemitelib/system_accounts.hpp>
#include <yosemitelib/transaction_fee.hpp>
#include <yosemitelib/native_token.hpp>

namespace yosemite {

    void token::create(const yx_symbol &ysymbol, uint16_t can_set_options) {
        eosio_assert(static_cast<uint32_t>(ysymbol.is_valid()), "invalid ysymbol name");
        eosio_assert(static_cast<uint32_t>(ysymbol.precision() >= 4), "token precision must be equal or larger than 4");
        eosio_assert(static_cast<uint32_t>(ysymbol.name() != YOSEMITE_NATIVE_TOKEN_SYMBOL_NAME),
                     "cannot create the native token with this operation");
        eosio_assert(static_cast<uint32_t>(can_set_options <= TOKEN_CAN_SET_OPTIONS_MAX), "invalid can_set_options");

        require_auth(ysymbol.issuer);

        stats stats_table(get_self(), ysymbol.value);
        const auto &tstats = stats_table.find(ysymbol.issuer);
        eosio_assert(static_cast<uint32_t>(tstats == stats_table.end()), "already created");

        stats_table.emplace(get_self(), [&](auto &s) {
            s.issuer = ysymbol.issuer;
            s.can_set_options = can_set_options;
        });

        charge_fee(ysymbol.issuer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_CREATE);
    }

    void token::issue(const account_name &to, const yx_asset &token, const string &memo) {
        eosio_assert(static_cast<uint32_t>(token.is_valid()), "invalid token");
        eosio_assert(static_cast<uint32_t>(token.amount > 0), "must be positive token");
        eosio_assert(static_cast<uint32_t>(token.symbol.name() != YOSEMITE_NATIVE_TOKEN_SYMBOL_NAME),
                     "cannot issue native token with this operation");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        require_auth(token.issuer);

        stats stats_table(get_self(), token.symbol.value);
        const auto &tstats = stats_table.get(token.issuer, "token is not yet created");

        stats_table.modify(tstats, 0, [&](auto &s) {
            s.supply += token.amount;
            eosio_assert(static_cast<uint32_t>(s.supply > 0 && s.supply <= asset::max_amount),
                         "token amount cannot be more than 2^62 - 1");
        });

        add_token_balance(token.issuer, token);

        if (to != token.issuer) {
            INLINE_ACTION_SENDER(yosemite::token, transfer)
                    (get_self(), {{token.issuer, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                     { token.issuer, to, token, memo });
        }

        charge_fee(token.issuer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_ISSUE);
    }

    void token::redeem(const yx_asset &token, const string &memo) {
        eosio_assert(static_cast<uint32_t>(token.is_valid()), "invalid token");
        eosio_assert(static_cast<uint32_t>(token.amount > 0), "must be positive token");
        eosio_assert(static_cast<uint32_t>(token.symbol.name() != YOSEMITE_NATIVE_TOKEN_SYMBOL_NAME),
                     "cannot redeem native token with this operation");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        require_auth(token.issuer);

        stats stats_table(get_self(), token.symbol.value);
        const auto &tstats = stats_table.get(token.issuer, "token is not yet created");
        eosio_assert(static_cast<uint32_t>(token.amount <= tstats.supply), "redeem token exceeds supply amount");

        stats_table.modify(tstats, 0, [&](auto &s) {
            s.supply -= token.amount;
        });

        sub_token_balance(token.issuer, token);

        charge_fee(token.issuer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_REDEEM);
    }

    void token::transfer(account_name from, account_name to, yx_asset asset, const string &memo) {
        wptransfer(from, to, asset, from, memo);
    }

    void token::wptransfer(account_name from, account_name to, yx_asset token, account_name payer, const string &memo) {
        bool is_auth_by_sysaccount = has_auth(YOSEMITE_SYSTEM_ACCOUNT);
        if (!is_auth_by_sysaccount) {
            eosio_assert(static_cast<uint32_t>(token.is_valid()), "invalid token");
            eosio_assert(static_cast<uint32_t>(token.amount > 0), "must transfer positive token");
            eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");
            eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");
            eosio_assert(static_cast<uint32_t>(token.symbol.name() != YOSEMITE_NATIVE_TOKEN_SYMBOL_NAME),
                         "cannot transfer native token with this contract; use yx.ntoken");

            require_auth(from);
            eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");
        }

        stats stats_table(get_self(), token.symbol.value);
        const auto &tstats = stats_table.get(token.issuer, "token is not yet created");

        if (tstats.options > 0) {
            eosio_assert(static_cast<uint32_t>((tstats.options & TOKEN_OPTIONS_FREEZE_TOKEN_TRANSFER) != TOKEN_OPTIONS_FREEZE_TOKEN_TRANSFER),
                         "token transfer is frozen");
        }
        if (!tstats.kyc_rule_types.empty()) {
            eosio_assert(static_cast<uint32_t>(check_identity_auth_for_transfer(from, TOKEN_KYC_RULE_TYPE_TRANSFER_SEND, tstats)),
                         "KYC authentication for from account is failed");
            eosio_assert(static_cast<uint32_t>(check_identity_auth_for_transfer(to, TOKEN_KYC_RULE_TYPE_TRANSFER_RECEIVE, tstats)),
                         "KYC authentication for to account is failed");
        }

        require_recipient(from);
        require_recipient(to);

        sub_token_balance(from, token);
        add_token_balance(to, token);

        if (!is_auth_by_sysaccount) {
            charge_fee(payer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_TRANSFER);
        }
    }

    void token::add_token_balance(const account_name &owner, const yx_asset &token) {
        accounts accounts_table(get_self(), owner);
        auto sym_index = accounts_table.get_index<N(yxsymbol)>();
        const uint128_t &yx_symbol_s = token.get_yx_symbol().to_uint128();
        const auto &balance_holder = sym_index.find(yx_symbol_s);

        if (balance_holder == sym_index.end()) {
            accounts_table.emplace(get_self(), [&](auto &holder) {
                holder.id = accounts_table.available_primary_key();
                holder.yx_symbol_s = yx_symbol_s;
                holder.amount = token.amount;
            });
        } else {
            eosio_assert(static_cast<uint32_t>((balance_holder->options & TOKEN_ACCOUNT_OPTIONS_FREEZE_ACCOUNT) != TOKEN_ACCOUNT_OPTIONS_FREEZE_ACCOUNT),
                    "account is fronzen by token issuer");

            sym_index.modify(balance_holder, 0, [&](auto &holder) {
                holder.amount += token.amount;
                eosio_assert(static_cast<uint32_t>(holder.amount > 0 && holder.amount <= asset::max_amount),
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
        eosio_assert(static_cast<uint32_t>(balance_holder->amount >= token.amount), "insufficient token balance");
        eosio_assert(static_cast<uint32_t>((balance_holder->options & TOKEN_ACCOUNT_OPTIONS_FREEZE_ACCOUNT) != TOKEN_ACCOUNT_OPTIONS_FREEZE_ACCOUNT),
                "account is fronzen by token issuer");

        bool erase;
        sym_index.modify(balance_holder, 0, [&](auto &holder) {
            holder.amount -= token.amount;
            erase = holder.amount == 0;
        });
        if (erase) {
            sym_index.erase(balance_holder);
        }
    }

    void token::charge_fee(const account_name &payer, uint64_t operation) {
        charge_transaction_fee(payer, operation);
    }

    void token::setkycrule(const yx_symbol &ysymbol, uint8_t type, uint16_t kyc) {
        eosio_assert(static_cast<uint32_t>(type < TOKEN_KYC_RULE_TYPE_MAX), "invalid type");
//        eosio_assert(static_cast<uint32_t>(is_valid_kyc_status(kyc)), "invalid kyc flags");
        require_auth(ysymbol.issuer);

        stats stats_table(get_self(), ysymbol.value);
        const auto &tstats = stats_table.get(ysymbol.issuer, "token is not yet created");
        eosio_assert(static_cast<uint32_t>((tstats.can_set_options & TOKEN_CAN_SET_OPTIONS_SET_KYC_RULE) == TOKEN_CAN_SET_OPTIONS_SET_KYC_RULE),
                "cannot set KYC rule");

        auto itr = std::find(tstats.kyc_rule_types.begin(), tstats.kyc_rule_types.end(), type);
        if (itr == tstats.kyc_rule_types.end()) {
            stats_table.modify(tstats, 0, [&](auto &s) {
                s.kyc_rule_types.push_back(type);
                s.kyc_rule_flags.push_back(kyc);
            });
        } else {
            auto index = std::distance(tstats.kyc_rule_types.begin(), itr);
            stats_table.modify(tstats, 0, [&](auto &s) {
                s.kyc_rule_types[static_cast<size_t>(index)] = type;
                s.kyc_rule_flags[static_cast<size_t>(index)] = kyc;
            });
        }

        charge_fee(ysymbol.issuer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_SETKYCRULE);
    }

    bool token::check_identity_auth_for_transfer(account_name account, const token_kyc_rule_type &kycrule_type,
                                                 const token_stats &tstats) {
        eosio_assert(static_cast<uint32_t>(!has_account_state(account, YOSEMITE_ID_ACC_STATE_BLACKLISTED)),
                     "account is blacklisted by identity authority");

        auto itr = std::find(tstats.kyc_rule_types.begin(), tstats.kyc_rule_types.end(), kycrule_type);
        if (itr == tstats.kyc_rule_types.end()) return true;

        auto index = std::distance(tstats.kyc_rule_types.begin(), itr);
        auto kyc_flags = tstats.kyc_rule_flags[static_cast<size_t>(index)];
        return has_all_kyc_status(account, kyc_flags);
    }

    void token::setoptions(const yx_symbol &ysymbol, uint16_t options, bool overwrite) {
        eosio_assert(static_cast<uint32_t>(options <= TOKEN_OPTIONS_MAX), "invalid options");
        require_auth(ysymbol.issuer);

        stats stats_table(get_self(), ysymbol.value);
        const auto &tstats = stats_table.get(ysymbol.issuer, "token is not yet created");
        if ((options & TOKEN_OPTIONS_FREEZE_TOKEN_TRANSFER) == TOKEN_OPTIONS_FREEZE_TOKEN_TRANSFER) {
            eosio_assert(static_cast<uint32_t>((tstats.can_set_options & TOKEN_CAN_SET_OPTIONS_FREEZE_TOKEN_TRANSFER) == TOKEN_CAN_SET_OPTIONS_FREEZE_TOKEN_TRANSFER),
                    "cannot set TOKEN_OPTIONS_FREEZE_TOKEN_TRANSFER");
        }

        stats_table.modify(tstats, 0, [&](auto &s) {
            if (overwrite) {
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
        eosio_assert(static_cast<uint32_t>((tstats.can_set_options & TOKEN_CAN_SET_OPTIONS_FREEZE_ACCOUNT) == TOKEN_CAN_SET_OPTIONS_FREEZE_ACCOUNT),
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

}

EOSIO_ABI(yosemite::token, (create)(issue)(redeem)(transfer)(wptransfer)(setkycrule)(setoptions)(freezeacc)
)
