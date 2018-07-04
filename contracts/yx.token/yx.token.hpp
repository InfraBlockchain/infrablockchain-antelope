#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/yx_asset.hpp>

#include <string>

namespace yosemite {

    using namespace eosio;
    using std::string;
    using boost::container::flat_map;

    static const uint64_t NATIVE_TOKEN_NAME = S(0, DKRW) >> 8;
    static const uint64_t NATIVE_TOKEN = S(4, DKRW);
    static const account_name FEEDIST_ACCOUNT_NAME = N(yx.feedist);

    class token : public contract {
    public:
        explicit token(account_name self) : contract(self) {
        }

        void regdepo(const yx_symbol &symbol);
        void issue(const account_name &to, const yx_asset &quantity, const string &memo);
        void redeem(const yx_asset &quantity, const string &memo);
        void transfer(account_name from,
                      account_name to,
                      yx_asset quantity,
                      const string &memo,
                      account_name payer);

        void setfee(const name &operation_name, const asset &fee);

        void regdepon(const account_name &issuer);
        void issuen(const account_name &to, const yx_asset &quantity, const string &memo);
        void redeemn(const yx_asset &quantity, const string &memo);
        void transfern(account_name from, account_name to, const yx_asset &quantity,
                       const string &memo, account_name payer);

        inline int64_t get_total_native_token_balance(const account_name &owner) const;
        inline bool is_kyc_depository(const account_name &depository) const;

        void printsupply(const yx_symbol &symbol);
        void printsupplyn(const account_name &depository);
        void printbalance(account_name owner, yx_symbol symbol);
        void clear(const yx_symbol &symbol);
        void clearn();
    private:
        struct fee_holder {
            uint64_t operation;
            asset fee;

            uint64_t primary_key() const { return operation; }
        };

        //TODO: make the specific account as frozen
        struct token_balance {
            int64_t amount{};
            bool frozen = false;
        };

        struct balance_holder {
            account_name owner;
            bool frozen = false;
            flat_map<yx_symbol, token_balance> balance_map{};

            uint64_t primary_key() const { return owner; }
        };

        struct native_balance_holder {
            account_name owner;
            int64_t total_balance = 0;
            flat_map<account_name, token_balance> balance_map{}; // account_name is depository

            uint64_t primary_key() const { return owner; }
        };

        struct token_stats {
            int64_t supply = 0;
            bool frozen = false;
        };

        struct token_stats_holder {
            uint64_t id;
            uint128_t yx_symbol_s;
            token_stats tstats;

            uint64_t primary_key() const { return id; }
            uint128_t by_yx_symbol_s() const { return yx_symbol_s; }
        };

        struct native_token_stats_holder {
            account_name depository;
            token_stats tstats;

            uint64_t primary_key() const { return depository; }
        };

        typedef eosio::multi_index<N(stats), token_stats_holder,
                indexed_by<N(extendedsym), const_mem_fun<token_stats_holder, uint128_t, &token_stats_holder::by_yx_symbol_s> >
        > stats;
        typedef eosio::multi_index<N(accounts), balance_holder> accounts;
        typedef eosio::multi_index<N(nativestats), native_token_stats_holder> stats_native;
        typedef eosio::multi_index<N(accnative), native_balance_holder> accounts_native;
        typedef eosio::multi_index<N(fees), fee_holder> fees;

        void transfer_internal(account_name from, account_name to, yx_asset quantity, bool fee_required, account_name payer);
        void transfern_internal(const account_name &from, const account_name &to, const yx_asset &quantity, bool fee_required,
                                const account_name &payer);

        void add_token_balance(const account_name &owner, const yx_asset &quantity);
        void sub_token_balance(const account_name &owner, const yx_asset &quantity);
        void add_native_token_balance(const account_name &owner, const int64_t &quantity, const account_name &depository);
        void sub_native_token_balance(const account_name &owner, const int64_t &quantity, const account_name &depository);
        int64_t get_supply(yx_symbol symbol) const;
        bool check_fee_operation(const uint64_t &operation_name);
        void charge_fee(account_name payer, uint64_t operation);
        uint128_t yx_symbol_to_uint128(const yx_symbol &symbol) const;

        void transfer_token(const account_name &from, const account_name &to, const yx_asset &quantity);
        void transfer_native_token(const account_name &from, const account_name &to, yx_asset quantity);
    };

    int64_t token::get_total_native_token_balance(const account_name &owner) const {
        accounts_native accounts_table(N(yx.token), NATIVE_TOKEN);
        const auto &balance_holder = accounts_table.get(owner, "account doesn't have native token balance");
        return balance_holder.total_balance;
    }

    bool token::is_kyc_depository(const account_name &depository) const {
        stats_native stats(get_self(), NATIVE_TOKEN);
        const auto &holder = stats.find(depository);
        return holder != stats.end();
    }
}
