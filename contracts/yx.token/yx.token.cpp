/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include "yx.token.hpp"

namespace yosemitex {

    uint128_t token::extended_symbol_to_uint128(const extended_symbol &symbol) const {
        return (uint128_t(symbol.value) << 64) | symbol.contract;
    }

    void token::create(const extended_symbol &symbol) {
        eosio_assert(static_cast<uint32_t>(symbol.is_valid()), "invalid symbol name");
        eosio_assert(static_cast<uint32_t>(symbol.precision() >= 4), "token precision must be equal or larger than 4");
        eosio_assert(static_cast<uint32_t>(symbol.name() != NATIVE_TOKEN_NAME), "cannot create native token without permission");

        //TODO:fee feature
        //pay_fee(symbol.contract, N(create));

        stats stats_table(get_self(), symbol.value);
        const auto &sym_index = stats_table.get_index<N(extendedsym)>();
        const uint128_t &extended_symbol_s = extended_symbol_to_uint128(symbol);
        const auto &holder = sym_index.find(extended_symbol_s);

        eosio_assert(static_cast<uint32_t>(holder == sym_index.end()), "already created by issuer");

        stats_table.emplace(get_self(), [&](auto &s) {
            s.id = stats_table.available_primary_key();
            s.extended_symbol_s = extended_symbol_s;
        });
    }

    void token::issue(const account_name &to, const extended_asset &quantity, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must be positive quantity");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.name() != NATIVE_TOKEN_NAME), "cannot issue native token with this operation");

        stats stats_table(get_self(), quantity.symbol.value);
        auto sym_index = stats_table.get_index<N(extendedsym)>();
        const extended_symbol &symbol = quantity.get_extended_symbol();
        const uint128_t &extended_symbol_s = extended_symbol_to_uint128(symbol);
        const auto &holder = sym_index.find(extended_symbol_s);

        eosio_assert(static_cast<uint32_t>(holder != sym_index.end()), "not yet created token");

        require_auth(quantity.contract);

        sym_index.modify(holder, 0, [&](auto &s) {
            s.tstats.supply += quantity.amount;
        });

        add_token_balance(quantity.contract, quantity);

        if (to != quantity.contract) {
            // fee is charged by transfer
            SEND_INLINE_ACTION(*this, transfer, {quantity.contract, N(active)}, {quantity.contract, to, quantity, memo});
        } else {
#if 0
            pay_fee(quantity.contract, N(issue));
#endif
        }
    }

    void token::redeem(const extended_asset &quantity, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must be positive quantity");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.name() != NATIVE_TOKEN_NAME), "cannot redeem native token with this operation");

        stats stats_table(get_self(), quantity.symbol.value);
        auto sym_index = stats_table.get_index<N(extendedsym)>();
        const extended_symbol &symbol = quantity.get_extended_symbol();
        const uint128_t &extended_symbol_s = extended_symbol_to_uint128(symbol);
        const auto &stats_holder = sym_index.find(extended_symbol_s);

        eosio_assert(static_cast<uint32_t>(stats_holder != sym_index.end()), "not yet created token");
        eosio_assert(static_cast<uint32_t>(quantity.amount <= stats_holder->tstats.supply), "redeem quantity exceeds supply amount");

        require_auth(quantity.contract);
        print("redeem requested by depository : ");
        quantity.print();
        print("\n");

        sym_index.modify(stats_holder, 0, [&](auto &s) {
            s.tstats.supply -= quantity.amount;
        });

        sub_token_balance(quantity.contract, quantity);
    }

