/**
 *  @file
 *  @copyright defined in yosemite-public-blockchain/LICENSE
 */
#pragma once
#include <eosio/chain/symbol.hpp>

namespace yosemite { namespace chain {
    using namespace eosio::chain;

    const static char symbol_issuer_denominator = '@';

    struct yx_symbol {
        symbol       tsymbol;
        account_name issuer;

        yx_symbol() = default;
        yx_symbol(eosio::chain::symbol s, name n) : tsymbol(s), issuer(n) {}

        eosio::chain::uint128_t to_uint128() const {
            eosio::chain::uint128_t result(tsymbol.value());
            result <<= 64;
            return result | issuer.value;
        }

        static yx_symbol from_string(const string& from) {
            try {
                auto at_pos = from.find(symbol_issuer_denominator);
                EOS_ASSERT(at_pos != string::npos, symbol_type_exception, "missing at(@) in yx_symbol");
                auto eosio_symbol_part = from.substr(0, at_pos);
                eosio::chain::symbol sym = symbol::from_string(eosio_symbol_part);

                auto name_part = from.substr(at_pos + 1);
                EOS_ASSERT(!name_part.empty(), symbol_type_exception, "missing issuer in yx_symbol");
                return yx_symbol{sym, string_to_name(name_part.c_str())};
            } FC_CAPTURE_LOG_AND_RETHROW((from))
        }

        explicit operator string() const {
            std::string result = tsymbol.to_string();
            result += symbol_issuer_denominator;
            result += issuer.to_string();
            return result;
        }

        std::string to_string() const {
            return std::string(*this);
        }

        friend bool operator==(const yx_symbol &a, const yx_symbol &b) {
            return std::tie(a.tsymbol, a.issuer) == std::tie(b.tsymbol, b.issuer);
        }

        friend bool operator!=(const yx_symbol &a, const yx_symbol &b) {
            return std::tie(a.tsymbol, a.issuer) != std::tie(b.tsymbol, b.issuer);
        }
    };

}} // namespace yosemite::chain

namespace fc {
   inline void to_variant(const yosemite::chain::yx_symbol& var, fc::variant& vo) { vo = var.to_string(); }
   inline void from_variant(const fc::variant& var, yosemite::chain::yx_symbol& vo) {
      vo = yosemite::chain::yx_symbol::from_string(var.get_string());
   }
}

FC_REFLECT(yosemite::chain::yx_symbol, (tsymbol)(issuer))
