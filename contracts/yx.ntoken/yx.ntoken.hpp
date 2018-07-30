/**
 *  @copyright defined in LICENSE.txt
 */

#pragma once

#include <eosiolib/eosio.hpp>
#include <yosemitelib/yx_asset.hpp>
#include <yosemitelib/system_accounts.hpp>
#include <yosemitelib/native_token.hpp>
#include <string>

namespace yosemite {

    using namespace eosio;
    using std::string;
    using boost::container::flat_map;

    static const uint64_t NTOKEN_BASIC_STATS_KEY = N(basicstats);

    class ntoken : public contract {
    public:
        explicit ntoken(account_name self) : contract(self) {
        }

        void nissue(const account_name &to, const yx_asset &asset, const string &memo);
        void nredeem(const yx_asset &asset, const string &memo);
        void transfer(account_name from, account_name to, eosio::asset asset, const string &memo);
        void wptransfer(account_name from, account_name to, eosio::asset asset, account_name payer, const string &memo);
        void ntransfer(account_name from, account_name to, const yx_asset &asset, const string &memo);
        void wpntransfer(account_name from, account_name to, const yx_asset &asset, account_name payer, const string &memo);
        void payfee(account_name payer, asset asset);
        void feetransfer(account_name payer, const yx_asset &asset);

    private:
        asset charge_fee(const account_name &payer, uint64_t operation);

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

        void add_native_token_balance(const account_name &owner, const yx_asset &asset);
        void sub_native_token_balance(const account_name &owner, const yx_asset &asset);

        bool is_auth_enough_for_transfer(uint32_t kycvector);
    };

}
