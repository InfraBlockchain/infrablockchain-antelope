#pragma once

#include <eosiolib/eosio.hpp>
#include <yosemitelib/yx_asset.hpp>
#include <yosemitelib/yx_fee.hpp>

#include <string>

namespace yosemite {

    using namespace eosio;
    using std::string;
    using boost::container::flat_map;

    class token : public fee_contract {
    public:
        explicit token(account_name self) : fee_contract(self) {
        }

        void create(const yx_symbol &symbol, uint32_t kycvector);
        void issue(const account_name &to, const yx_asset &quantity, const string &memo);
        void redeem(const yx_asset &quantity, const string &memo);
        void transfer(account_name from, account_name to, yx_asset quantity, const string &memo);
        void wptransfer(account_name from, account_name to, yx_asset quantity, account_name payer, const string &memo);

    protected:
        bool check_fee_operation(const uint64_t &operation_name) override;
        void charge_fee(const account_name &payer, uint64_t operation) override;

    private:
        /* scope = owner */
        struct balance_holder {
            uint64_t id = 0;
            uint128_t yx_symbol_s{}; // yx_symbol which is serialized to 128 bit;
                                     // eosio::symbol_type::value is higher 64 bit and issuer account is lower 64 bit.
            int64_t amount = 0;
            uint8_t options = 0;

            uint64_t primary_key() const { return id; }
            uint128_t by_yx_symbol_s() const { return yx_symbol_s; }
        };

        /* scope = token symbol */
        struct token_stats {
            uint64_t issuer = 0;
            int64_t supply = 0;
            uint32_t required_kycvector = 0;
            uint8_t options = 0;

            uint64_t primary_key() const { return issuer; }
        };

        typedef eosio::multi_index<N(tstats), token_stats> stats;
        typedef eosio::multi_index<N(taccounts), balance_holder,
                indexed_by<N(yxsymbol), const_mem_fun<balance_holder, uint128_t, &balance_holder::by_yx_symbol_s> >
        > accounts;

        void add_token_balance(const account_name &owner, const yx_asset &quantity);
        void sub_token_balance(const account_name &owner, const yx_asset &quantity);
    };
}
