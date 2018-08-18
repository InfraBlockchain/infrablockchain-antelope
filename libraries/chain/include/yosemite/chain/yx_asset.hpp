/**
 *  @file
 *  @copyright defined in yosemite/LICENSE
 */
#pragma once
#include <eosio/chain/asset.hpp>

namespace yosemite { namespace chain {
    using namespace eosio::chain;

    struct yx_asset {
        yx_asset() = default;
        yx_asset(asset a, name n) : amount(a), issuer(n) {}

        asset amount;
        name issuer;
    };

}} // namespace yosemite::chain

FC_REFLECT(yosemite::chain::yx_asset, (amount)(issuer))
