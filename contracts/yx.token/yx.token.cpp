/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include "yx.token.hpp"

namespace yosemitex {

    void token::create(extended_symbol symbol,
                       uint8_t issuer_can_freeze,
                       uint8_t issuer_can_recall,
                       uint8_t issuer_can_whitelist) {
        eosio_assert(static_cast<uint32_t>(symbol.is_valid()), "invalid symbol name");
        eosio_assert(static_cast<uint32_t>(symbol.precision() >= 4), "token precision must be equal or larger than 4");

        //TODO:native coin & create account check
#if 0
        if (symbol.value == NATIVE_COIN) {
            print("for native coin\n");
            //require_auth(N(yx.prods));
        } else {
            pay_fee(symbol.contract, N(create));
        }
#endif
        
        stats stats_table(get_self(), symbol.name());
        const auto &holder = stats_table.find(symbol.name());

        token_stats new_stats;
        new_stats.supply.symbol = symbol;
        new_stats.issuer = symbol.contract;
        new_stats.can_freeze = issuer_can_freeze;
        new_stats.can_recall = issuer_can_recall;
        new_stats.can_whitelist = issuer_can_whitelist;

        if (holder == stats_table.end()) {
            stats_table.emplace(get_self(), [&](auto &s) {
                s.symbol = symbol.name();
                s.stats_map[symbol.contract] = new_stats;
            });
        } else {
            auto itr = holder->stats_map.find(symbol.contract);
            eosio_assert(static_cast<uint32_t>(itr == holder->stats_map.end()), "already created by issuer");

            stats_table.modify(holder, 0, [&](auto &s) {
                s.stats_map[symbol.contract] = new_stats;
            });
        }
    }

    void token::issue(account_name to, extended_asset quantity, string /*memo*/) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must be positive quantity");

        auto symbol_name = quantity.symbol.name();
        stats stats_table(get_self(), symbol_name);
        const auto &stats_holder = stats_table.get(symbol_name);

        auto itr = stats_holder.stats_map.find(quantity.contract);
        eosio_assert(static_cast<uint32_t>(itr != stats_holder.stats_map.end()), "token is not created by issuer");
        const auto &tstats = itr->second;

        require_auth(tstats.issuer);
#if 0
        if (to != tstats.issuer) {
            pay_fee(tstats.issuer, N(issue));
        }
        //TODO:Issue by itself should not be charged
#endif

        stats_table.modify(stats_holder, 0, [&](auto &s) {
            auto stats = s.stats_map[tstats.issuer];
            stats.supply += quantity;
            s.stats_map[tstats.issuer] = stats;
        });

        if (to != tstats.issuer) {
            require_recipient(to);
        }
        add_balance(to, quantity);
    }

    void token::redeem(account_name from, extended_asset quantity, string /*memo*/) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must be positive quantity");

        auto symbol_name = quantity.symbol.name();
        stats stats_table(get_self(), symbol_name);
        const auto &stats_holder = stats_table.get(symbol_name);

        auto itr = stats_holder.stats_map.find(quantity.contract);
        eosio_assert(static_cast<uint32_t>(itr != stats_holder.stats_map.end()), "token is not created by issuer");
        const auto &tstats = itr->second;

        require_auth(tstats.issuer);

        accounts _accounts(get_self(), from);
        const auto &account_holder = _accounts.get(symbol_name);
        eosio_assert(static_cast<uint32_t>(quantity.amount <= account_holder.total_balance.amount), "quantity exceeds total balance");

        auto acc_bal_itr = account_holder.balance_map.find(quantity.contract);
        eosio_assert(static_cast<uint32_t>(acc_bal_itr != account_holder.balance_map.end()), "token of the issuer not found from the account");
        eosio_assert(static_cast<uint32_t>(quantity.amount <= acc_bal_itr->second.amount), "quantity exceeds the account balance");

#if 0
        pay_fee(quantity.contract, N(redeem));
