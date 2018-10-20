#ifndef YX_SYMBOL_HPP
#define YX_SYMBOL_HPP

#include <eosiolib/symbol.hpp>
#include <yosemitelib/native_token_symbol.hpp>

namespace yosemite {

    const static char symbol_issuer_denominator = '@';

    struct yx_symbol : public eosio::symbol_type {
        account_name issuer;

        explicit yx_symbol(eosio::symbol_name s = 0, account_name c = 0) : symbol_type{s}, issuer{c} {}

        bool is_native(bool assert = true) const {
            if (assert) {
                assert_valid_native_symbol();
            }
            return value == YOSEMITE_NATIVE_TOKEN_SYMBOL;
        }

        void assert_valid_native_symbol() const {
            if (name() == YOSEMITE_NATIVE_TOKEN_SYMBOL_NAME_VALUE) {
                eosio_assert(static_cast<uint32_t>(precision() == YOSEMITE_NATIVE_TOKEN_PRECISION), "invalid native token");
            }
        }

        bool is_valid(bool assert = true) const {
            if (assert) {
                assert_valid_native_symbol();
            }
            return eosio::symbol_type::is_valid();
        }

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

        explicit operator std::string() const {
            std::string result = symbol_type::to_string();
            result += symbol_issuer_denominator;
            result += eosio::name{issuer}.to_string();
            return result;
        }

       std::string to_string() const {
          return std::string(*this);
       }

        friend bool operator==(const yx_symbol &a, const yx_symbol &b) {
            return std::tie(a.value, a.issuer) == std::tie(b.value, b.issuer);
        }

        friend bool operator!=(const yx_symbol &a, const yx_symbol &b) {
            return std::tie(a.value, a.issuer) != std::tie(b.value, b.issuer);
        }

        friend bool operator<(const yx_symbol& a, const yx_symbol& b) {
            return std::tie(a.value, a.issuer) < std::tie(b.value, b.issuer);
        }

        friend bool operator<=(const yx_symbol& a, const yx_symbol& b) {
            return std::tie(a.value, a.issuer) <= std::tie(b.value, b.issuer);
        }

        friend bool operator>(const yx_symbol& a, const yx_symbol& b) {
            return std::tie(a.value, a.issuer) > std::tie(b.value, b.issuer);
        }

        friend bool operator>=(const yx_symbol& a, const yx_symbol& b) {
            return std::tie(a.value, a.issuer) >= std::tie(b.value, b.issuer);
        }

        EOSLIB_SERIALIZE(yx_symbol, (value)(issuer))
    };

}

#endif //YX_SYMBOL_HPP
