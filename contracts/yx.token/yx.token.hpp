#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/yx_asset.hpp>
#include <eosiolib/yx_fee.hpp>

#include <string>

namespace yosemite {

    using namespace eosio;
    using std::string;
    using boost::container::flat_map;

    class token : public fee_contract {
    public:
        explicit token(account_name self) : fee_contract(self) {
        }

        void regdepo(const yx_symbol &symbol);
        void issue(const account_name &to, const yx_asset &quantity, const string &memo);
        void redeem(const yx_asset &quantity, const string &memo);
        void transfer(account_name from, account_name to, yx_asset quantity, account_name payer, const string &memo);

        void printsupply(const yx_symbol &symbol);
        void printbalance(account_name owner, yx_symbol symbol);
        void clear(const yx_symbol &symbol);

    protected:
        bool check_fee_operation(const uint64_t &operation_name) override;
        void charge_fee(const account_name &payer, uint64_t operation) override;

    private:
        //TODO: make the specific account as frozen
        struct token_balance {
            int64_t amount{};
            bool frozen = false;
        };

        struct balance_holder {
            account_name owner{};
            bool frozen = false;
            flat_map<yx_symbol, token_balance> balance_map{};

            uint64_t primary_key() const { return owner; }
        };

        struct token_stats {
            int64_t supply = 0;
            bool frozen = false;
        };

        struct token_stats_holder {
            uint64_t id{};
            uint128_t yx_symbol_s{};
            token_stats tstats;

            uint64_t primary_key() const { return id; }
            uint128_t by_yx_symbol_s() const { return yx_symbol_s; }
        };

        typedef eosio::multi_index<N(stats), token_stats_holder,
                indexed_by<N(extendedsym), const_mem_fun<token_stats_holder, uint128_t, &token_stats_holder::by_yx_symbol_s> >
        > stats;
        typedef eosio::multi_index<N(accounts), balance_holder> accounts;

        void transfer_internal(account_name from, account_name to, yx_asset quantity, bool fee_required, account_name payer);

        void add_token_balance(const account_name &owner, const yx_asset &quantity);
        void sub_token_balance(const account_name &owner, const yx_asset &quantity);
        int64_t get_supply(yx_symbol symbol) const;
    };
}