    void token::transfer(account_name from, account_name to, extended_asset quantity, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must transfer positive quantity");
        eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");

        print("transfer requested by ", from, " to ", to, ", amount=");
        quantity.print();
        print("\n");

        require_auth(from);

        if (quantity.symbol.name() == NATIVE_TOKEN_NAME) {
#if 0
            auto symbol_name = quantity.symbol.name();
        stats stats_table(get_self(), symbol_name);
        const auto &stats_holder = stats_table.get(symbol_name);
        accounts _accounts(get_self(), from);
        const auto &account_holder = _accounts.get(symbol_name);

        eosio_assert(static_cast<uint32_t>(account_holder.total_balance.amount >= quantity.amount), "overdrawn balance");

        require_recipient(from);
        require_recipient(to);

        for (auto &from_balance : account_holder.balance_map) {
            if (quantity.amount == 0) {
                break;
            }
            auto itr = stats_holder.stats_map.find(from_balance.first);
            eosio_assert(static_cast<uint32_t>(itr != stats_holder.stats_map.end()), "issuer not found");
            if (has_auth(from)) {
                eosio_assert(static_cast<uint32_t>(!from_balance.second.frozen), "account is frozen by issuer");
                eosio_assert(static_cast<uint32_t>(!itr->second.is_frozen), "all transfers are frozen by issuer");
                eosio_assert(static_cast<uint32_t>(!itr->second.enforce_whitelist || from_balance.second.whitelist), "account is not whitelisted");
            } else {
                eosio_assert(false, "insufficient authority");
            }

            // subtract the balance from the 'from' account
            extended_asset to_quantity(quantity, from_balance.first);
            _accounts.modify(account_holder, 0, [&](auto &_holder) {
                auto acc_balance = _holder.balance_map[from_balance.first];
                if (acc_balance.amount <= quantity.amount) {
                    to_quantity.amount = acc_balance.amount;
                    quantity.amount -= acc_balance.amount;
                    acc_balance.amount = 0;
                } else {
                    to_quantity.amount = quantity.amount;
                    acc_balance.amount -= quantity.amount;
                    quantity.amount = 0;
                }
                if (acc_balance.amount == 0) {
                    _holder.balance_map.erase(from_balance.first);
                } else {
                    _holder.balance_map[from_balance.first] = acc_balance;
                }
                _holder.total_balance.amount -= to_quantity.amount;
            });

            add_balance(to, to_quantity);
        }
#endif
        } else {
            transfer_token(from, to, quantity);
        }
    }

    void token::transfer_token(account_name from, account_name to, const extended_asset &quantity) {
        stats stats_table(get_self(), quantity.symbol.value);
        auto sym_index = stats_table.get_index<N(extendedsym)>();
        const extended_symbol &symbol = quantity.get_extended_symbol();
        const uint128_t &extended_symbol_s = extended_symbol_to_uint128(symbol);
        const auto &stats_holder = sym_index.find(extended_symbol_s);

        eosio_assert(static_cast<uint32_t>(stats_holder != sym_index.end()), "not yet created token");
        eosio_assert(static_cast<uint32_t>(!stats_holder->tstats.frozen), "token is frozen by issuer");

        sub_token_balance(from, quantity);
        add_token_balance(to, quantity);
    }

    int64_t token::get_supply(extended_symbol symbol) const {
        stats stats_table(get_self(), symbol.value);
        const auto &sym_index = stats_table.get_index<N(extendedsym)>();
        const uint128_t &extended_symbol_s = extended_symbol_to_uint128(symbol);
        const auto &holder = sym_index.get(extended_symbol_s);

        return holder.tstats.supply;
    }

    asset token::get_total_balance(account_name owner, extended_symbol symbol) const {
#if 0
        accounts accounts_table(get_self(), owner);
        const auto &account_holder = accounts_table.get(symbol.name());
        return account_holder.total_balance;
#endif
    }