#endif

        _accounts.modify(account_holder, 0, [&](auto &_holder) {
            if (quantity.amount == acc_bal_itr->second.amount) {
                _holder.balance_map.erase(quantity.contract);
            } else {
                auto acc_bal = _holder.balance_map[quantity.contract];
                acc_bal.amount -= quantity.amount;
                _holder.balance_map[quantity.contract] = acc_bal;
            }

            _holder.total_balance.amount -= quantity.amount;
        });

        stats_table.modify(stats_holder, 0, [&](auto &s) {
            auto stats = s.stats_map[quantity.contract];
            stats.supply.amount -= quantity.amount;
            s.stats_map[quantity.contract] = stats;
        });
    }

    void token::transfer(account_name from, account_name to, asset quantity, string /*memo*/) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must transfer positive quantity");
        eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");

        require_auth(from);

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
                eosio_assert(static_cast<uint32_t>(!itr->second.can_freeze || !from_balance.second.frozen), "account is frozen by issuer");
                eosio_assert(static_cast<uint32_t>(!itr->second.can_freeze || !itr->second.is_frozen), "all transfers are frozen by issuer");
                eosio_assert(static_cast<uint32_t>(!itr->second.enforce_whitelist || from_balance.second.whitelist), "account is not whitelisted");
            } else if (has_auth(itr->second.issuer)) {
                eosio_assert(static_cast<uint32_t>(itr->second.can_recall), "issuer may not recall token");
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
    }

    asset token::get_supply(extended_symbol symbol) const {
        stats stats_table(get_self(), symbol.name());
        const auto &st = stats_table.get(symbol.name());
        auto itr = st.stats_map.find(symbol.contract);
        eosio_assert(static_cast<uint32_t>(itr != st.stats_map.end()), "issuer not found");
        return itr->second.supply;
    }

    asset token::get_total_balance(account_name owner, extended_symbol symbol) const {
        accounts accounts_table(get_self(), owner);
        const auto &account_holder = accounts_table.get(symbol.name());
        return account_holder.total_balance;
    }

    void token::add_balance(account_name owner, extended_asset value) {
        accounts _accounts(get_self(), owner);
        const auto &holder = _accounts.find(value.symbol.name());
        if (holder == _accounts.end()) {
            _accounts.emplace(get_self(), [&](auto &real) {
                account_balance acc_bal;
                acc_bal.amount = value.amount;
                real.balance_map[value.contract] = acc_bal;

                real.total_balance = value;
            });
        } else {
            auto itr = holder->balance_map.find(value.contract);
            if (itr == holder->balance_map.end()) {
                _accounts.modify(holder, 0, [&](auto &real) {
                    account_balance acc_bal;
                    acc_bal.amount = value.amount;
                    real.balance_map[value.contract] = acc_bal;

                    real.total_balance.amount += value.amount;
                });
            } else {
                _accounts.modify(holder, 0, [&](auto &real) {
                    account_balance acc_bal = real.balance_map[value.contract];
                    acc_bal.amount += value.amount;
                    real.balance_map[value.contract] = acc_bal;

                    real.total_balance.amount += value.amount;
                });
            }
        }
    }

    void token::printsupply(extended_symbol symbol) {
        require_auth(get_self());

        get_supply(symbol).print();
    }

    void token::printbalance(account_name owner, extended_symbol symbol) {
        require_auth(get_self());

        accounts accounts_table(_self, owner);
        const auto &account_holder = accounts_table.get(symbol.name());
        account_holder.total_balance.print();
        print("\n");

        for (auto &from_balance : account_holder.balance_map) {
            print("issuer=", from_balance.first, ", balance=", from_balance.second.amount, "\n");
        }
    }

    void token::rmall(account_name owner, extended_symbol symbol) {
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
    }

    void token::setfee(name operation_name, asset fee) {
        //require_auth(N(yx.prods));

        eosio_assert(fee.is_valid(), "wrong fee format");
        eosio_assert(fee.amount >= 0, "negative fee");
        eosio_assert(operations.find(operation_name.value) != operations.end(), "wrong operation name");
        //TODO:NATIVE coin check, how?
        eosio_assert(fee.symbol.value == NATIVE_COIN, "wrong symbol for fee");

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

        transfer(payer, N(eosio.fee), fee_holder.fee, "fee");
    }
}

EOSIO_ABI(yosemitex::token, (create)(issue)(redeem)(transfer)(setfee)(printsupply)(printbalance)(rmall))
