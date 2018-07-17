#ifndef YX_ASSET_HPP
#define YX_ASSET_HPP

#include <eosiolib/asset.hpp>
//#include <eosiolib/yx_symbol.hpp>
#include "yx_symbol.hpp"

namespace yosemite {

    struct yx_asset : public eosio::asset {
        account_name issuer;

        yx_symbol get_yx_symbol() const { return yx_symbol(symbol, issuer); }

        yx_asset() = default;

        yx_asset(int64_t v, yx_symbol s) : eosio::asset(v, s), issuer(s.issuer) {}

        yx_asset(eosio::asset a, account_name c) : eosio::asset(a), issuer(c) {}

        void print() const {
            eosio::asset::print();
            prints("@");
            printn(issuer);
        }

        yx_asset operator-() const {
            eosio::asset r = this->asset::operator-();
            return {r, issuer};
        }

        friend yx_asset operator-(const yx_asset &a, const yx_asset &b) {
            eosio_assert(a.issuer == b.issuer, "type mismatch");
            eosio::asset r = static_cast<const eosio::asset &>(a) - static_cast<const eosio::asset &>(b);
            return {r, a.issuer};
        }

        friend yx_asset operator+(const yx_asset &a, const yx_asset &b) {
            eosio_assert(a.issuer == b.issuer, "type mismatch");
            eosio::asset r = static_cast<const eosio::asset &>(a) + static_cast<const eosio::asset &>(b);
            return {r, a.issuer};
        }

        EOSLIB_SERIALIZE(yx_asset, (amount)(symbol)(issuer))
    };

}

#endif //YX_ASSET_HPP
