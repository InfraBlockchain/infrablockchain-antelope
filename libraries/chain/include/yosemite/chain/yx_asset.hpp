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
    };

}} // namespace yosemite::chain

FC_REFLECT(yosemite::chain::yx_asset, (amount)(issuer))
