#include <yosemitelib/native_token.hpp>
#include <yosemitelib/system_accounts.hpp>
#include <yosemitelib/transaction_fee.hpp>
#include <yosemitelib/system_depository.hpp>
#include <yosemitelib/transaction_vote.h>

namespace yosemite { namespace native_token {

    bool ntoken::check_identity_auth_for_transfer(account_name account, const ntoken_kyc_rule_type &kycrule_type) {
        eosio_assert(static_cast<uint32_t>(!identity::has_account_state(account, YOSEMITE_ID_ACC_STATE_BLACKLISTED)),
                     "account is blacklisted by identity authority");
        switch (kycrule_type) {
            case NTOKEN_KYC_RULE_TYPE_TRANSFER_SEND:
                eosio_assert(static_cast<uint32_t>(!identity::has_account_state(account, YOSEMITE_ID_ACC_STATE_BLACKLISTED_NTOKEN_SEND)),
                             "account is send-blacklisted by identity authority");
                break;
            case NTOKEN_KYC_RULE_TYPE_TRANSFER_RECEIVE:
                eosio_assert(static_cast<uint32_t>(!identity::has_account_state(account, YOSEMITE_ID_ACC_STATE_BLACKLISTED_NTOKEN_RECEIVE)),
                             "account is receive-blacklisted by identity authority");
                break;
            case NTOKEN_KYC_RULE_TYPE_MAX:
                // ignored
                break;
        }

        kyc_rule_index kyc_rule(get_self(), get_self());
        const auto &rule = kyc_rule.get(kycrule_type, "KYC rule is not set; use setkycrule operation to set");

        return identity::has_all_kyc_status(account, rule.kyc_flags);
    }

