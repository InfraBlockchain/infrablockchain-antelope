/**
 *  @file chain/infrablockchain/standard_token_action_handlers.cpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */

#include <eosio/chain/controller.hpp>
#include <eosio/chain/transaction_context.hpp>
#include <eosio/chain/apply_context.hpp>
#include <eosio/chain/transaction.hpp>
#include <eosio/chain/account_object.hpp>

#include <infrablockchain/chain/system_accounts.hpp>
#include <infrablockchain/chain/standard_token_action_handlers.hpp>
#include <infrablockchain/chain/standard_token_action_types.hpp>
#include <infrablockchain/chain/standard_token_manager.hpp>
#include <infrablockchain/chain/exceptions.hpp>

namespace infrablockchain { namespace chain {

   using namespace eosio::chain;
   using namespace infrablockchain::chain::standard_token;

   /**
    * 'settokenmeta' standard token action apply handler
    */
   void apply_infrablockchain_built_in_action_settokenmeta( apply_context& context ) {

      auto settokenmeta_action = context.get_action().data_as_built_in_common_action<settokenmeta>();
      try {
         token_id_type token_id = context.get_action().account;
         EOS_ASSERT( token_id == context.get_receiver(), infrablockchain_standard_token_exception, "settokenmeta built-in action handler should be invoked only in first receiver context" );

         // only the account owner can set token metadata for its own token
         context.require_authorization(token_id);

         context.control.get_mutable_standard_token_manager().set_token_meta_info(context, token_id, settokenmeta_action);

      } FC_CAPTURE_AND_RETHROW( (settokenmeta_action) )
   }

   /**
    * 'issue' standard token action apply handler
    */
   void apply_infrablockchain_built_in_action_issue( apply_context& context ) {

   }

   /**
    * 'transfer' standard token action apply handler
    */
   void apply_infrablockchain_built_in_action_transfer( apply_context& context ) {

   }


   /**
    * 'txfee' standard token action apply handler
    */
   void apply_infrablockchain_built_in_action_txfee( apply_context& context ) {

   }


   /**
    * 'redeem' standard token action apply handler
    */
   void apply_infrablockchain_built_in_action_redeem( apply_context& context ) {

   }

} } // namespace infrablockchain::chain
