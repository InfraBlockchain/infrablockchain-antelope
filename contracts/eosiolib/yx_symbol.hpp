#ifndef YX_SYMBOL_HPP
#define YX_SYMBOL_HPP

#include <eosiolib/symbol.hpp>

namespace yosemite {

    struct yx_symbol : public eosio::symbol_type {
        explicit yx_symbol(eosio::symbol_name s = 0, account_name c = 0) : symbol_type{s}, issuer{c} {}

        account_name issuer;

        void print() const {
            eosio::symbol_type::print();
            prints("@");
            printn(issuer);
        }

        uint128_t to_uint128() const {
            uint128_t result(value);
            result <<= 64;
            return result | issuer;
        }

        friend bool operator==(const yx_symbol &a, const yx_symbol &b) {
            return std::tie(a.value, a.issuer) == std::tie(b.value, b.issuer);
        }

        friend bool operator!=(const yx_symbol &a, const yx_symbol &b) {
            return std::tie(a.value, a.issuer) != std::tie(b.value, b.issuer);
        }

        EOSLIB_SERIALIZE(yx_symbol, (value)(issuer))
    };

}

#endif //YX_SYMBOL_HPP
