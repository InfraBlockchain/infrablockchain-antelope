#ifndef YX_NATIVE_TOKEN_HPP
#define YX_NATIVE_TOKEN_HPP

#include <eosiolib/multi_index.hpp>
#include <yosemitelib/native_token_symbol.hpp>
#include <yosemitelib/system_accounts.hpp>
#include <yosemitelib/transaction_fee.hpp>
#include <yosemitelib/yx_asset.hpp>

/* Some constants must be sync with plugins/chain_plugin/chain_plugin.cpp::get_account(). */

namespace yosemite { namespace native_token {
    static const uint64_t NTOKEN_TOTAL_BALANCE_KEY = N(totalbal);

    using namespace eosio;
    using std::string;

    static const uint64_t NTOKEN_BASIC_STATS_KEY = N(basicstats);

    class ntoken : public contract {
    public:
        explicit ntoken(account_name self) : contract(self) {
        }

        void nissue(const account_name &to, const yx_asset &token, const string &memo);
        void nredeem(const yx_asset &token, const string &memo);
        void transfer(account_name from, account_name to, eosio::asset amount, const string &memo);
        void wptransfer(account_name from, account_name to, eosio::asset amount, account_name payer, const string &memo);
        void ntransfer(account_name from, account_name to, const yx_asset &token, const string &memo);
        void wpntransfer(account_name from, account_name to, const yx_asset &token, account_name payer, const string &memo);
        void payfee(account_name payer, yx_asset token);
        void setkycrule(uint8_t type, uint16_t kyc);

    private:

        /* native token stats per depository, scope = depository */
        struct native_token_stats {
            uint64_t key = 0;
            int64_t supply = 0;
            uint8_t options = 0; // frozen, ...

            uint64_t primary_key() const { return key; }
        };

        /* KYC rule for native token, scope = get_self() */
        enum ntoken_kyc_rule_type {
            NTOKEN_KYC_RULE_TYPE_TRANSFER_SEND    = 0,
            NTOKEN_KYC_RULE_TYPE_TRANSFER_RECEIVE = 1,

            NTOKEN_KYC_RULE_TYPE_MAX // MUST NOT EXCEED MORE THAN 255
        };

        struct kyc_rule {
            uint8_t type = 0; // == ntoken_kyc_rule_type
            uint16_t kyc_flags = 0; // from yosemitelib/identity.hpp

            uint64_t primary_key() const { return type; }
        };

        typedef eosio::multi_index<N(ntstats), native_token_stats> stats_native;
        typedef eosio::multi_index<N(kycrule), kyc_rule> kyc_rule_index;

        void add_native_token_balance(const account_name &owner, const yx_asset &token);
        void sub_native_token_balance(const account_name &owner, const yx_asset &token);

        bool check_identity_auth_for_transfer(account_name account, const ntoken_kyc_rule_type &kycrule_type);
    };

    /* scope = owner */
    struct native_balance_holder {
        account_name depository{};
        int64_t amount = 0;

        uint64_t primary_key() const { return depository; }
    };

    /* scope = owner */
    struct total_balance {
        int64_t amount = 0;

        uint64_t primary_key() const { return NTOKEN_TOTAL_BALANCE_KEY; }
    };

    typedef eosio::multi_index<N(ntaccounts), native_balance_holder> accounts_native;
    typedef eosio::multi_index<N(ntaccountstt), total_balance> accounts_native_total;

    int64_t get_total_native_token_balance(const account_name &owner) {
        accounts_native_total accounts_total_table(YOSEMITE_NATIVE_TOKEN_ACCOUNT, owner);
        const auto &balance_holder = accounts_total_table.get(NTOKEN_TOTAL_BALANCE_KEY, "account doesn't have native token balance");
        return balance_holder.amount;
    }

    void charge_transaction_fee(account_name payer, uint64_t operation,
                                const std::vector<account_name> &zeroedout_depos = {},
                                const yx_asset &remained_ntoken = {}) {
        auto tx_fee = yosemite::get_transaction_fee(operation);
        if (tx_fee.amount > 0) {
            accounts_native accounts_table_native(YOSEMITE_NATIVE_TOKEN_ACCOUNT, payer);
            for (auto &balance_holder : accounts_table_native) {
                if (std::find(zeroedout_depos.begin(), zeroedout_depos.end(), balance_holder.depository) != zeroedout_depos.end()) {
                    continue;
                }

                eosio::asset to_send{0, YOSEMITE_NATIVE_TOKEN_SYMBOL};
                if (balance_holder.depository == remained_ntoken.issuer) {
                    if (remained_ntoken.amount >= tx_fee.amount) {
                        to_send.amount = tx_fee.amount;
                        tx_fee.amount = 0;
                    } else {
                        to_send.amount = remained_ntoken.amount;
                        tx_fee.amount -= remained_ntoken.amount;
                    }
                } else {
                    if (balance_holder.amount >= tx_fee.amount) {
                        to_send.amount = tx_fee.amount;
                        tx_fee.amount = 0;
                    } else {
                        to_send.amount = balance_holder.amount;
                        tx_fee.amount -= balance_holder.amount;
                    }
                }

                INLINE_ACTION_SENDER(ntoken, payfee)
                        (YOSEMITE_NATIVE_TOKEN_ACCOUNT, {{payer, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                         {payer, yx_asset(to_send, balance_holder.depository)});

                if (tx_fee.amount == 0) {
                    break;
                }
            }

            eosio_assert(static_cast<uint32_t>(tx_fee.amount == 0), "payer account cannot afford transaction fee");
        }
    }
}}

#endif // YX_NATIVE_TOKEN_HPP
