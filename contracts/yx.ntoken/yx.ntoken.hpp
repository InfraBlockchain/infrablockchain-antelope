/**
 *  @copyright defined in LICENSE.txt
 */

#pragma once

#include <eosiolib/eosio.hpp>
#include "../yosemitelib/yx_asset.hpp"

#include <string>
#include "../yosemitelib/yx_fee.hpp"

namespace yosemite {

    using namespace eosio;
    using std::string;
    using boost::container::flat_map;

    static const uint64_t NTOKEN_TOTAL_BALANCE_KEY = N(totalbal);
    static const uint64_t NTOKEN_BASIC_STATS_KEY = N(basicstats);

    class ntoken : public fee_contract {
    public:
        explicit ntoken(account_name self) : fee_contract(self) {
        }

        void nissue(const account_name &to, const yx_asset &asset, const string &memo);
        void nredeem(const yx_asset &asset, const string &memo);
        void transfer(account_name from, account_name to, eosio::asset asset, const string &memo);
        void wptransfer(account_name from, account_name to, eosio::asset asset, account_name payer, const string &memo);
        void ntransfer(account_name from, account_name to, const yx_asset &asset, const string &memo);
        void wpntransfer(account_name from, account_name to, const yx_asset &asset, account_name payer, const string &memo);
        void payfee(account_name payer, asset asset);
        void feetransfer(account_name payer, const yx_asset &asset);

        static inline int64_t get_total_native_token_balance(const account_name &owner);

    protected:
        bool check_fee_operation(const uint64_t &operation_name) override;
        void charge_fee(const account_name &payer, uint64_t operation) override;

    private:
        /* scope = owner */
        struct total_balance {
            int64_t amount = 0;

            uint64_t primary_key() const { return NTOKEN_TOTAL_BALANCE_KEY; }
        };

        /* scope = owner */
        struct native_balance_holder {
            account_name depository{};
            int64_t amount = 0;

            uint64_t primary_key() const { return depository; }
        };

        /* native token stats per depository, scope = depository */
        struct native_token_stats {
            uint64_t key;
            int64_t supply = 0;
            uint8_t options = 0; // frozen, ...

            uint64_t primary_key() const { return key; }
        };

        typedef eosio::multi_index<N(ntstats), native_token_stats> stats_native;
        typedef eosio::multi_index<N(ntaccounts), native_balance_holder> accounts_native;
        typedef eosio::multi_index<N(ntaccountstt), total_balance> accounts_native_total;

        void add_native_token_balance(const account_name &owner, const yx_asset &asset);
        void sub_native_token_balance(const account_name &owner, const yx_asset &asset);

        bool is_auth_enough_for_transfer(uint32_t kycvector);
    };

    int64_t ntoken::get_total_native_token_balance(const account_name &owner) {
        accounts_native_total accounts_total_table(N(yx.ntoken), owner);
        const auto &balance_holder = accounts_total_table.get(NTOKEN_TOTAL_BALANCE_KEY, "account doesn't have native token balance");
        return balance_holder.amount;
    }
}
