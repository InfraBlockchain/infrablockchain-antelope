#include "yx.ntoken.hpp"
#include <yx.kyc/yx.kyc.hpp>
#include <yx.system/yx.system.hpp>

//TODO:1. freeze by account/token

namespace yosemite {

    bool ntoken::is_auth_enought_for_transfer(uint32_t kycvector) {
        return ((kycvector & KYC_VECTOR_REAL_NAME_AUTH) == KYC_VECTOR_REAL_NAME_AUTH) &&
               ((kycvector & KYC_VECTOR_BANK_ACCOUNT_AUTH) == KYC_VECTOR_BANK_ACCOUNT_AUTH);
    }

    void ntoken::createn(const account_name &issuer) {
        eosio_assert(static_cast<uint32_t>(yosemitesys::system_contract::is_authorized_sys_depository(issuer)),
                     "issuer for native token must be authrized as system depository");
        require_auth(issuer);

        stats_native stats(get_self(), NATIVE_TOKEN);
        const auto &holder = stats.find(issuer);
        eosio_assert(static_cast<uint32_t>(holder == stats.end()), "already registered");

        stats.emplace(get_self(), [&](auto &s) {
            s.depository = issuer;
        });
    }

    void ntoken::issuen(const account_name &to, const yx_asset &quantity, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must be positive quantity");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.value == NATIVE_TOKEN), "cannot issue non-native token with this operation or wrong precision is specified");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        stats_native stats(get_self(), NATIVE_TOKEN);
        const auto &tstats = stats.get(quantity.issuer, "depository is not registered");

        require_auth(quantity.issuer);

        if (to != quantity.issuer) {
            charge_fee(quantity.issuer, N(issuen));
        }
        //TODO:how to limit self-issuance properly?

        stats.modify(tstats, 0, [&](auto &s) {
            s.supply += quantity.amount;
        });

        add_native_token_balance(quantity.issuer, quantity.amount, quantity.issuer);

        if (to != quantity.issuer) {
            transfern_internal(quantity.issuer, to, quantity, false, 0);
        }
    }

    void ntoken::redeemn(const yx_asset &quantity, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must be positive quantity");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.value == NATIVE_TOKEN), "cannot redeem non-native token with this operation or wrong precision is specified");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        stats_native stats(get_self(), NATIVE_TOKEN);
        const auto &tstats = stats.get(quantity.issuer, "depository is not registered");
        eosio_assert(static_cast<uint32_t>(tstats.supply >= quantity.amount), "insufficient supply of the native token of the specified depository");

        require_auth(quantity.issuer);

        charge_fee(quantity.issuer, N(redeemn));

        stats.modify(tstats, 0, [&](auto &s) {
            s.supply -= quantity.amount;
        });

        sub_native_token_balance(quantity.issuer, quantity.amount, quantity.issuer);
    }

