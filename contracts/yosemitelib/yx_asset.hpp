#ifndef YX_ASSET_HPP
#define YX_ASSET_HPP

#include <eosiolib/asset.hpp>
#include <yosemitelib/yx_symbol.hpp>

namespace yosemite {

    struct yx_asset : public eosio::asset {
        account_name issuer;

        yx_asset() = default;

        yx_asset(int64_t v, yx_symbol s) : eosio::asset(v, s), issuer(s.issuer) {}

        yx_asset(eosio::asset a, account_name c) : eosio::asset(a), issuer(c) {}

        yx_symbol get_yx_symbol() const { return yx_symbol(symbol, issuer); }

        bool is_native(bool assert = true) const {
            if (assert) {
                assert_valid_native_asset();
            }
            return symbol.value == YOSEMITE_NATIVE_TOKEN_SYMBOL;
        }

        void assert_valid_native_asset() const {
            if (symbol.name() == YOSEMITE_NATIVE_TOKEN_SYMBOL_NAME_VALUE) {
                eosio_assert(static_cast<uint32_t>(symbol.precision() == YOSEMITE_NATIVE_TOKEN_PRECISION), "invalid native token");
            }
        }

        bool is_valid(bool assert = true) const {
            if (assert) {
                assert_valid_native_asset();
            }
            return eosio::asset::is_valid();
        }

        void print() const {
            asset::print();
            prints("@");
            printn(issuer);
        }

        yx_asset operator-() const {
            eosio::asset r = asset::operator-();
            return {r, issuer};
        }

        yx_asset& operator-=(const yx_asset& a) {
            eosio_assert(issuer == a.issuer, "issuer mismatch");
            asset::operator-=(a);
            return *this;
        }

        yx_asset& operator+=(const yx_asset& a) {
            eosio_assert(issuer == a.issuer, "issuer mismatch");
            asset::operator+=(a);
            return *this;
        }

        yx_asset& operator*=(int64_t a) {
            asset::operator*=(a);
            return *this;
        }

        friend yx_asset operator*(const yx_asset &a, int64_t b) {
            yx_asset r = a;
            r *= b;
            return r;
        }

        friend yx_asset operator*(int64_t a, const yx_asset &b) {
            yx_asset r = b;
            r *= a;
            return r;
        }

        yx_asset& operator/=(int64_t a) {
            amount /= a;
            return *this;
        }

        friend yx_asset operator/(const yx_asset &a, int64_t b) {
            yx_asset result = a;
            result /= b;
            return result;
        }

        friend yx_asset operator/(int64_t a, const yx_asset &b) {
            yx_asset result = b;
            result /= a;
            return result;
        }

        friend yx_asset operator-(const yx_asset &a, const yx_asset &b) {
            eosio_assert(a.issuer == b.issuer, "issuer mismatch");
            eosio::asset r = static_cast<const eosio::asset &>(a) - static_cast<const eosio::asset &>(b);
            return {r, a.issuer};
        }

        friend yx_asset operator+(const yx_asset &a, const yx_asset &b) {
            eosio_assert(a.issuer == b.issuer, "issuer mismatch");
            eosio::asset r = static_cast<const eosio::asset &>(a) + static_cast<const eosio::asset &>(b);
            return {r, a.issuer};
        }

        friend int64_t operator*(const yx_asset& a, const yx_asset& b) {
            eosio_assert(a.issuer == b.issuer, "issuer mismatch");
            eosio_assert(a.symbol == b.symbol, "comparison of assets with different symbols is not allowed");
            return a.amount * b.amount;
        }

        friend int64_t operator/(const yx_asset& a, const yx_asset& b) {
            eosio_assert(a.issuer == b.issuer, "issuer mismatch");
            eosio_assert(a.symbol == b.symbol, "comparison of assets with different symbols is not allowed");
            return a.amount / b.amount;
        }

        friend bool operator==(const yx_asset& a, const yx_asset& b) {
            eosio_assert(a.issuer == b.issuer, "issuer mismatch");
            eosio_assert(a.symbol == b.symbol, "comparison of assets with different symbols is not allowed");
            return a.amount == b.amount;
        }

        friend bool operator!=(const yx_asset& a, const yx_asset& b) {
            return !(a == b);
        }

        friend bool operator<(const yx_asset& a, const yx_asset& b) {
            eosio_assert(a.issuer == b.issuer, "issuer mismatch");
            eosio_assert(a.symbol == b.symbol, "comparison of assets with different symbols is not allowed");
            return a.amount < b.amount;
        }

        friend bool operator<=(const yx_asset& a, const yx_asset& b) {
            eosio_assert(a.issuer == b.issuer, "issuer mismatch");
            eosio_assert(a.symbol == b.symbol, "comparison of assets with different symbols is not allowed");
            return a.amount <= b.amount;
        }

        friend bool operator>(const yx_asset& a, const yx_asset& b) {
            eosio_assert(a.issuer == b.issuer, "issuer mismatch");
            eosio_assert(a.symbol == b.symbol, "comparison of assets with different symbols is not allowed");
            return a.amount > b.amount;
        }

        friend bool operator>=(const yx_asset& a, const yx_asset& b) {
            eosio_assert(a.issuer == b.issuer, "issuer mismatch");
            eosio_assert(a.symbol == b.symbol, "comparison of assets with different symbols is not allowed");
            return a.amount >= b.amount;
        }

        EOSLIB_SERIALIZE(yx_asset, (amount)(symbol)(issuer))
    };

}

#endif //YX_ASSET_HPP
