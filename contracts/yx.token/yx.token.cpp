#include "yx.token.hpp"
#include <yx.ntoken/yx.ntoken.hpp>
#include <yx.kyc/yx.kyc.hpp>

//TODO:1. freeze by account/token

namespace yosemite {

    void token::regdepo(const yx_symbol &symbol, uint32_t authvector) {
        eosio_assert(static_cast<uint32_t>(symbol.is_valid()), "invalid symbol name");
        eosio_assert(static_cast<uint32_t>(symbol.precision() >= 4), "token precision must be equal or larger than 4");
        eosio_assert(static_cast<uint32_t>(symbol.name() != NATIVE_TOKEN_NAME), "cannot register depository for the native token with this operation");
        eosio_assert(static_cast<uint32_t>(is_account(symbol.issuer)), "issuer account does not exist");
        eosio_assert(static_cast<uint32_t>(authvector <= KYC_AUTHVECTOR_MAX_VALUE), "invalid authvector value");

        stats stats_table(get_self(), symbol.value);
        const auto &sym_index = stats_table.get_index<N(extendedsym)>();
        const uint128_t &yx_symbol_s = symbol.to_uint128();
        const auto &holder = sym_index.find(yx_symbol_s);
        eosio_assert(static_cast<uint32_t>(holder == sym_index.end()), "already registered");

        charge_fee(symbol.issuer, N(regdepo));

        stats_table.emplace(get_self(), [&](auto &s) {
            s.id = stats_table.available_primary_key();
            s.yx_symbol_s = yx_symbol_s;
            s.required_authvector = authvector;
        });
    }

    void token::issue(const account_name &to, const yx_asset &quantity, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must be positive quantity");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.name() != NATIVE_TOKEN_NAME), "cannot issue native token with this operation");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        stats stats_table(get_self(), quantity.symbol.value);
        auto sym_index = stats_table.get_index<N(extendedsym)>();
        const yx_symbol &symbol = quantity.get_yx_symbol();
        const uint128_t &yx_symbol_s = symbol.to_uint128();
        const auto &tstats = sym_index.find(yx_symbol_s);
        eosio_assert(static_cast<uint32_t>(tstats != sym_index.end()), "not yet registered token");

        require_auth(quantity.issuer);

        charge_fee(quantity.issuer, N(issue));

        sym_index.modify(tstats, 0, [&](auto &s) {
            s.supply += quantity.amount;
        });

        add_token_balance(quantity.issuer, quantity);

        if (to != quantity.issuer) {
            transfer_internal(quantity.issuer, to, quantity, false, 0);
        }
    }

    void token::redeem(const yx_asset &quantity, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must be positive quantity");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.name() != NATIVE_TOKEN_NAME), "cannot redeem native token with this operation");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        stats stats_table(get_self(), quantity.symbol.value);
        auto sym_index = stats_table.get_index<N(extendedsym)>();
        const yx_symbol &symbol = quantity.get_yx_symbol();
        const uint128_t &yx_symbol_s = symbol.to_uint128();
        const auto &tstats = sym_index.find(yx_symbol_s);

        eosio_assert(static_cast<uint32_t>(tstats != sym_index.end()), "not yet registered token");
        eosio_assert(static_cast<uint32_t>(quantity.amount <= tstats->supply), "redeem quantity exceeds supply amount");

        require_auth(quantity.issuer);

        charge_fee(quantity.issuer, N(redeem));

        sym_index.modify(tstats, 0, [&](auto &s) {
            s.supply -= quantity.amount;
        });

        sub_token_balance(quantity.issuer, quantity);
    }

