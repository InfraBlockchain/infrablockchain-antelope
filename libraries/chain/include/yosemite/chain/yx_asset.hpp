/**
 *  @file
 *  @copyright defined in yosemite/LICENSE
 */
#pragma once

#include <eosio/chain/asset.hpp>
#include <yosemite/chain/yx_symbol.hpp>

namespace yosemite { namespace chain {

   using namespace eosio::chain;

   struct yx_asset {
      asset amount;
      name issuer;

      yx_asset() = default;

      yx_asset(asset a, name n) : amount(a), issuer(n) {}

      yx_symbol get_yx_symbol() const {
         return yx_symbol{amount.get_symbol(), issuer};
      }

      static yx_asset from_string(const string &from) {
         try {
            auto at_pos = from.find(symbol_issuer_denominator);
            EOS_ASSERT(at_pos != string::npos, asset_type_exception, "missing at(@) in yx_asset");
            auto eosio_asset_part = from.substr(0, at_pos);
            eosio::chain::asset as = asset::from_string(eosio_asset_part);

            auto name_part = from.substr(at_pos + 1);
            EOS_ASSERT(!name_part.empty(), asset_type_exception, "missing issuer in yx_asset");
            return yx_asset{as, string_to_name(name_part.c_str())};
         } FC_CAPTURE_LOG_AND_RETHROW((from))
      }

      explicit operator string() const {
         std::string result = amount.to_string();
         result += symbol_issuer_denominator;
         result += issuer.to_string();
         return result;
      }

      std::string to_string() const {
         return std::string(*this);
      }
   };

 }} // namespace yosemite::chain

FC_REFLECT(yosemite::chain::yx_asset, (amount)(issuer))
