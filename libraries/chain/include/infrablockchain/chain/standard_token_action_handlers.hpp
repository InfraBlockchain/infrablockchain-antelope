/**
 *  @file infrablockchain/chain/standard_token_action_handlers.hpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */
#pragma once

#include <eosio/chain/types.hpp>
#include <eosio/chain/contract_types.hpp>
#include <eosio/chain/apply_context.hpp>
#include <infrablockchain/chain/standard_token_object.hpp>

namespace infrablockchain { namespace chain {

   using namespace eosio::chain;

   /**
    * @defgroup native_standard_token_action_handlers InfraBlockchain Built-in Standard Token Action Handlers
    */
   ///@{

   void apply_infrablockchain_built_in_action_settokenmeta( apply_context& context );

   void apply_infrablockchain_built_in_action_issue( apply_context& context );

   void apply_infrablockchain_built_in_action_transfer( apply_context& context );

   void apply_infrablockchain_built_in_action_txfee( apply_context& context );

   void apply_infrablockchain_built_in_action_retire( apply_context& context );


      ///@}  end action handlers

} } /// namespace infrablockchain::chain