    void token::transfer(account_name from, account_name to, yx_asset quantity, account_name payer, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must transfer positive quantity");
        eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.name() != NATIVE_TOKEN_NAME), "cannot transfer native token with this contract; use yx.ntoken");

        transfer_internal(from, to, quantity, from != FEEDIST_ACCOUNT_NAME, payer);
    }

    void token::transfer_internal(account_name from, account_name to, yx_asset quantity, bool fee_required,
                                  account_name payer) {
        require_auth(from);
        eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");

        if (fee_required) {
            if (from != payer) {
                require_auth(payer);
            }
            charge_fee(payer, N(transfer));
        }

        stats stats_table(get_self(), quantity.symbol.value);
        auto sym_index = stats_table.get_index<N(extendedsym)>();
        const yx_symbol &symbol = quantity.get_yx_symbol();
        const uint128_t &yx_symbol_s = symbol.to_uint128();
        const auto &tstats = sym_index.find(yx_symbol_s);

        eosio_assert(static_cast<uint32_t>(tstats != sym_index.end()), "not yet registered token");
        eosio_assert(static_cast<uint32_t>(!tstats->frozen), "token is frozen by issuer");

        kyc kyc_contract(N(yx.kyc));
        eosio_assert(static_cast<uint32_t>((kyc_contract.get_kyc_authvector(from, false) & tstats->required_authvector) == tstats->required_authvector),
                     "authentication for from account is not enough");
        eosio_assert(static_cast<uint32_t>((kyc_contract.get_kyc_authvector(to, false) & tstats->required_authvector) == tstats->required_authvector),
                     "authentication for to account is not enough");

        require_recipient(from);
        require_recipient(to);

        sub_token_balance(from, quantity);
        add_token_balance(to, quantity);
    }

    int64_t token::get_supply(yx_symbol symbol) const {
        stats stats_table(get_self(), symbol.value);
        const auto &sym_index = stats_table.get_index<N(extendedsym)>();
        const uint128_t &yx_symbol_s = symbol.to_uint128();
        const auto &tstats = sym_index.get(yx_symbol_s, "token symbol not found");

        return tstats.supply;
    }

    void token::add_token_balance(const account_name &owner, const yx_asset &quantity) {
        const yx_symbol &symbol = quantity.get_yx_symbol();
        accounts accounts_table(get_self(), symbol.value);
        const auto &holder = accounts_table.find(owner);

        if (holder == accounts_table.end()) {
            accounts_table.emplace(get_self(), [&](auto &real) {
                token_balance balance;
                balance.amount = quantity.amount;
                real.balance_map[symbol] = balance;

                real.owner = owner;
            });
        } else {
            auto itr = holder->balance_map.find(symbol);
            if (itr == holder->balance_map.end()) {
                accounts_table.modify(holder, 0, [&](auto &real) {
                    token_balance balance;
                    balance.amount = quantity.amount;
                    real.balance_map[symbol] = balance;
                });
            } else {
                accounts_table.modify(holder, 0, [&](auto &real) {
                    token_balance balance = real.balance_map[symbol];
                    balance.amount += quantity.amount;
                    real.balance_map[symbol] = balance;
                });
            }
        }
    }

    void token::sub_token_balance(const account_name &owner, const yx_asset &quantity) {
        const yx_symbol &symbol = quantity.get_yx_symbol();
        accounts accounts_table(get_self(), symbol.value);
        const auto &balance_holder = accounts_table.find(owner);

        eosio_assert(static_cast<uint32_t>(balance_holder != accounts_table.end()), "owner account not found");
        eosio_assert(static_cast<uint32_t>(!balance_holder->frozen), "owner account is frozen");

        const auto &itr = balance_holder->balance_map.find(symbol);
        eosio_assert(static_cast<uint32_t>(itr != balance_holder->balance_map.end()), "account don't have token");
        eosio_assert(static_cast<uint32_t>(itr->second.amount >= quantity.amount), "insufficient token balance");
        eosio_assert(static_cast<uint32_t>(!itr->second.frozen), "token of the account is frozen");

        // subtract the balance from the 'owner' account
        accounts_table.modify(balance_holder, 0, [&](auto &holder) {
            auto token_balance = holder.balance_map[symbol];
            token_balance.amount -= quantity.amount;
            holder.balance_map[symbol] = token_balance;
        });
    }

    void token::printsupply(const yx_symbol &symbol) {
        print(get_supply(symbol));
    }

    void token::printbalance(account_name owner, yx_symbol symbol) {
        accounts accounts_table(get_self(), symbol.value);
        const auto &balance_holder = accounts_table.get(owner, "account not found");

        for (auto &balance : balance_holder.balance_map) {
            print("symbol=");
            balance.first.print();
            print(", balance=", balance.second.amount, "\n");
        }
    }

    bool token::check_fee_operation(const uint64_t &operation_name) {
        return operation_name == N(regdepo) ||
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
                    (N(yx.ntoken), {payer, N(active)},
                     { payer, FEEDIST_ACCOUNT_NAME, yx_asset{fee_holder.fee, 0}, payer, "" });
        }
    }

    void token::clear(const yx_symbol &symbol) {
        require_auth(get_self());

        accounts accounts_table(get_self(), symbol.value);
        for (auto iterator = accounts_table.begin(); iterator != accounts_table.end(); ) {
            print("owner=", iterator->owner, "\n");
            iterator = accounts_table.erase(iterator);
        }

        stats stats_table(get_self(), symbol.value);
        for (auto iterator = stats_table.begin(); iterator != stats_table.end(); ) {
            print("supply=", iterator->supply, "\n");
            iterator = stats_table.erase(iterator);
        }
    }
}

EOSIO_ABI(yosemite::token, (regdepo)(issue)(redeem)(transfer)(setfee)
                           (printsupply)(printbalance)(clear))
