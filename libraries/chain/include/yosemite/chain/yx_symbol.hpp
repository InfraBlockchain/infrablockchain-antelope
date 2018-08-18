/**
 *  @file
 *  @copyright defined in yosemite/LICENSE
 */
#pragma once
#include <eosio/chain/symbol.hpp>

namespace yosemite { namespace chain {
    using namespace eosio::chain;

    struct yx_symbol {
        symbol       symbol;
        account_name issuer;
    };

}} // namespace yosemite::chain

FC_REFLECT(yosemite::chain::yx_symbol, (symbol)(issuer))