    void ntoken::nissue(const account_name &to, const yx_asset &token, const string &memo) {
        eosio_assert(static_cast<uint32_t>(token.is_valid()), "invalid token");
        eosio_assert(static_cast<uint32_t>(token.amount > 0), "must be positive token");
        eosio_assert(static_cast<uint32_t>(token.symbol.value == YOSEMITE_NATIVE_TOKEN_SYMBOL),
                     "cannot issue non-native token with this operation or wrong precision is specified");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        require_auth(token.issuer);
        eosio_assert(static_cast<uint32_t>(is_authorized_sys_depository(token.issuer)),
                     "issuer account is not system depository");

        stats_native stats(get_self(), token.issuer);
        const auto &tstats = stats.find(NTOKEN_BASIC_STATS_KEY);

        if (tstats == stats.end()) {
            stats.emplace(get_self(), [&](auto &s) {
                s.key = NTOKEN_BASIC_STATS_KEY;
                s.supply = asset{token.amount, YOSEMITE_NATIVE_TOKEN_SYMBOL};
            });
        } else {
            stats.modify(tstats, 0, [&](auto &s) {
                s.supply.amount += token.amount;
                eosio_assert(static_cast<uint32_t>(s.supply.amount > 0 && s.supply.amount <= asset::max_amount),
                             "cannot issue token more than 2^62 - 1");
            });
        }

        add_native_token_balance(token.issuer, token);

        if (to != token.issuer) {
            INLINE_ACTION_SENDER(ntoken, ntransfer)
                    (get_self(), {{token.issuer, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                     {token.issuer, to, token, memo});
        }

        charge_transaction_fee(token.issuer, YOSEMITE_TX_FEE_OP_NAME_NTOKEN_ISSUE);
    }

    void ntoken::nredeem(const yx_asset &token, const string &memo) {
        eosio_assert(static_cast<uint32_t>(token.is_valid()), "invalid token");
        eosio_assert(static_cast<uint32_t>(token.amount > 0), "must be positive token");
        eosio_assert(static_cast<uint32_t>(token.symbol.value == YOSEMITE_NATIVE_TOKEN_SYMBOL),
                     "cannot redeem non-native token with this operation or wrong precision is specified");
        eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

        require_auth(token.issuer);
        eosio_assert(static_cast<uint32_t>(is_authorized_sys_depository(token.issuer)),
                     "issuer account is not system depository");

        stats_native stats(get_self(), token.issuer);
        const auto &tstats = stats.get(NTOKEN_BASIC_STATS_KEY, "createn for the issuer is not called");
        eosio_assert(static_cast<uint32_t>(tstats.supply.amount >= token.amount),
                     "insufficient supply of the native token of the specified depository");

        stats.modify(tstats, 0, [&](auto &s) {
            s.supply.amount -= token.amount;
        });

        sub_native_token_balance(token.issuer, token);

        charge_transaction_fee(token.issuer, YOSEMITE_TX_FEE_OP_NAME_NTOKEN_REDEEM);
    }

    void ntoken::transfer(account_name from, account_name to, eosio::asset amount, const string &memo) {
        bool called_by_system_contract = has_auth(YOSEMITE_SYSTEM_ACCOUNT);
        if (!called_by_system_contract) {
            eosio_assert(static_cast<uint32_t>(yx_asset{amount, 0}.is_valid()), "invalid amount");
            eosio_assert(static_cast<uint32_t>(amount.amount > 0), "must transfer positive amount");
            eosio_assert(static_cast<uint32_t>(amount.symbol.value == YOSEMITE_NATIVE_TOKEN_SYMBOL),
                         "only native token is supported; use yx.token::transfer instead");
            eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");
            eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

            require_auth(from);
            eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");
        }

        // NOTE:We don't need notification to from and to account here. It's done by several ntransfer operation.

        eosio_assert(static_cast<uint32_t>(check_identity_auth_for_transfer(from, NTOKEN_KYC_RULE_TYPE_TRANSFER_SEND)),
                     "KYC authentication for from account is failed");
        eosio_assert(static_cast<uint32_t>(check_identity_auth_for_transfer(to, NTOKEN_KYC_RULE_TYPE_TRANSFER_RECEIVE)),
                     "KYC authentication for to account is failed");

        vector<account_name> zeroedout_depos{};
        yx_asset remained_ntoken{};
        const account_name delegated_payer = delegated_trx_fee_payer();
        const account_name payer = delegated_payer != 0 ? delegated_payer : from;

        // In order to check whether from account have enough native token to pay the transaction fee,
        // it tracks the native token balance of the specific system depository of 'from' account.
        accounts_native accounts_table_native(get_self(), from);
        for (auto &balance_holder : accounts_table_native) {
            yx_symbol native_token_symbol{YOSEMITE_NATIVE_TOKEN_SYMBOL, balance_holder.token.issuer};

            int64_t to_balance = 0;
            if (balance_holder.token.amount <= amount.amount) {
                to_balance = balance_holder.token.amount;
                amount.amount -= to_balance;
                if (from == payer) {
                    zeroedout_depos.push_back(balance_holder.token.issuer);
                }
            } else {
                to_balance = amount.amount;
                amount.amount = 0;
                if (from == payer) {
                    remained_ntoken = yx_asset{balance_holder.token.amount - to_balance, native_token_symbol};
                }
            }

            INLINE_ACTION_SENDER(ntoken, ntransfer)
                    (get_self(), {{from, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                     {from, to, {to_balance, native_token_symbol}, memo});

            if (amount.amount == 0) {
                if (!called_by_system_contract) {
                    if (from == payer) {
                        charge_transaction_fee(payer, YOSEMITE_TX_FEE_OP_NAME_NTOKEN_TRANSFER, zeroedout_depos,
                                               remained_ntoken);
                    } else{
                        charge_transaction_fee(payer, YOSEMITE_TX_FEE_OP_NAME_NTOKEN_TRANSFER);
                    }
                }
                break;
            }
        }

        eosio_assert(static_cast<uint32_t>(amount.amount == 0), "from account cannot afford native token amount");
    }

    void ntoken::ntransfer(account_name from, account_name to, const yx_asset &token, const string &memo) {
        bool called_by_system_contract = has_auth(YOSEMITE_SYSTEM_ACCOUNT);
        if (!called_by_system_contract) {
            eosio_assert(static_cast<uint32_t>(token.is_valid()), "invalid token");
            eosio_assert(static_cast<uint32_t>(token.amount > 0), "must transfer positive amount");
            eosio_assert(static_cast<uint32_t>(token.symbol.value == YOSEMITE_NATIVE_TOKEN_SYMBOL),
                         "only native token is supported; use yx.token::transfer instead");
            eosio_assert(static_cast<uint32_t>(from != to), "from and to account cannot be the same");
            eosio_assert(static_cast<uint32_t>(memo.size() <= 256), "memo has more than 256 bytes");

            require_auth(from);
            eosio_assert(static_cast<uint32_t>(is_account(to)), "to account does not exist");
        }

        eosio_assert(static_cast<uint32_t>(check_identity_auth_for_transfer(from, NTOKEN_KYC_RULE_TYPE_TRANSFER_SEND)),
                     "KYC authentication for from account is failed");
        eosio_assert(static_cast<uint32_t>(check_identity_auth_for_transfer(to, NTOKEN_KYC_RULE_TYPE_TRANSFER_RECEIVE)),
                     "KYC authentication for to account is failed");

        require_recipient(from);
        require_recipient(to);

        sub_native_token_balance(from, token);
        add_native_token_balance(to, token);

        if (!called_by_system_contract) {
            charge_transaction_fee(from, YOSEMITE_TX_FEE_OP_NAME_NTOKEN_NTRANSFER);
        }
    }

    void ntoken::payfee(account_name payer, yx_asset token) {
        require_auth(payer); // just for sure
        require_auth(YOSEMITE_SYSTEM_ACCOUNT);

        if (token.amount <= 0) return;

        eosio_assert(static_cast<uint32_t>(check_identity_auth_for_transfer(payer, NTOKEN_KYC_RULE_TYPE_TRANSFER_SEND)),
                     "KYC authentication for the fee payer account is failed");

        require_recipient(payer);
        require_recipient(YOSEMITE_TX_FEE_ACCOUNT);

        sub_native_token_balance(payer, token);
        add_native_token_balance(YOSEMITE_TX_FEE_ACCOUNT, token);

        cast_transaction_vote(static_cast<uint32_t>(token.amount));
    }

    void ntoken::add_native_token_balance(const account_name &owner, const yx_asset &token) {
        accounts_native accounts_table_native(get_self(), owner);
        const auto &native_holder = accounts_table_native.find(token.issuer);

        if (native_holder == accounts_table_native.end()) {
            accounts_table_native.emplace(get_self(), [&](auto &holder) {
                holder.token = token;
            });
        } else {
            accounts_table_native.modify(native_holder, 0, [&](auto &holder) {
                holder.token += token;
                eosio_assert(static_cast<uint32_t>(holder.token.amount > 0 && holder.token.amount <= asset::max_amount),
                             "token amount cannot be more than 2^62 - 1");
            });
        }

        accounts_native_total accounts_table_total(get_self(), owner);
        const auto &total_holder = accounts_table_total.find(NTOKEN_TOTAL_BALANCE_KEY);
        if (total_holder == accounts_table_total.end()) {
            accounts_table_total.emplace(get_self(), [&](auto &tot_holder) {
                tot_holder.amount.amount = token.amount;
                tot_holder.amount.symbol = token.symbol;
            });
        } else {
            accounts_table_total.modify(total_holder, 0, [&](auto &tot_holder) {
                tot_holder.amount.amount += token.amount;
                eosio_assert(static_cast<uint32_t>(tot_holder.amount.amount > 0 && tot_holder.amount.amount <= asset::max_amount),
                             "token amount cannot be more than 2^62 - 1");
            });
        }
    }

    void ntoken::sub_native_token_balance(const account_name &owner, const yx_asset &token) {
        accounts_native accounts_table_native(get_self(), owner);
        const auto &native_holder = accounts_table_native.get(token.issuer, "account doesn't have native token of the specified depository");
        eosio_assert(static_cast<uint32_t>(native_holder.token.amount >= token.amount), "insufficient native token of the specified depository");

        bool erase;
        accounts_table_native.modify(native_holder, 0, [&](auto &holder) {
            holder.token -= token;
            erase = holder.token.amount == 0;
        });
        if (erase) {
            accounts_table_native.erase(native_holder);
        }

        accounts_native_total accounts_table_total(get_self(), owner);
        const auto &total_holder = accounts_table_total.get(NTOKEN_TOTAL_BALANCE_KEY, "account doesn't have native token balance");
        eosio_assert(static_cast<uint32_t>(total_holder.amount.amount >= token.amount), "insufficient total native token");

        accounts_table_total.modify(total_holder, 0, [&](auto &tot_holder) {
            tot_holder.amount.amount -= token.amount;
            erase = tot_holder.amount.amount == 0;
        });
        if (erase) {
            accounts_table_total.erase(total_holder);
        }
    }

    void ntoken::setkycrule(uint8_t type, identity::identity_kyc_t kyc) {
        eosio_assert(static_cast<uint32_t>(type < NTOKEN_KYC_RULE_TYPE_MAX), "invalid type");
        require_auth(YOSEMITE_SYSTEM_ACCOUNT);

        kyc_rule_index kyc_rule(get_self(), get_self());
        auto itr = kyc_rule.find(type);

        if (itr == kyc_rule.end()) {
            kyc_rule.emplace(get_self(), [&](auto &holder) {
                holder.type = type;
                holder.kyc_flags = kyc;
            });
        } else {
            kyc_rule.modify(itr, 0, [&](auto &holder) {
                holder.kyc_flags = kyc;
            });
        }
    }
}} // namespace native_token yosemite

EOSIO_ABI(yosemite::native_token::ntoken, (nissue)(nredeem)(transfer)(ntransfer)(payfee)(setkycrule)
)
