/**
 *  @file
 *  @copyright defined in yosemite/LICENSE
 */
#pragma once
#include <eosio/chain/types.hpp>
#include <fc/reflect/reflect.hpp>

namespace yosemite { namespace chain {
    struct dcid {
        eosio::chain::account_name creator;
        uint64_t sequence;
    };

}} // namespace yosemite::chain

FC_REFLECT(yosemite::chain::dcid, (creator)(sequence))
