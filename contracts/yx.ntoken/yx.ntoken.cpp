#include "yx.ntoken.hpp"
#include <yosemitelib/system_accounts.hpp>
#include <yx.kyc/yx.kyc.hpp>
#include <yx.system/yx.system.hpp>


namespace yosemite {

    bool ntoken::is_auth_enough_for_transfer(uint32_t kycvector) {
        return ((kycvector & KYC_VECTOR_REAL_NAME_AUTH) == KYC_VECTOR_REAL_NAME_AUTH) &&
               ((kycvector & KYC_VECTOR_BANK_ACCOUNT_AUTH) == KYC_VECTOR_BANK_ACCOUNT_AUTH);
    }

    void ntoken::nissue(const account_name &to, const yx_asset &quantity, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must be positive quantity");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.value == NATIVE_TOKEN), "cannot issue non-native token with this operation or wrong precision is specified");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        require_auth(quantity.issuer);
        eosio_assert(static_cast<uint32_t>(yosemitesys::system_contract::is_authorized_sys_depository(quantity.issuer)),
                     "issuer account is not system depository");

        stats_native stats(get_self(), quantity.issuer);
        const auto &tstats = stats.find(NTOKEN_BASIC_STATS_KEY);

        if (tstats == stats.end()) {
            stats.emplace(get_self(), [&](auto &s) {
                s.key = NTOKEN_BASIC_STATS_KEY;
                s.supply = quantity.amount;
            });
        } else {
            charge_fee(quantity.issuer, N(nissue));

            stats.modify(tstats, 0, [&](auto &s) {
                s.supply += quantity.amount;
                eosio_assert(static_cast<uint32_t>(s.supply > 0 && s.supply <= asset::max_amount),
                             "cannot issue token more than 2^62 - 1");
            });
        }

        add_native_token_balance(quantity.issuer, quantity);

        if (to != quantity.issuer) {
            INLINE_ACTION_SENDER(yosemite::ntoken, ntransfer)
                    (get_self(), {{quantity.issuer, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                     { quantity.issuer, to, quantity, quantity.issuer, memo });
        }
    }

    void ntoken::nredeem(const yx_asset &quantity, const string &memo) {
        eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
        eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must be positive quantity");
        eosio_assert(static_cast<uint32_t>(quantity.symbol.value == NATIVE_TOKEN), "cannot redeem non-native token with this operation or wrong precision is specified");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        require_auth(quantity.issuer);

        stats_native stats(get_self(), quantity.issuer);
        const auto &tstats = stats.get(NTOKEN_BASIC_STATS_KEY, "createn for the issuer is not called");
        eosio_assert(static_cast<uint32_t>(tstats.supply >= quantity.amount), "insufficient supply of the native token of the specified depository");

        charge_fee(quantity.issuer, N(nredeem));

        stats.modify(tstats, 0, [&](auto &s) {
            s.supply -= quantity.amount;
        });

        sub_native_token_balance(quantity.issuer, quantity);
    }

    void ntoken::transfer(account_name from, account_name to, eosio::asset quantity, account_name payer, const string &memo) {
        if (!has_auth(YOSEMITE_SYSTEM_ACCOUNT)) {
            eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
            eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must transfer positive quantity");
            eosio_assert(static_cast<uint32_t>(quantity.symbol.value == NATIVE_TOKEN),
                         "only native token is supported; use yx.token::transfer instead");
            eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");
            eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

            eosio_assert(static_cast<uint32_t>(is_auth_enough_for_transfer(kyc::get_kyc_vector(from))),
                         "authentication for from account is not enough");
            eosio_assert(static_cast<uint32_t>(is_auth_enough_for_transfer(kyc::get_kyc_vector(to))),
                         "authentication for to account is not enough");

            require_auth(from);
            eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");

            if (from != payer) {
                require_auth(payer);
                eosio_assert(static_cast<uint32_t>(is_auth_enough_for_transfer(kyc::get_kyc_vector(payer))),
                             "authentication for fee payer account is not enough");
            }
            charge_fee(payer, N(transfer));
        }

        // NOTE:We don't need notification to from and to account here. It's done by several ntrasfer operation.

        accounts_native_total from_total(get_self(), from);
        const auto &total_holder = from_total.get(NTOKEN_TOTAL_BALANCE_KEY, "from account doesn't have native token balance");
        eosio_assert(static_cast<uint32_t>(total_holder.amount >= quantity.amount), "insufficient native token balance");

        accounts_native accounts_table_native(get_self(), from);
        for (auto &from_balance_holder : accounts_table_native) {
            if (quantity.amount == 0) {
                break;
            }

            int64_t to_balance = 0;

            if (from_balance_holder.amount <= quantity.amount) {
                to_balance = from_balance_holder.amount;
                quantity.amount -= to_balance;
            } else {
                to_balance = quantity.amount;
                quantity.amount = 0;
            }

            yx_symbol native_token_symbol{NATIVE_TOKEN, from_balance_holder.depository};
            INLINE_ACTION_SENDER(yosemite::ntoken, ntransfer)
                    (get_self(), {{from, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                     { from, to, {to_balance, native_token_symbol}, payer, memo });
        }
    }

    void ntoken::ntransfer(account_name from, account_name to, const yx_asset &quantity, account_name payer,
                           const string &memo) {
        if (!has_auth(YOSEMITE_SYSTEM_ACCOUNT)) {
            eosio_assert(static_cast<uint32_t>(quantity.is_valid()), "invalid quantity");
            eosio_assert(static_cast<uint32_t>(quantity.amount > 0), "must transfer positive quantity");
            eosio_assert(static_cast<uint32_t>(quantity.symbol.value == NATIVE_TOKEN),
                         "cannot transfer non-native token with this operation or wrong precision is specified");
            eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");
            eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

            require_auth(from);
            eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");

            eosio_assert(static_cast<uint32_t>(is_auth_enough_for_transfer(kyc::get_kyc_vector(from))),
                         "authentication for from account is not enough");
            eosio_assert(static_cast<uint32_t>(is_auth_enough_for_transfer(kyc::get_kyc_vector(to))),
                         "authentication for to account is not enough");

            if (from != payer) {
                require_auth(payer);
                eosio_assert(static_cast<uint32_t>(is_auth_enough_for_transfer(kyc::get_kyc_vector(payer))),
                             "authentication for fee payer account is not enough");
            }
            charge_fee(payer, N(ntransfer));
        }

        require_recipient(from);
        require_recipient(to);

        sub_native_token_balance(from, quantity);
        add_native_token_balance(to, quantity);
    }

    void ntoken::payfee(account_name payer, asset quantity) {
        require_auth(payer);
        require_auth(YOSEMITE_SYSTEM_ACCOUNT);

        accounts_native_total from_total(get_self(), payer);
        const auto &total_holder = from_total.get(NTOKEN_TOTAL_BALANCE_KEY, "payer doesn't have native token balance");
        eosio_assert(static_cast<uint32_t>(total_holder.amount >= quantity.amount), "insufficient native token balance");

        accounts_native accounts_table_native(get_self(), payer);
        for (auto &from_balance_holder : accounts_table_native) {
            if (quantity.amount == 0) {
                break;
            }

            int64_t to_balance = 0;

            if (from_balance_holder.amount <= quantity.amount) {
                to_balance = from_balance_holder.amount;
                quantity.amount -= to_balance;
            } else {
                to_balance = quantity.amount;
                quantity.amount = 0;
            }

            yx_symbol native_token_symbol{NATIVE_TOKEN, from_balance_holder.depository};
            INLINE_ACTION_SENDER(yosemite::ntoken, feetransfer)
                    (get_self(), {{payer, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                     { payer, {to_balance, native_token_symbol} });
        }
    }

    void ntoken::feetransfer(account_name payer, const yx_asset &quantity) {
        require_auth(payer);
        require_auth(YOSEMITE_SYSTEM_ACCOUNT);

        require_recipient(payer);
        require_recipient(YOSEMITE_TX_FEE_ACCOUNT);

        sub_native_token_balance(payer, quantity);
        add_native_token_balance(YOSEMITE_TX_FEE_ACCOUNT, quantity);
    }

    bool ntoken::check_fee_operation(const uint64_t &operation_name) {
        return operation_name == N(nissue) ||
               operation_name == N(nredeem) ||
               operation_name == N(transfer) ||
               operation_name == N(ntransfer)
               ;
    }

    void ntoken::charge_fee(const account_name &payer, uint64_t operation) {
        fees fee_table(get_self(), get_self());
        const auto &fee_holder = fee_table.get(operation, "fee is not set or unknown operation");

        if (fee_holder.fee.amount > 0) {
            INLINE_ACTION_SENDER(yosemite::ntoken, payfee)
                    (get_self(), {{payer, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                     {payer, fee_holder.fee});
        }
    }

    void ntoken::add_native_token_balance(const account_name &owner, const yx_asset &quantity) {
        accounts_native accounts_table_native(get_self(), owner);
        const auto &native_holder = accounts_table_native.find(quantity.issuer);

        if (native_holder == accounts_table_native.end()) {
            accounts_table_native.emplace(get_self(), [&](auto &holder) {
                holder.depository = quantity.issuer;
                holder.amount = quantity.amount;
            });
        } else {
            accounts_table_native.modify(native_holder, 0, [&](auto &holder) {
                holder.amount += quantity.amount;
                eosio_assert(static_cast<uint32_t>(holder.amount > 0 && holder.amount <= asset::max_amount), "token amount cannot be more than 2^62 - 1");
            });
        }

        accounts_native_total accounts_table_total(get_self(), owner);
        const auto &total_holder = accounts_table_total.find(NTOKEN_TOTAL_BALANCE_KEY);
        if (total_holder == accounts_table_total.end()) {
            accounts_table_total.emplace(get_self(), [&](auto &tot_holder) {
                tot_holder.amount = quantity.amount;
            });
        } else {
            accounts_table_total.modify(total_holder, 0, [&](auto &tot_holder) {
                tot_holder.amount += quantity.amount;
                eosio_assert(static_cast<uint32_t>(tot_holder.amount > 0 && tot_holder.amount <= asset::max_amount), "token amount cannot be more than 2^62 - 1");
            });
        }
    }

    void ntoken::sub_native_token_balance(const account_name &owner, const yx_asset &quantity) {
        accounts_native accounts_table_native(get_self(), owner);
        const auto &native_holder = accounts_table_native.get(quantity.issuer, "account doesn't have native token of the specified depository");
        eosio_assert(static_cast<uint32_t>(native_holder.amount >= quantity.amount), "insufficient native token of the specified depository");

        accounts_table_native.modify(native_holder, 0, [&](auto &holder) {
            holder.amount -= quantity.amount;
        });

        accounts_native_total accounts_table_total(get_self(), owner);
        const auto &total_holder = accounts_table_total.get(NTOKEN_TOTAL_BALANCE_KEY, "account doesn't have native token balance");
        accounts_table_total.modify(total_holder, 0, [&](auto &tot_holder) {
            tot_holder.amount -= quantity.amount;
        });
    }

}

EOSIO_ABI(yosemite::ntoken, (nissue)(nredeem)(transfer)(ntransfer)(setfee)
                            (payfee)(feetransfer)
)
