#include "yx.ntoken.hpp"
#include <yosemitelib/system_accounts.hpp>
#include <yosemitelib/transaction_fee.hpp>
#include <yosemitelib/system_depository.hpp>
#include <yx.kyc/yx.kyc.hpp>


namespace yosemite {

    bool ntoken::is_auth_enough_for_transfer(uint32_t kycvector) {
        return ((kycvector & KYC_VECTOR_REAL_NAME_AUTH) == KYC_VECTOR_REAL_NAME_AUTH) &&
               ((kycvector & KYC_VECTOR_BANK_ACCOUNT_AUTH) == KYC_VECTOR_BANK_ACCOUNT_AUTH);
    }

    void ntoken::nissue(const account_name &to, const yx_asset &asset, const string &memo) {
        eosio_assert(static_cast<uint32_t>(asset.is_valid()), "invalid asset");
        eosio_assert(static_cast<uint32_t>(asset.amount > 0), "must be positive asset");
        eosio_assert(static_cast<uint32_t>(asset.symbol.value == YOSEMITE_NATIVE_TOKEN_SYMBOL),
                     "cannot issue non-native token with this operation or wrong precision is specified");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        require_auth(asset.issuer);
        eosio_assert(static_cast<uint32_t>(is_authorized_sys_depository(asset.issuer)),
                     "issuer account is not system depository");

        stats_native stats(get_self(), asset.issuer);
        const auto &tstats = stats.find(NTOKEN_BASIC_STATS_KEY);

        if (tstats == stats.end()) {
            stats.emplace(get_self(), [&](auto &s) {
                s.key = NTOKEN_BASIC_STATS_KEY;
                s.supply = asset.amount;
            });
        } else {
            charge_fee(asset.issuer, YOSEMITE_TX_FEE_OP_NAME_NTOKEN_ISSUE);

            stats.modify(tstats, 0, [&](auto &s) {
                s.supply += asset.amount;
                eosio_assert(static_cast<uint32_t>(s.supply > 0 && s.supply <= asset::max_amount),
                             "cannot issue token more than 2^62 - 1");
            });
        }

        add_native_token_balance(asset.issuer, asset);

        if (to != asset.issuer) {
            INLINE_ACTION_SENDER(yosemite::ntoken, ntransfer)
                    (get_self(), {{asset.issuer, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                     { asset.issuer, to, asset, memo });
        }
    }

    void ntoken::nredeem(const yx_asset &asset, const string &memo) {
        eosio_assert(static_cast<uint32_t>(asset.is_valid()), "invalid asset");
        eosio_assert(static_cast<uint32_t>(asset.amount > 0), "must be positive asset");
        eosio_assert(static_cast<uint32_t>(asset.symbol.value == YOSEMITE_NATIVE_TOKEN_SYMBOL), "cannot redeem non-native token with this operation or wrong precision is specified");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        require_auth(asset.issuer);

        stats_native stats(get_self(), asset.issuer);
        const auto &tstats = stats.get(NTOKEN_BASIC_STATS_KEY, "createn for the issuer is not called");
        eosio_assert(static_cast<uint32_t>(tstats.supply >= asset.amount), "insufficient supply of the native token of the specified depository");

        charge_fee(asset.issuer, YOSEMITE_TX_FEE_OP_NAME_NTOKEN_REDEEM);

        stats.modify(tstats, 0, [&](auto &s) {
            s.supply -= asset.amount;
        });

        sub_native_token_balance(asset.issuer, asset);
    }

    void ntoken::transfer(account_name from, account_name to, eosio::asset asset, const string &memo) {
        wptransfer(from, to, asset, from, memo);
    }

    void ntoken::wptransfer(account_name from, account_name to, eosio::asset asset, account_name payer, const string &memo) {
        eosio::asset txfee_amount;

        if (has_auth(YOSEMITE_SYSTEM_ACCOUNT)) {
            txfee_amount = eosio::asset{0, YOSEMITE_NATIVE_TOKEN_SYMBOL};
        } else {
            eosio_assert(static_cast<uint32_t>(asset.is_valid()), "invalid asset");
            eosio_assert(static_cast<uint32_t>(asset.amount > 0), "must transfer positive asset");
            eosio_assert(static_cast<uint32_t>(asset.symbol.value == YOSEMITE_NATIVE_TOKEN_SYMBOL),
                         "only native token is supported; use yx.token::transfer instead");
            eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");
            eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

            require_auth(from);
            eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");

            txfee_amount = charge_fee(payer, YOSEMITE_TX_FEE_OP_NAME_NTOKEN_TRANSFER);
        }

        // NOTE:We don't need notification to from and to account here. It's done by several ntrasfer operation.

        eosio_assert(static_cast<uint32_t>(is_auth_enough_for_transfer(kyc::get_kyc_vector(from))),
                     "authentication for from account is not enough");
        eosio_assert(static_cast<uint32_t>(is_auth_enough_for_transfer(kyc::get_kyc_vector(to))),
                     "authentication for to account is not enough");

        accounts_native_total from_total(get_self(), from);
        const auto &total_holder = from_total.get(NTOKEN_TOTAL_BALANCE_KEY, "from account doesn't have native token balance");
        if (txfee_amount.amount > 0) {
            int64_t sum = asset.amount + txfee_amount.amount;
            eosio_assert(static_cast<uint32_t>(sum > 0 && sum <= asset::max_amount), "sum with asset and fee amount cannot be more than 2^62 - 1");
            eosio_assert(static_cast<uint32_t>(total_holder.amount >= sum), "insufficient native token balance");
        }

        accounts_native accounts_table_native(get_self(), from);
        for (auto &from_balance_holder : accounts_table_native) {
            if (asset.amount == 0) {
                break;
            }

            // consider transaction fee by calculation because fee transaction is inline transaction
            // (not reflected at this time)
            int64_t from_balance = 0;
            if (txfee_amount.amount > 0) {
                if (from_balance_holder.amount <= txfee_amount.amount) {
                    from_balance = 0;
                    txfee_amount.amount -= from_balance_holder.amount;
                } else {
                    from_balance = from_balance_holder.amount - txfee_amount.amount;
                    txfee_amount.amount = 0;
                }
            } else {
                from_balance = from_balance_holder.amount;
            }
            if (from_balance <= 0) {
                continue;
            }

            int64_t to_balance = 0;
            if (from_balance <= asset.amount) {
                to_balance = from_balance;
                asset.amount -= to_balance;
            } else {
                to_balance = asset.amount;
                asset.amount = 0;
            }

            yx_symbol native_token_symbol{YOSEMITE_NATIVE_TOKEN_SYMBOL, from_balance_holder.depository};

            if (from == payer) {
                INLINE_ACTION_SENDER(yosemite::ntoken, ntransfer)
                        (get_self(), {{from, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                         { from, to, {to_balance, native_token_symbol}, memo });
            } else {
                INLINE_ACTION_SENDER(yosemite::ntoken, wpntransfer)
                        (get_self(), {{from, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                         { from, to, {to_balance, native_token_symbol}, payer, memo });
            }
        }
    }

    void ntoken::ntransfer(account_name from, account_name to, const yx_asset &asset, const string &memo) {
        wpntransfer(from, to, asset, from, memo);
    }

    void ntoken::wpntransfer(account_name from, account_name to, const yx_asset &asset, account_name payer,
                           const string &memo) {
        if (!has_auth(YOSEMITE_SYSTEM_ACCOUNT)) {
            eosio_assert(static_cast<uint32_t>(asset.is_valid()), "invalid asset");
            eosio_assert(static_cast<uint32_t>(asset.amount > 0), "must transfer positive asset");
            eosio_assert(static_cast<uint32_t>(asset.symbol.value == YOSEMITE_NATIVE_TOKEN_SYMBOL),
                         "cannot transfer non-native token with this operation or wrong precision is specified");
            eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");
            eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

            require_auth(from);
            eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");

            charge_fee(payer, YOSEMITE_TX_FEE_OP_NAME_NTOKEN_NTRANSFER);
        }

        eosio_assert(static_cast<uint32_t>(is_auth_enough_for_transfer(kyc::get_kyc_vector(from))),
                     "authentication for from account is not enough");
        eosio_assert(static_cast<uint32_t>(is_auth_enough_for_transfer(kyc::get_kyc_vector(to))),
                     "authentication for to account is not enough");

        require_recipient(from);
        require_recipient(to);

        sub_native_token_balance(from, asset);
        add_native_token_balance(to, asset);
    }

    void ntoken::payfee(account_name payer, asset asset) {
        require_auth(payer);
        require_auth(YOSEMITE_SYSTEM_ACCOUNT);

        if (asset.amount == 0) return;

        accounts_native_total from_total(get_self(), payer);
        const auto &total_holder = from_total.get(NTOKEN_TOTAL_BALANCE_KEY, "payer doesn't have native token balance");
        eosio_assert(static_cast<uint32_t>(total_holder.amount >= asset.amount), "insufficient native token balance");

        eosio_assert(static_cast<uint32_t>(is_auth_enough_for_transfer(kyc::get_kyc_vector(payer))),
                     "authentication for fee payer account is not enough");

        accounts_native accounts_table_native(get_self(), payer);
        for (auto &from_balance_holder : accounts_table_native) {
            if (asset.amount == 0) {
                break;
            }

            int64_t to_balance = 0;

            if (from_balance_holder.amount <= asset.amount) {
                to_balance = from_balance_holder.amount;
                asset.amount -= to_balance;
            } else {
                to_balance = asset.amount;
                asset.amount = 0;
            }

            yx_symbol native_token_symbol{YOSEMITE_NATIVE_TOKEN_SYMBOL, from_balance_holder.depository};
            INLINE_ACTION_SENDER(yosemite::ntoken, feetransfer)
                    (get_self(), {{payer, N(active)},{get_self(), N(active)}},
                     { payer, {to_balance, native_token_symbol} });
        }
    }

    // Assume that payfee operation is called only.
    void ntoken::feetransfer(account_name payer, const yx_asset &asset) {
        require_auth(payer);
        require_auth(get_self());

        require_recipient(payer);
        require_recipient(YOSEMITE_TX_FEE_ACCOUNT);

        sub_native_token_balance(payer, asset);
        add_native_token_balance(YOSEMITE_TX_FEE_ACCOUNT, asset);
    }

    asset ntoken::charge_fee(const account_name &payer, uint64_t operation) {
        auto tx_fee = yosemite::get_transaction_fee(operation);

        if (tx_fee.amount > 0) {
            INLINE_ACTION_SENDER(yosemite::ntoken, payfee)
                    (get_self(), {{payer, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                     {payer, tx_fee});
        }

        return tx_fee;
    }

    void ntoken::add_native_token_balance(const account_name &owner, const yx_asset &asset) {
        accounts_native accounts_table_native(get_self(), owner);
        const auto &native_holder = accounts_table_native.find(asset.issuer);

        if (native_holder == accounts_table_native.end()) {
            accounts_table_native.emplace(get_self(), [&](auto &holder) {
                holder.depository = asset.issuer;
                holder.amount = asset.amount;
            });
        } else {
            accounts_table_native.modify(native_holder, 0, [&](auto &holder) {
                holder.amount += asset.amount;
                eosio_assert(static_cast<uint32_t>(holder.amount > 0 && holder.amount <= asset::max_amount), "token amount cannot be more than 2^62 - 1");
            });
        }

        accounts_native_total accounts_table_total(get_self(), owner);
        const auto &total_holder = accounts_table_total.find(NTOKEN_TOTAL_BALANCE_KEY);
        if (total_holder == accounts_table_total.end()) {
            accounts_table_total.emplace(get_self(), [&](auto &tot_holder) {
                tot_holder.amount = asset.amount;
            });
        } else {
            accounts_table_total.modify(total_holder, 0, [&](auto &tot_holder) {
                tot_holder.amount += asset.amount;
                eosio_assert(static_cast<uint32_t>(tot_holder.amount > 0 && tot_holder.amount <= asset::max_amount), "token amount cannot be more than 2^62 - 1");
            });
        }
    }

    void ntoken::sub_native_token_balance(const account_name &owner, const yx_asset &asset) {
        accounts_native accounts_table_native(get_self(), owner);
        const auto &native_holder = accounts_table_native.get(asset.issuer, "account doesn't have native token of the specified depository");
        eosio_assert(static_cast<uint32_t>(native_holder.amount >= asset.amount), "insufficient native token of the specified depository");

        accounts_table_native.modify(native_holder, 0, [&](auto &holder) {
            holder.amount -= asset.amount;
        });

        accounts_native_total accounts_table_total(get_self(), owner);
        const auto &total_holder = accounts_table_total.get(NTOKEN_TOTAL_BALANCE_KEY, "account doesn't have native token balance");
        eosio_assert(static_cast<uint32_t>(total_holder.amount >= asset.amount), "insufficient total native token");

        accounts_table_total.modify(total_holder, 0, [&](auto &tot_holder) {
            tot_holder.amount -= asset.amount;
        });
    }

}

EOSIO_ABI(yosemite::ntoken, (nissue)(nredeem)(transfer)(wptransfer)(ntransfer)(wpntransfer)
                            (payfee)(feetransfer)
)
