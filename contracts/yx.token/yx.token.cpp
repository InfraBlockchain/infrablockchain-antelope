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

        stats stats_table(get_self(), symbol.value);
        const auto &sym_index = stats_table.get_index<N(extendedsym)>();
        const uint128_t &extended_symbol_s = extended_symbol_to_uint128(symbol);
        const auto &holder = sym_index.find(extended_symbol_s);
        eosio_assert(static_cast<uint32_t>(holder == sym_index.end()), "already created by issuer");

        charge_fee(symbol.contract, N(create));

        stats_table.emplace(get_self(), [&](auto &s) {
            s.id = stats_table.available_primary_key();
            s.extended_symbol_s = extended_symbol_s;
        });
    }

    void token::issue(const account_name &to, const extended_asset &quantity, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must be positive quantity");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.name() != NATIVE_TOKEN_NAME), "cannot issue native token with this operation");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        stats stats_table(get_self(), quantity.symbol.value);
        auto sym_index = stats_table.get_index<N(extendedsym)>();
        const extended_symbol &symbol = quantity.get_extended_symbol();
        const uint128_t &extended_symbol_s = extended_symbol_to_uint128(symbol);
        const auto &holder = sym_index.find(extended_symbol_s);
        eosio_assert(static_cast<uint32_t>(holder != sym_index.end()), "not yet created token");

        require_auth(quantity.contract);

        charge_fee(quantity.contract, N(issue));

        sym_index.modify(holder, 0, [&](auto &s) {
            s.tstats.supply += quantity.amount;
        });

        add_token_balance(quantity.contract, quantity);

        if (to != quantity.contract) {
            transfer_internal(quantity.contract, to, quantity, false);
        }
    }

    void token::redeem(const extended_asset &quantity, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must be positive quantity");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.name() != NATIVE_TOKEN_NAME), "cannot redeem native token with this operation");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        stats stats_table(get_self(), quantity.symbol.value);
        auto sym_index = stats_table.get_index<N(extendedsym)>();
        const extended_symbol &symbol = quantity.get_extended_symbol();
        const uint128_t &extended_symbol_s = extended_symbol_to_uint128(symbol);
        const auto &stats_holder = sym_index.find(extended_symbol_s);

        eosio_assert(static_cast<uint32_t>(stats_holder != sym_index.end()), "not yet created token");
        eosio_assert(static_cast<uint32_t>(quantity.amount <= stats_holder->tstats.supply), "redeem quantity exceeds supply amount");

        require_auth(quantity.contract);

        charge_fee(quantity.contract, N(redeem));

        sym_index.modify(stats_holder, 0, [&](auto &s) {
            s.tstats.supply -= quantity.amount;
        });

        sub_token_balance(quantity.contract, quantity);
    }

    void token::transfer(account_name from, account_name to, extended_asset quantity, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must transfer positive quantity");
        eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        transfer_internal(from, to, quantity, false);
    }

    void token::transfer_native_token(const account_name &from, const account_name &to, extended_asset quantity) {
        accounts_native accounts_table_native(get_self(), NATIVE_TOKEN);
        const auto &native_holder = accounts_table_native.get(from, "account not found");
        eosio_assert(static_cast<uint32_t>(native_holder.total_balance >= quantity.amount), "insufficient native token balance");

        stats_native stats(get_self(), NATIVE_TOKEN);
        for (auto &from_balance : native_holder.balance_map) {
            if (quantity.amount == 0) {
                break;
            }

            const auto &stats_holder = stats.get(from_balance.first, "native token is not created by the depository");
            eosio_assert(static_cast<uint32_t>(!from_balance.second.frozen), "account is frozen by issuer");
            eosio_assert(static_cast<uint32_t>(!stats_holder.tstats.frozen), "all transfers are frozen by depository");

            int64_t to_balance = 0;
            // subtract the balance from the 'from' account
            accounts_table_native.modify(native_holder, 0, [&](auto &_holder) {
                auto acc_balance = _holder.balance_map[from_balance.first];
                if (acc_balance.amount <= quantity.amount) {
                    to_balance = acc_balance.amount;
                    quantity.amount -= acc_balance.amount;
                    acc_balance.amount = 0;
                } else {
                    to_balance = quantity.amount;
                    acc_balance.amount -= quantity.amount;
                    quantity.amount = 0;
                }
                if (acc_balance.amount == 0) {
                    _holder.balance_map.erase(from_balance.first);
                } else {
                    _holder.balance_map[from_balance.first] = acc_balance;
                }
                _holder.total_balance -= to_balance;
            });

            add_native_token_balance(to, to_balance, from_balance.first);
        }
    }

    void token::transfer_token(const account_name &from, const account_name &to, const extended_asset &quantity) {
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

    void token::printsupply(const extended_symbol &symbol) {
        print(get_supply(symbol));
    }

    void token::printsupplyn(const account_name &depository) {
        stats_native stats(get_self(), NATIVE_TOKEN);
        const auto &holder = stats.get(depository, "native token is not created by the depository");
        print(holder.tstats.supply);
    }

    void token::printbalance(account_name owner, extended_symbol symbol) {
        if (symbol.name() == NATIVE_TOKEN_NAME) {
            accounts_native accounts_table(get_self(), NATIVE_TOKEN);
            const auto &balance_holder = accounts_table.get(owner, "account not found");
            print("total native : ", balance_holder.total_balance, "\n");
            for (auto &balance : balance_holder.balance_map) {
                print("depository=", balance.first);
                print(", balance=", balance.second.amount, "\n");
            }
        } else {
            accounts accounts_table(get_self(), symbol.value);
            const auto &balance_holder = accounts_table.get(owner, "account not found");

            for (auto &balance : balance_holder.balance_map) {
                print("symbol=");
                balance.first.print();
                print(", balance=", balance.second.amount, "\n");
            }
        }
    }

    void token::setfee(const name &operation_name, const extended_asset &fee) {
        //require_auth(N(yx.prods)); //TODO:multisig

        eosio_assert(static_cast<uint32_t>(fee.is_valid()), "wrong fee format");
        eosio_assert(static_cast<uint32_t>(fee.amount >= 0), "negative fee is not allowed");
        eosio_assert(static_cast<uint32_t>(operations.find(operation_name.value) != operations.end()), "wrong operation name");
        //TODO:NATIVE coin check, how?
        eosio_assert(static_cast<uint32_t>(fee.symbol.value == NATIVE_TOKEN), "only the native token is allowed for fee");

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

    void token::charge_fee(account_name payer, uint64_t operation) {
        fees fee_table(get_self(), get_self());
        const auto &fee_holder = fee_table.get(operation, "fee is not set");

        if (fee_holder.fee.amount > 0) {
            transfer_internal(payer, FEEDIST_ACCOUNT_NAME, fee_holder.fee, false);
        }
    }

    void token::createn(const account_name &depository) {
        //TODO:multisig
        //require_auth(N(yx.prods));
        //require_auth(N(eosio.prods));

        stats_native stats(get_self(), NATIVE_TOKEN);
        const auto &holder = stats.find(depository);
        eosio_assert(static_cast<uint32_t>(holder == stats.end()), "already created by the depository");

        stats.emplace(get_self(), [&](auto &s) {
            s.depository = depository;
        });
    }

    void token::issuen(const account_name &to, const extended_asset &quantity, const account_name &depository, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must be positive quantity");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.value == NATIVE_TOKEN), "cannot issue non-native token with this operation");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        stats_native stats(get_self(), NATIVE_TOKEN);
        const auto &holder = stats.get(depository, "native token is not created by the depository");

        require_auth(depository);

        if (to != depository) {
            charge_fee(depository, N(issuen));
        }
        //TODO:how to limit self-issuance properly?

        stats.modify(holder, 0, [&](auto &s) {
            s.tstats.supply += quantity.amount;
        });

        add_native_token_balance(depository, quantity.amount, depository);

        if (to != depository) {
            transfern_internal(depository, to, quantity, depository, false);
        }
    }

    void token::redeemn(const extended_asset &quantity, const account_name &depository, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must be positive quantity");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.value == NATIVE_TOKEN), "cannot redeem non-native token with this operation");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        stats_native stats(get_self(), NATIVE_TOKEN);
        const auto &stats_holder = stats.get(depository, "native token is not created by the depository");
        eosio_assert(static_cast<uint32_t>(stats_holder.tstats.supply >= quantity.amount), "insufficient supply of the native token of the specified depository");

        require_auth(depository);

        charge_fee(depository, N(redeemn));

        stats.modify(stats_holder, 0, [&](auto &s) {
            s.tstats.supply -= quantity.amount;
        });

        sub_native_token_balance(depository, quantity.amount, depository);
    }

    void token::add_native_token_balance(const account_name &owner, const int64_t &quantity, const account_name &depository) {
        accounts_native accounts_table_native(get_self(), NATIVE_TOKEN);
        const auto &native_holder = accounts_table_native.find(owner);

        if (native_holder == accounts_table_native.end()) {
            accounts_table_native.emplace(get_self(), [&](auto &holder) {
                holder.owner = owner;
                holder.total_balance = quantity;

                token_balance tkbal;
                tkbal.amount = quantity;
                holder.balance_map[depository] = tkbal;
            });
        } else {
            auto itr = native_holder->balance_map.find(depository);
            if (itr == native_holder->balance_map.end()) {
                accounts_table_native.modify(native_holder, 0, [&](auto &holder) {
                    holder.total_balance += quantity;

                    token_balance tkbal;
                    tkbal.amount = quantity;
                    holder.balance_map[depository] = tkbal;
                });
            } else {
                accounts_table_native.modify(native_holder, 0, [&](auto &holder) {
                    holder.total_balance += quantity;

                    token_balance tkbal = holder.balance_map[depository];
                    tkbal.amount += quantity;
                    holder.balance_map[depository] = tkbal;
                });
            }
        }
    }

    void token::sub_native_token_balance(const account_name &owner, const int64_t &quantity, const account_name &depository) {
        accounts_native accounts_table_native(get_self(), NATIVE_TOKEN);
        const auto &native_holder = accounts_table_native.get(owner, "account not found");
        auto itr = native_holder.balance_map.find(depository);
        eosio_assert(static_cast<uint32_t>(itr != native_holder.balance_map.end()), "account doesn't have native token of the specified depository");
        eosio_assert(static_cast<uint32_t>(itr->second.amount >= quantity), "insufficient native token of the specified depository");

        accounts_table_native.modify(native_holder, 0, [&](auto &holder) {
            holder.total_balance -= quantity;

            token_balance tkbal = holder.balance_map[depository];
            tkbal.amount -= quantity;
            holder.balance_map[depository] = tkbal;
        });
    }

    void token::transfern(const account_name &from, const account_name &to, const extended_asset &quantity,
                          const account_name &depository, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must transfer positive quantity");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.value == NATIVE_TOKEN), "cannot transfer non-native token with this operation");
        eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        transfern_internal(from, to, quantity, depository, false);
    }

    void token::clear(const extended_symbol &symbol) {
        require_auth(get_self());

        accounts accounts_table(get_self(), symbol.value);
        for (auto iterator = accounts_table.begin(); iterator != accounts_table.end(); ) {
            print("owner=", iterator->owner, "\n");
            iterator = accounts_table.erase(iterator);
        }

        stats stats_table(get_self(), symbol.value);
        for (auto iterator = stats_table.begin(); iterator != stats_table.end(); ) {
            print("supply=", iterator->tstats.supply, "\n");
            iterator = stats_table.erase(iterator);
        }
    }

    void token::clearn() {
        require_auth(get_self());

        accounts_native accounts_table(get_self(), NATIVE_TOKEN);
        for (auto iterator = accounts_table.begin(); iterator != accounts_table.end(); ) {
            iterator = accounts_table.erase(iterator);
        }

        stats_native stats(get_self(), NATIVE_TOKEN);
        for (auto iterator = stats.begin(); iterator != stats.end(); ) {
            iterator = stats.erase(iterator);
        }
    }

    void token::transfer_internal(account_name from, account_name to, extended_asset quantity, bool fee_required) {
        require_auth(from); //TODO:if from is FEEDIST_ACCOUNT_NAME, check multisig?
        eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");

        if (fee_required) {
            charge_fee(from, N(transfer));
        }

        require_recipient(from);
        require_recipient(to);

        if (quantity.symbol.name() == NATIVE_TOKEN_NAME) {
            transfer_native_token(from, to, quantity);
        } else {
            transfer_token(from, to, quantity);
        }
    }

    void token::transfern_internal(const account_name &from, const account_name &to, const extended_asset &quantity,
                                   const account_name &depository, bool fee_required) {
        print("transfern_internal : from=", from, ", to=", to, ", quantity=");
        quantity.print();
        print(", depository=", depository, ", fee=", fee_required, "\n");

        require_auth(from);
        eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");

        if (fee_required) {
            charge_fee(from, N(transfern));
        }

        require_recipient(from);
        require_recipient(to);

        sub_native_token_balance(from, quantity.amount, depository);
        add_native_token_balance(to, quantity.amount, depository);
    }
}

EOSIO_ABI(yosemitex::token, (create)(createn)(issue)(issuen)(redeem)(redeemn)(transfer)(transfern)(setfee)
                            (printsupply)(printsupplyn)(printbalance)(clear)(clearn))