    void ntoken::transfer(account_name from, account_name to, yx_asset quantity, account_name payer, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.symbol.value == NATIVE_TOKEN), "only native token is supported; use yx.token::transfer instead");
        eosio_assert(static_cast<uint32_t>(quantity.issuer == 0), "invalid issuer : empty string is only allowed");
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must transfer positive quantity");
        eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        eosio_assert(static_cast<uint32_t>(is_auth_enought_for_transfer(kyc::get_kyc_vector(from))),
                     "authentication for from account is not enough");
        eosio_assert(static_cast<uint32_t>(is_auth_enought_for_transfer(kyc::get_kyc_vector(to))),
                     "authentication for to account is not enough");

        transfer_internal(from, to, quantity, from != FEEDIST_ACCOUNT_NAME && to != FEEDIST_ACCOUNT_NAME, payer);
    }

    void ntoken::transfern(account_name from, account_name to, const yx_asset &quantity, account_name payer, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must transfer positive quantity");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.value == NATIVE_TOKEN), "cannot transfer non-native token with this operation or wrong precision is specified");
        eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        transfern_internal(from, to, quantity, true, payer);
    }

    void ntoken::transfer_native_token(const account_name &from, const account_name &to, yx_asset quantity) {
        accounts_native accounts_table_native(get_self(), NATIVE_TOKEN);
        const auto &native_holder = accounts_table_native.get(from, "account not found");
        eosio_assert(static_cast<uint32_t>(native_holder.total_balance >= quantity.amount), "insufficient native token balance");

        stats_native stats(get_self(), NATIVE_TOKEN);
        for (auto &from_balance : native_holder.balance_map) {
            if (quantity.amount == 0) {
                break;
            }

            const auto &tstats = stats.get(from_balance.first, "native token is not created by the depository");
            eosio_assert(static_cast<uint32_t>(!from_balance.second.frozen), "account is frozen by issuer");
            eosio_assert(static_cast<uint32_t>(!tstats.frozen), "all transfers are frozen by depository");

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

    void ntoken::printsupplyn(const account_name &depository) {
        stats_native stats(get_self(), NATIVE_TOKEN);
        const auto &holder = stats.get(depository, "depository for the native token is not registered");
        print(holder.supply);
    }

    void ntoken::printbalance(account_name owner, yx_symbol symbol) {
        eosio_assert(static_cast<uint32_t>(symbol.value == NATIVE_TOKEN), "cannot print non-native token balance with this operation");

        accounts_native accounts_table(get_self(), NATIVE_TOKEN);
        const auto &balance_holder = accounts_table.get(owner, "account not found");
        print("total native : ", balance_holder.total_balance, "\n");
        for (auto &balance : balance_holder.balance_map) {
            print("depository=", balance.first);
            print(", balance=", balance.second.amount, "\n");
        }
    }

    bool ntoken::check_fee_operation(const uint64_t &operation_name) {
        return operation_name == N(createn) ||
               operation_name == N(issuen) ||
               operation_name == N(redeemn) ||
               operation_name == N(transfer) ||
               operation_name == N(transfern)
               ;
    }

    void ntoken::charge_fee(const account_name &payer, uint64_t operation) {
        fees fee_table(get_self(), get_self());
        const auto &fee_holder = fee_table.get(operation, "fee is not set or unknown operation");

        if (fee_holder.fee.amount > 0) {
            transfer_internal(payer, FEEDIST_ACCOUNT_NAME, {fee_holder.fee, 0}, false, 0);
        }
    }

    void ntoken::add_native_token_balance(const account_name &owner, const int64_t &quantity, const account_name &depository) {
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

    void ntoken::sub_native_token_balance(const account_name &owner, const int64_t &quantity, const account_name &depository) {
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

    void ntoken::clearn() {
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

    void ntoken::transfer_internal(account_name from, account_name to, yx_asset quantity, bool fee_required,
                                   account_name payer) {
        require_auth(from);
        eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");

        if (fee_required) {
            if (from != payer) {
                require_auth(payer);
                eosio_assert(static_cast<uint32_t>(is_auth_enought_for_transfer(kyc::get_kyc_vector(payer))),
                             "authentication for fee payer account is not enough");
            }
            charge_fee(payer, N(transfer));
        }

        require_recipient(from);
        require_recipient(to);

        transfer_native_token(from, to, quantity);
    }

    void ntoken::transfern_internal(const account_name &from, const account_name &to, const yx_asset &quantity,
                                    bool fee_required, const account_name &payer) {
        require_auth(from);
        eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");

        eosio_assert(static_cast<uint32_t>(is_auth_enought_for_transfer(kyc::get_kyc_vector(from))),
                     "authentication for from account is not enough");
        eosio_assert(static_cast<uint32_t>(is_auth_enought_for_transfer(kyc::get_kyc_vector(to))),
                     "authentication for to account is not enough");

        if (fee_required) {
            if (from != payer) {
                require_auth(payer);
                eosio_assert(static_cast<uint32_t>(is_auth_enought_for_transfer(kyc::get_kyc_vector(payer))),
                             "authentication for fee payer account is not enough");
            }
            charge_fee(payer, N(transfern));
        }

        require_recipient(from);
        require_recipient(to);

        sub_native_token_balance(from, quantity.amount, quantity.issuer);
        add_native_token_balance(to, quantity.amount, quantity.issuer);
    }
}

EOSIO_ABI(yosemite::ntoken, (createn)(issuen)(redeemn)(transfer)(transfern)(setfee)
                            (printsupplyn)(printbalance)(clearn))
