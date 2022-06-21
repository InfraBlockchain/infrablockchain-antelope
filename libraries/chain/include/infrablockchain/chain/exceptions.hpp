/**
 *  @file
 *  @copyright defined in infrablockchain/LICENSE
 */
#pragma once
#include <eosio/chain/exceptions.hpp>

namespace infrablockchain { namespace chain {

   using namespace eosio::chain;

// FC_DECLARE_DERIVED_EXCEPTION( transaction_exception,             chain_exception,
//                               3040000, "Transaction exception" )


   FC_DECLARE_DERIVED_EXCEPTION( infrablockchain_standard_token_exception, chain_exception,
                               3711000, "InfraBlockchain standard token exception" )



// FC_DECLARE_DERIVED_EXCEPTION( infrablockchain_transaction_fee_exception, chain_exception,
//                               3712000, "InfraBlockchain transaction fee exception" )


}} // namespace infrablockchain::chain
