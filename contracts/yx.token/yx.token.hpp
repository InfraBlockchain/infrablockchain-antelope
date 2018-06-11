#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>

#include <string>

namespace yosemitex {

    using namespace eosio;
    using std::string;
    using boost::container::flat_map;
    using boost::container::flat_set;

    static constexpr uint64_t NATIVE_COIN = S(4,DKRW);

    class token : public contract {
    public:
        explicit token(account_name self) : contract(self) {
            operations.insert(N(create));
            operations.insert(N(issue));
            operations.insert(N(redeem));
            operations.insert(N(transfer));
        }

        void create(extended_symbol symbol,
                    uint8_t issuer_can_freeze,
                    uint8_t issuer_can_recall,
                    uint8_t issuer_can_whitelist);

        void issue(account_name to, extended_asset quantity, string memo);

        void redeem(account_name from, extended_asset quantity, string memo);

        void transfer(account_name from,
                      account_name to,
                      asset quantity,
                      string memo);

        /* multisig */
        void setfee(name operation_name, asset fee);

        void printsupply(extended_symbol symbol);
        void printbalance(account_name owner, extended_symbol symbol);
        void rmall(account_name owner, extended_symbol symbol);
    private:
        flat_set<uint64_t> operations{};

        //TODO: make the specific account as frozen
        struct account_balance {
            int64_t amount{};
            bool whitelist = true;
            bool frozen = false;
        };

        struct account_balance_holder {
            asset total_balance;
            flat_map<account_name, account_balance> balance_map{}; // account_name is issuer

            uint64_t primary_key() const { return total_balance.symbol.name(); }
        };

        struct token_stats {
            asset supply;
            account_name issuer{};
            bool can_freeze = true;
            bool can_recall = true;
            bool can_whitelist = true;
            bool is_frozen = false;
            bool enforce_whitelist = false;
        };

        struct token_stats_holder {
            symbol_name symbol;
            flat_map<account_name, token_stats> stats_map; // account_name is issuer

            uint64_t primary_key() const { return symbol; }
        };

        struct fee_holder {
            uint64_t operation;
            asset fee;

            uint64_t primary_key() const { return operation; }
        };

        typedef eosio::multi_index<N(accounts), account_balance_holder> accounts;
        typedef eosio::multi_index<N(stats), token_stats_holder> stats;
        typedef eosio::multi_index<N(fees), fee_holder> fees;

        void add_balance(account_name owner, extended_asset value);
        asset get_supply(extended_symbol symbol) const;
        asset get_total_balance(account_name owner, extended_symbol symbol) const;
        void pay_fee(account_name payer, uint64_t operation);
    };
}