    void token::add_token_balance(const account_name &owner, const extended_asset &quantity) {
        const extended_symbol &symbol = quantity.get_extended_symbol();
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

    void token::sub_token_balance(const account_name &owner, const extended_asset &quantity) {
        const extended_symbol &symbol = quantity.get_extended_symbol();
        accounts accounts_table(get_self(), symbol.value);
        const auto &balance_holder = accounts_table.find(owner);

        eosio_assert(static_cast<uint32_t>(balance_holder != accounts_table.end()), "account not found");
        eosio_assert(static_cast<uint32_t>(!balance_holder->frozen), "account is frozen");

        const auto &itr = balance_holder->balance_map.find(symbol);
        eosio_assert(static_cast<uint32_t>(itr != balance_holder->balance_map.end()), "account don't have token");
        print("account ", owner, " balance=", itr->second.amount, ", sub quantity=", quantity.amount, "\n");
        eosio_assert(static_cast<uint32_t>(itr->second.amount >= quantity.amount), "insufficient token balance");
        eosio_assert(static_cast<uint32_t>(!itr->second.frozen), "token of the account is frozen");
        //eosio_assert(static_cast<uint32_t>(!stats_holder->tstats.enforce_whitelist || !itr->second.whitelist), "account is not whitelisted");

        // subtract the balance from the 'owner' account
        accounts_table.modify(balance_holder, 0, [&](auto &holder) {
            auto token_balance = holder.balance_map[symbol];
            token_balance.amount -= quantity.amount;
            holder.balance_map[symbol] = token_balance;
        });
    }

    void token::printsupply(extended_symbol symbol) {
        print(get_supply(symbol));
    }

    void token::printbalance(account_name owner, extended_symbol symbol) {
        accounts accounts_table(_self, symbol.value);
        const auto &balance_holder = accounts_table.get(owner);

        print("account ", owner, "\n");
        for (auto &balance : balance_holder.balance_map) {
            print("symbol=");
            balance.first.print();
            print(", balance=", balance.second.amount, "\n");
        }
    }

    void token::rmall(account_name owner, extended_symbol symbol) {
#if 0
        require_auth(get_self());
        accounts accounts_table(_self, owner);
        const auto &holder = accounts_table.get(symbol.name());

        stats stats_table(_self, symbol.name());
        const auto &stats_holder = stats_table.get(symbol.name());

        //initialize supply
        for (const auto &account_bal : holder.balance_map) {
            stats_table.modify(stats_holder, 0, [&](auto &s) {
                auto tstats = s.stats_map[account_bal.first];
                tstats.supply.amount -= account_bal.second.amount;
                s.stats_map[account_bal.first] = tstats;
            });
        }

        accounts_table.erase(holder);
#endif
    }

    void token::setfee(const name &operation_name, const extended_asset &fee) {
        //require_auth(N(yx.prods));

        eosio_assert(fee.is_valid(), "wrong fee format");
        eosio_assert(fee.amount >= 0, "negative fee");
        eosio_assert(operations.find(operation_name.value) != operations.end(), "wrong operation name");
        //TODO:NATIVE coin check, how?
        eosio_assert(fee.symbol.value == NATIVE_TOKEN, "wrong symbol for fee");

        fees fee_table(get_self(), get_self());

        const auto &holder = fee_table.find(operation_name.value);
        if (holder == fee_table.end()) {
            fee_table.emplace(get_self(), [&](auto& a) {
                a.operation = operation_name.value;
                a.fee = fee;
            });
        } else {
            fee_table.modify(holder, 0, [&](auto& a) {
                a.fee = fee;
            });
        }
    }

    void token::pay_fee(account_name payer, uint64_t operation) {
        fees fee_table(get_self(), get_self());
        const auto &fee_holder = fee_table.get(operation);

        transfer(payer, FEEDIST_ACCOUNT_NAME, fee_holder.fee, "fee");
    }

    void token::createnative(const account_name &depository) {
        //TODO:Need the agreement of producers
        //require_auth(N(yx.prods));
        //require_auth(N(eosio.prods));

        native_stats stats(get_self(), NATIVE_TOKEN);
        const auto &holder = stats.find(depository);
        eosio_assert(holder == stats.end(), "already created by the account");

        stats.emplace(get_self(), [&](auto &s) {
            s.depository = depository;
        });
    }

    void token::issuenative(const account_name &depository, const account_name &to, const extended_asset &quantity,
                            const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must be positive quantity");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.value == NATIVE_TOKEN), "cannot create native token without permission");

        native_stats stats(get_self(), NATIVE_TOKEN);
        const auto &holder = stats.find(depository);
        eosio_assert(holder != stats.end(), "native token is not created by the account");

        require_auth(depository);

        stats.modify(holder, 0, [&](auto &s) {
            s.tstats.supply += quantity.amount;
        });

        add_native_token_balance(depository, quantity.amount);
    }

    void token::add_native_token_balance(const account_name &owner, const int64_t &quantity) {
        accounts_native accounts_table(get_self(), NATIVE_TOKEN);
        const auto &native_holder = accounts_table.find(owner);

        if (native_holder == accounts_table.end()) {
            accounts_table.emplace(get_self(), [&](auto &holder) {
            });
        } else {
        }
    }
}

EOSIO_ABI(yosemitex::token, (create)(createnative)(issue)(redeem)(transfer)(setfee)(printsupply)(printbalance)(rmall))
