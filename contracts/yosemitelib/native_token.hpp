#ifndef YX_NATIVE_TOKEN_HPP
#define YX_NATIVE_TOKEN_HPP

#include <yosemitelib/native_token_symbol.hpp>
#include <eosiolib/multi_index.hpp>
#include <yosemitelib/system_accounts.hpp>

namespace yosemite {

    static const uint64_t NTOKEN_TOTAL_BALANCE_KEY = N(totalbal);

    /* scope = owner */
    struct total_balance {
        int64_t amount = 0;

        uint64_t primary_key() const { return NTOKEN_TOTAL_BALANCE_KEY; }
    };

    typedef eosio::multi_index<N(ntaccountstt), total_balance> accounts_native_total;

    int64_t get_total_native_token_balance(const account_name &owner) {
        accounts_native_total accounts_total_table(YOSEMITE_NATIVE_TOKEN_ACCOUNT, owner);
        const auto &balance_holder = accounts_total_table.get(NTOKEN_TOTAL_BALANCE_KEY, "account doesn't have native token balance");
        return balance_holder.amount;
    }
}

#endif // YX_NATIVE_TOKEN_HPP
