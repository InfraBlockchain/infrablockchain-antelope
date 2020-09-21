/**
 *  @file
 *  @copyright defined in infrablockchain/LICENSE
 */
#pragma once
#include <eosio/chain/types.hpp>
#include <fc/reflect/reflect.hpp>

namespace infrablockchain { namespace chain {
    struct dcid {
        eosio::chain::account_name creator;
        uint64_t sequence;
    };

}} // namespace infrablockchain::chain

FC_REFLECT(infrablockchain::chain::dcid, (creator)(sequence))
