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
         const token_id_type token_id = context.get_action().account;
         EOS_ASSERT( token_id == context.get_receiver(), token_action_validate_exception, "settokenmeta built-in standard token action handler should be invoked only in first receiver context" );

         // only the account owner can set token metadata for its own token
         context.require_authorization(token_id);

         context.control.get_mutable_standard_token_manager().set_token_meta_info(context, token_id, settokenmeta_action);

      } FC_CAPTURE_AND_RETHROW( (settokenmeta_action) )
   }

   /**
    * 'issue' standard token action apply handler
    */
   void apply_infrablockchain_built_in_action_issue( apply_context& context ) {

      auto issue_action = context.get_action().data_as_built_in_common_action<issue>();
      try {
         const token_id_type token_id = context.get_action().account;
         EOS_ASSERT( token_id == context.get_receiver(), token_action_validate_exception, "issue built-in standard token action handler should be invoked only in first receiver context" );

         EOS_ASSERT( issue_action.qty.is_valid(), token_action_validate_exception, "invalid quantity" );
         EOS_ASSERT( issue_action.tag.size() <= 256, token_action_validate_exception, "tag has more than 256 bytes" );
         // action parameter validation will be done in context.issue_token()

         const account_name to_account = issue_action.to;
         const share_type issue_amount = issue_action.qty.get_amount();

         auto& standard_token_manager = context.control.get_standard_token_manager();

         auto* token_meta_obj_ptr = standard_token_manager.get_token_meta_object(token_id);
         EOS_ASSERT( token_meta_obj_ptr, token_not_yet_created_exception, "token not yet created for the account ${account}", ("account", action_receiver) );
         auto token_meta_obj = *token_meta_obj_ptr;
         EOS_ASSERT( issue_action.qty.get_symbol() == token_meta_obj.sym, token_symbol_mismatch_exception,
                     "token symbol of quantity field mismatches with the symbol(${sym}) of the registered token metadata",
                     ("sym", token_meta_obj.sym.to_string()) );

         // only the token account owner can issue new token
         context.require_authorization( token_id );

         // update token balance and ram usage
         context.issue_token( to_account, issue_amount );

         // notify 'issue' action to 'to account who could process additional operations for their own sake
         context.require_recipient( to_account );

      } FC_CAPTURE_AND_RETHROW( (issue_action) )
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
