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

        stats_native stats(get_self(), issuer);
        const auto &holder = stats.find(NTOKEN_STATS_KEY);
        eosio_assert(static_cast<uint32_t>(holder == stats.end()), "already created");

        stats.emplace(get_self(), [&](auto &s) {
            s.supply = 0;
        });
    }

    void ntoken::issuen(const account_name &to, const yx_asset &quantity, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must be positive quantity");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.value == NATIVE_TOKEN), "cannot issue non-native token with this operation or wrong precision is specified");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        require_auth(quantity.issuer);

        stats_native stats(get_self(), quantity.issuer);
        const auto &tstats = stats.get(NTOKEN_STATS_KEY, "createn for the issuer is not called");

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

        require_auth(quantity.issuer);

        stats_native stats(get_self(), quantity.issuer);
        const auto &tstats = stats.get(NTOKEN_STATS_KEY, "createn for the issuer is not called");
        eosio_assert(static_cast<uint32_t>(tstats.supply >= quantity.amount), "insufficient supply of the native token of the specified depository");

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
        accounts_native_total from_total(get_self(), from);
        const auto &total_holder = from_total.get(NTOKEN_TOTAL_BALANCE_KEY, "from account doesn't have native token balance");
        eosio_assert(static_cast<uint32_t>(total_holder.amount >= quantity.amount), "insufficient native token balance");

        accounts_native accounts_table_native(get_self(), from);
        for (auto &from_balance_holder : accounts_table_native) {
            if (quantity.amount == 0) {
                break;
            }

            //TODO: add freeze functionality and check

            int64_t to_balance = 0;

            if (from_balance_holder.amount <= quantity.amount) {
                to_balance = from_balance_holder.amount;
                quantity.amount -= to_balance;
            } else {
                to_balance = quantity.amount;
                quantity.amount = 0;
            }

            sub_native_token_balance(from, to_balance, from_balance_holder.depository);
            add_native_token_balance(to, to_balance, from_balance_holder.depository);
        }
    }

    void ntoken::printsupplyn(const account_name &depository) {
        require_auth(get_self());

        stats_native stats(get_self(), depository);
        const auto &holder = stats.get(NTOKEN_STATS_KEY, "createn for system depository is not called");
        print(holder.supply);
    }

    void ntoken::printbalance(account_name owner, yx_symbol symbol) {
        require_auth(get_self());

        eosio_assert(static_cast<uint32_t>(symbol.value == NATIVE_TOKEN), "cannot print non-native token balance with this operation");

        accounts_native_total accounts_table_total(get_self(), owner);
        const auto &total_holder = accounts_table_total.get(NTOKEN_TOTAL_BALANCE_KEY, "account doesn't have native token balance");
        print("total native : ", total_holder.amount, "\n");

        accounts_native accounts_table(get_self(), owner);
        for (auto &balance : accounts_table) {
            print("depository=");
            printn(balance.depository);
            print(", balance=", balance.amount, "\n");
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
        accounts_native accounts_table_native(get_self(), owner);
        const auto &native_holder = accounts_table_native.find(depository);

        if (native_holder == accounts_table_native.end()) {
            accounts_table_native.emplace(get_self(), [&](auto &holder) {
                holder.depository = depository;
                holder.amount = quantity;
            });
        } else {
            accounts_table_native.modify(native_holder, 0, [&](auto &holder) {
                holder.amount += quantity;
            });
        }

        accounts_native_total accounts_table_total(get_self(), owner);
        const auto &total_holder = accounts_table_total.find(NTOKEN_TOTAL_BALANCE_KEY);
        if (total_holder == accounts_table_total.end()) {
            accounts_table_total.emplace(get_self(), [&](auto &tot_holder) {
                tot_holder.amount = quantity;
            });
        } else {
            accounts_table_total.modify(total_holder, 0, [&](auto &tot_holder) {
                tot_holder.amount += quantity;
            });
        }
    }

    void ntoken::sub_native_token_balance(const account_name &owner, const int64_t &quantity, const account_name &depository) {
        accounts_native accounts_table_native(get_self(), owner);
        const auto &native_holder = accounts_table_native.get(depository, "account doesn't have native token of the specified depository");
        eosio_assert(static_cast<uint32_t>(native_holder.amount >= quantity), "insufficient native token of the specified depository");

        accounts_table_native.modify(native_holder, 0, [&](auto &holder) {
            holder.amount -= quantity;
        });

        accounts_native_total accounts_table_total(get_self(), owner);
        const auto &total_holder = accounts_table_total.get(NTOKEN_TOTAL_BALANCE_KEY, "account doesn't have native token balance");
        accounts_table_total.modify(total_holder, 0, [&](auto &tot_holder) {
            tot_holder.amount -= quantity;
        });
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
                            (printsupplyn)(printbalance))
