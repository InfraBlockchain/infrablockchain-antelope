#include "yx.token.hpp"
#include <yosemitelib/system_accounts.hpp>
#include <yx.ntoken/yx.ntoken.hpp>
#include <yx.kyc/yx.kyc.hpp>
#include <yx.system/yx.system.hpp>

namespace yosemite {

    void token::create(const yx_symbol &symbol, uint32_t kycvector) {
        eosio_assert(static_cast<uint32_t>(symbol.is_valid()), "invalid symbol name");
        eosio_assert(static_cast<uint32_t>(symbol.precision() >= 4), "token precision must be equal or larger than 4");
        eosio_assert(static_cast<uint32_t>(symbol.name() != NATIVE_TOKEN_NAME), "cannot create the native token with this operation");
        eosio_assert(static_cast<uint32_t>(kycvector <= KYC_VECTOR_MAX_VALUE), "invalid kycvector value");

        require_auth(symbol.issuer);

        stats stats_table(get_self(), symbol.value);
        const auto &tstats = stats_table.find(symbol.issuer);
        eosio_assert(static_cast<uint32_t>(tstats == stats_table.end()), "already created");

        charge_fee(symbol.issuer, N(create));

        stats_table.emplace(get_self(), [&](auto &s) {
            s.issuer = symbol.issuer;
            s.required_kycvector = kycvector;
        });
    }

    void token::issue(const account_name &to, const yx_asset &quantity, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must be positive quantity");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.name() != NATIVE_TOKEN_NAME), "cannot issue native token with this operation");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        require_auth(quantity.issuer);

        stats stats_table(get_self(), quantity.symbol.value);
        const auto &tstats = stats_table.find(quantity.issuer);
        eosio_assert(static_cast<uint32_t>(tstats != stats_table.end()), "not yet created token");

        charge_fee(quantity.issuer, N(issue));

        stats_table.modify(tstats, 0, [&](auto &s) {
            s.supply += quantity.amount;
            eosio_assert(static_cast<uint32_t>(s.supply > 0 && s.supply <= asset::max_amount), "token amount cannot be more than 2^62 - 1");
        });

        add_token_balance(quantity.issuer, quantity);

        if (to != quantity.issuer) {
            INLINE_ACTION_SENDER(yosemite::token, transfer)
                    (get_self(), {{quantity.issuer, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                     { quantity.issuer, to, quantity, quantity.issuer, memo });
        }
    }

    void token::redeem(const yx_asset &quantity, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must be positive quantity");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.name() != NATIVE_TOKEN_NAME), "cannot redeem native token with this operation");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        require_auth(quantity.issuer);

        stats stats_table(get_self(), quantity.symbol.value);
        const auto &tstats = stats_table.find(quantity.issuer);
        eosio_assert(static_cast<uint32_t>(tstats != stats_table.end()), "not yet created token");
        eosio_assert(static_cast<uint32_t>(quantity.amount <= tstats->supply), "redeem quantity exceeds supply amount");

        charge_fee(quantity.issuer, N(redeem));

        stats_table.modify(tstats, 0, [&](auto &s) {
            s.supply -= quantity.amount;
        });

        sub_token_balance(quantity.issuer, quantity);
    }

    void token::transfer(account_name from, account_name to, yx_asset quantity, account_name payer, const string &memo) {
        if (!has_auth(YOSEMITE_SYSTEM_ACCOUNT)) {
            eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
            eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must transfer positive quantity");
            eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");
            eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");
            eosio_assert(static_cast<uint32_t>(quantity.symbol.name() != NATIVE_TOKEN_NAME), "cannot transfer native token with this contract; use yx.ntoken");

            require_auth(from);
            eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");

            if (from != payer) {
                require_auth(payer);
            }
            charge_fee(payer, N(transfer));
        }

        stats stats_table(get_self(), quantity.symbol.value);
        const auto &tstats = stats_table.find(quantity.issuer);
        eosio_assert(static_cast<uint32_t>(tstats != stats_table.end()), "not yet created token");
        //TODO:add freeze functionality

        eosio_assert(static_cast<uint32_t>((kyc::get_kyc_vector(from, false) & tstats->required_kycvector) == tstats->required_kycvector),
                     "authentication for from account is not enough");
        eosio_assert(static_cast<uint32_t>((kyc::get_kyc_vector(to, false) & tstats->required_kycvector) == tstats->required_kycvector),
                     "authentication for to account is not enough");

        require_recipient(from);
        require_recipient(to);

        sub_token_balance(from, quantity);
        add_token_balance(to, quantity);
    }

    void token::add_token_balance(const account_name &owner, const yx_asset &quantity) {
        accounts accounts_table(get_self(), owner);
        auto sym_index = accounts_table.get_index<N(yxsymbol)>();
        const uint128_t &yx_symbol_s = quantity.get_yx_symbol().to_uint128();
        const auto &balance_holder = sym_index.find(yx_symbol_s);

        if (balance_holder == sym_index.end()) {
            accounts_table.emplace(get_self(), [&](auto &holder) {
                holder.id = accounts_table.available_primary_key();
                holder.yx_symbol_s = yx_symbol_s;
                holder.amount = quantity.amount;
            });
        } else {
            sym_index.modify(balance_holder, 0, [&](auto &holder) {
                holder.amount += quantity.amount;
                eosio_assert(static_cast<uint32_t>(holder.amount > 0 && holder.amount <= asset::max_amount), "token amount cannot be more than 2^62 - 1");
            });
        }
    }

    void token::sub_token_balance(const account_name &owner, const yx_asset &quantity) {
        accounts accounts_table(get_self(), owner);
        auto sym_index = accounts_table.get_index<N(yxsymbol)>();
        const uint128_t &yx_symbol_s = quantity.get_yx_symbol().to_uint128();
        const auto &balance_holder = sym_index.find(yx_symbol_s);

        eosio_assert(static_cast<uint32_t>(balance_holder != sym_index.end()), "account doesn't have token");
        eosio_assert(static_cast<uint32_t>(balance_holder->amount >= quantity.amount), "insufficient token balance");
        //TODO:add freeze functionality

        // subtract the balance from the 'owner' account
        sym_index.modify(balance_holder, 0, [&](auto &holder) {
            holder.amount -= quantity.amount;
        });
    }

    bool token::check_fee_operation(const uint64_t &operation_name) {
        return operation_name == N(create) ||
               operation_name == N(issue) ||
               operation_name == N(redeem) ||
               operation_name == N(transfer)
               ;
    }

    void token::charge_fee(const account_name &payer, uint64_t operation) {
        fees fee_table(get_self(), get_self());
        const auto &fee_holder = fee_table.get(operation, "fee is not set or unknown operation");

        if (fee_holder.fee.amount > 0) {
            INLINE_ACTION_SENDER(yosemite::ntoken, transfer)
                    (N(yx.ntoken), {{payer, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                     { payer, FEEDIST_ACCOUNT_NAME, {fee_holder.fee, 0}, payer, "" });
        }
    }
}

EOSIO_ABI(yosemite::token, (create)(issue)(redeem)(transfer)(setfee)
)
