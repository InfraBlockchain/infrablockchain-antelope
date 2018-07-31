#include "yx.token.hpp"
#include <yosemitelib/identity.hpp>
#include <yosemitelib/system_accounts.hpp>
#include <yosemitelib/transaction_fee.hpp>
#include <yx.ntoken/yx.ntoken.hpp>
#include <yx.system/yx.system.hpp>

namespace yosemite {

    void token::create(const yx_symbol &symbol) {
        eosio_assert(static_cast<uint32_t>(symbol.is_valid()), "invalid symbol name");
        eosio_assert(static_cast<uint32_t>(symbol.precision() >= 4), "token precision must be equal or larger than 4");
        eosio_assert(static_cast<uint32_t>(symbol.name() != YOSEMITE_NATIVE_TOKEN_SYMBOL_NAME),
                     "cannot create the native token with this operation");

        require_auth(symbol.issuer);

        stats stats_table(get_self(), symbol.value);
        const auto &tstats = stats_table.find(symbol.issuer);
        eosio_assert(static_cast<uint32_t>(tstats == stats_table.end()), "already created");

        charge_fee(symbol.issuer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_CREATE);

        stats_table.emplace(get_self(), [&](auto &s) {
            s.issuer = symbol.issuer;
        });
    }

    void token::issue(const account_name &to, const yx_asset &asset, const string &memo) {
        eosio_assert(static_cast<uint32_t>(asset.is_valid()), "invalid asset");
        eosio_assert(static_cast<uint32_t>(asset.amount > 0), "must be positive asset");
        eosio_assert(static_cast<uint32_t>(asset.symbol.name() != YOSEMITE_NATIVE_TOKEN_SYMBOL_NAME),
                     "cannot issue native token with this operation");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        require_auth(asset.issuer);

        stats stats_table(get_self(), asset.symbol.value);
        const auto &tstats = stats_table.get(asset.issuer, "not yet created token");

        charge_fee(asset.issuer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_ISSUE);

        stats_table.modify(tstats, 0, [&](auto &s) {
            s.supply += asset.amount;
            eosio_assert(static_cast<uint32_t>(s.supply > 0 && s.supply <= asset::max_amount),
                         "token amount cannot be more than 2^62 - 1");
        });

        add_token_balance(asset.issuer, asset);

        if (to != asset.issuer) {
            INLINE_ACTION_SENDER(yosemite::token, transfer)
                    (get_self(), {{asset.issuer, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                     { asset.issuer, to, asset, memo });
        }
    }

    void token::redeem(const yx_asset &asset, const string &memo) {
        eosio_assert(static_cast<uint32_t>(asset.is_valid()), "invalid asset");
        eosio_assert(static_cast<uint32_t>(asset.amount > 0), "must be positive asset");
        eosio_assert(static_cast<uint32_t>(asset.symbol.name() != YOSEMITE_NATIVE_TOKEN_SYMBOL_NAME),
                     "cannot redeem native token with this operation");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        require_auth(asset.issuer);

        stats stats_table(get_self(), asset.symbol.value);
        const auto &tstats = stats_table.get(asset.issuer, "not yet created token");
        eosio_assert(static_cast<uint32_t>(asset.amount <= tstats.supply), "redeem asset exceeds supply amount");

        charge_fee(asset.issuer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_REDEEM);

        stats_table.modify(tstats, 0, [&](auto &s) {
            s.supply -= asset.amount;
        });

        sub_token_balance(asset.issuer, asset);
    }

    void token::transfer(account_name from, account_name to, yx_asset asset, const string &memo) {
        wptransfer(from, to, asset, from, memo);
    }

    void token::wptransfer(account_name from, account_name to, yx_asset asset, account_name payer, const string &memo) {
        if (!has_auth(YOSEMITE_SYSTEM_ACCOUNT)) {
            eosio_assert(static_cast<uint32_t>(asset.is_valid()), "invalid asset");
            eosio_assert(static_cast<uint32_t>(asset.amount > 0), "must transfer positive asset");
            eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");
            eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");
            eosio_assert(static_cast<uint32_t>(asset.symbol.name() != YOSEMITE_NATIVE_TOKEN_SYMBOL_NAME),
                         "cannot transfer native token with this contract; use yx.ntoken");

            require_auth(from);
            eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");

            if (from != payer) {
                require_auth(payer);
            }
            charge_fee(payer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_TRANSFER);
        }

        stats stats_table(get_self(), asset.symbol.value);
        const auto &tstats = stats_table.get(asset.issuer, "not yet created token");
        //TODO:add freeze functionality

        eosio_assert(static_cast<uint32_t>(check_identity_auth_for_transfer(from, TOKEN_KYC_RULE_TYPE_TRANSFER_SEND, tstats)),
                     "KYC authentication for from account is failed");
        eosio_assert(static_cast<uint32_t>(check_identity_auth_for_transfer(to, TOKEN_KYC_RULE_TYPE_TRANSFER_RECEIVE, tstats)),
                     "KYC authentication for to account is failed");

        require_recipient(from);
        require_recipient(to);

        sub_token_balance(from, asset);
        add_token_balance(to, asset);
    }

    void token::add_token_balance(const account_name &owner, const yx_asset &asset) {
        accounts accounts_table(get_self(), owner);
        auto sym_index = accounts_table.get_index<N(yxsymbol)>();
        const uint128_t &yx_symbol_s = asset.get_yx_symbol().to_uint128();
        const auto &balance_holder = sym_index.find(yx_symbol_s);

        if (balance_holder == sym_index.end()) {
            accounts_table.emplace(get_self(), [&](auto &holder) {
                holder.id = accounts_table.available_primary_key();
                holder.yx_symbol_s = yx_symbol_s;
                holder.amount = asset.amount;
            });
        } else {
            sym_index.modify(balance_holder, 0, [&](auto &holder) {
                holder.amount += asset.amount;
                eosio_assert(static_cast<uint32_t>(holder.amount > 0 && holder.amount <= asset::max_amount),
                             "token amount cannot be more than 2^62 - 1");
            });
        }
    }

    void token::sub_token_balance(const account_name &owner, const yx_asset &asset) {
        accounts accounts_table(get_self(), owner);
        auto sym_index = accounts_table.get_index<N(yxsymbol)>();
        const uint128_t &yx_symbol_s = asset.get_yx_symbol().to_uint128();
        const auto &balance_holder = sym_index.find(yx_symbol_s);

        eosio_assert(static_cast<uint32_t>(balance_holder != sym_index.end()), "account doesn't have token");
        eosio_assert(static_cast<uint32_t>(balance_holder->amount >= asset.amount), "insufficient token balance");
        //TODO:add freeze functionality

        // subtract the balance from the 'owner' account
        sym_index.modify(balance_holder, 0, [&](auto &holder) {
            holder.amount -= asset.amount;
        });
    }

    void token::charge_fee(const account_name &payer, uint64_t operation) {
        auto tx_fee = yosemite::get_transaction_fee(operation);

        if (tx_fee.amount > 0) {
            INLINE_ACTION_SENDER(yosemite::ntoken, payfee)
                    (YOSEMITE_NATIVE_TOKEN_ACCOUNT, {{payer, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                     {payer, tx_fee});
        }
    }

    void token::setkycrule(const yx_symbol &symbol, uint8_t type, uint16_t kyc) {
        eosio_assert(static_cast<uint32_t>(type < TOKEN_KYC_RULE_TYPE_MAX), "invalid type");
//        eosio_assert(static_cast<uint32_t>(is_valid_kyc_status(kyc)), "invalid kyc flags");
        require_auth(symbol.issuer);

        stats stats_table(get_self(), symbol.value);
        const auto &tstats = stats_table.get(symbol.issuer, "not yet created token");

        charge_fee(symbol.issuer, YOSEMITE_TX_FEE_OP_NAME_TOKEN_SETKYCRULE);

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
}

EOSIO_ABI(yosemite::token, (create)(issue)(redeem)(transfer)(wptransfer)(setkycrule)
)
