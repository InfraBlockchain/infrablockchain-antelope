#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/yx_asset.hpp>

#include <string>
#include <eosiolib/yx_fee.hpp>

namespace yosemite {

    using namespace eosio;
    using std::string;
    using boost::container::flat_map;

    class ntoken : public fee_contract {
    public:
        explicit ntoken(account_name self) : fee_contract(self) {
        }

        void regdepon(const account_name &issuer);
        void issuen(const account_name &to, const yx_asset &quantity, const string &memo);
        void redeemn(const yx_asset &quantity, const string &memo);
        void transfer(account_name from, account_name to, yx_asset quantity, account_name payer, const string &memo);
        void transfern(account_name from, account_name to, const yx_asset &quantity, account_name payer,
                       const string &memo);

        inline int64_t get_total_native_token_balance(const account_name &owner) const;
        inline bool is_kyc_depository(const account_name &depository) const;

        void printsupplyn(const account_name &depository);
        void printbalance(account_name owner, yx_symbol symbol);
        void clearn();

    protected:
        bool check_fee_operation(const uint64_t &operation_name) override;
        void charge_fee(const account_name &payer, uint64_t operation) override;

    private:
        struct token_balance {
            int64_t amount{};
            bool frozen = false;
        };

        struct native_balance_holder {
            account_name owner{};
            int64_t total_balance = 0;
            flat_map<account_name, token_balance> balance_map{}; // account_name is depository

            uint64_t primary_key() const { return owner; }
        };

        struct token_stats {
            int64_t supply = 0;
            bool frozen = false;
        };

        struct native_token_stats_holder {
            account_name depository{};
            token_stats tstats;

            uint64_t primary_key() const { return depository; }
        };

        typedef eosio::multi_index<N(nativestats), native_token_stats_holder> stats_native;
        typedef eosio::multi_index<N(accnative), native_balance_holder> accounts_native;

        void transfer_internal(account_name from, account_name to, yx_asset quantity, bool fee_required, account_name payer);
        void transfern_internal(const account_name &from, const account_name &to, const yx_asset &quantity, bool fee_required,
                                const account_name &payer);

        void add_native_token_balance(const account_name &owner, const int64_t &quantity, const account_name &depository);
        void sub_native_token_balance(const account_name &owner, const int64_t &quantity, const account_name &depository);

        void transfer_native_token(const account_name &from, const account_name &to, yx_asset quantity);
    };

    int64_t ntoken::get_total_native_token_balance(const account_name &owner) const {
        accounts_native accounts_table(N(yx.token), NATIVE_TOKEN);
        const auto &balance_holder = accounts_table.get(owner, "account doesn't have native token balance");
        return balance_holder.total_balance;
    }

    bool ntoken::is_kyc_depository(const account_name &depository) const {
        stats_native stats(get_self(), NATIVE_TOKEN);
        const auto &holder = stats.find(depository);
        return holder != stats.end();
    }
}
