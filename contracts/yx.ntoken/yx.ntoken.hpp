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
        void setkycrule(uint8_t type, uint16_t kyc);

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
        typedef eosio::multi_index<N(ntaccounts), native_balance_holder> accounts_native;
        typedef eosio::multi_index<N(kycrule), kyc_rule> kyc_rule_index;

        void add_native_token_balance(const account_name &owner, const yx_asset &asset);
        void sub_native_token_balance(const account_name &owner, const yx_asset &asset);

        bool check_identity_auth_for_transfer(account_name account, const ntoken_kyc_rule_type &kycrule_type);
    };

}
