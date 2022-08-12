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

         EOS_ASSERT( issue_action.quantity.is_valid(), token_action_validate_exception, "invalid quantity" );
         EOS_ASSERT( issue_action.memo.size() <= 256, token_action_validate_exception, "memo has more than 256 bytes" );
         // action parameter validation will be done in context.issue_token()

         const account_name to_account = issue_action.to;
         const share_type issue_amount = issue_action.quantity.get_amount();

         auto& standard_token_manager = context.control.get_standard_token_manager();

         auto* token_meta_obj_ptr = standard_token_manager.get_token_meta_object(token_id);
         EOS_ASSERT( token_meta_obj_ptr, token_not_yet_created_exception, "token not yet created for the account ${account}", ("account", token_id) );
         auto token_meta_obj = *token_meta_obj_ptr;
         EOS_ASSERT( issue_action.quantity.get_symbol() == token_meta_obj.sym, token_symbol_mismatch_exception,
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

      auto transfer_action = context.get_action().data_as_built_in_common_action<transfer>();
      try {
         const token_id_type token_id = context.get_action().account;
         EOS_ASSERT( token_id == context.get_receiver(), token_action_validate_exception, "transfer built-in standard token action handler should be invoked only in first receiver context" );

         const account_name from_account = transfer_action.from;
         const account_name to_account = transfer_action.to;
         const share_type transfer_amount = transfer_action.quantity.get_amount();

         EOS_ASSERT( from_account != to_account, token_action_validate_exception, "same from, to account" );
         EOS_ASSERT( transfer_action.quantity.is_valid(), token_action_validate_exception, "invalid quantity" );
         EOS_ASSERT( transfer_action.memo.size() <= 256, token_action_validate_exception, "memo has more than 256 bytes" );
         // action parameter validation will be done in context.transfer_token()

         auto& standard_token_manager = context.control.get_mutable_standard_token_manager();

         auto* token_meta_obj_ptr = standard_token_manager.get_token_meta_object(token_id);
         EOS_ASSERT( token_meta_obj_ptr, token_not_yet_created_exception, "token not yet created for the account ${account}", ("account", token_id) );
         auto token_meta_obj = *token_meta_obj_ptr;
         EOS_ASSERT( transfer_action.quantity.get_symbol() == token_meta_obj.sym, token_symbol_mismatch_exception,
                     "token symbol of quantity field mismatches with the symbol(${sym}) of the registered token metadata",
                     ("sym", token_meta_obj.sym.to_string()) );

         // need 'from' account signature
         context.require_authorization( from_account );

         // update token balance and ram usage
         context.transfer_token( from_account, to_account, transfer_amount );

         // notify 'transfer' action to 'from' and 'to' accounts who could process additional operations for their own sake
         context.require_recipient( from_account );
         context.require_recipient( to_account );

      } FC_CAPTURE_AND_RETHROW( (transfer_action) )
   }


   /**
    * 'txfee' standard token action apply handler
    */
   void apply_infrablockchain_built_in_action_txfee( apply_context& context ) {

      auto txfee_action = context.get_action().data_as_built_in_common_action<txfee>();
      try {
         const token_id_type token_id = context.get_action().account;
         EOS_ASSERT( token_id == context.get_receiver(), token_action_validate_exception, "txfee built-in standard token action handler should be invoked only in first receiver context" );

         const account_name payer_account = txfee_action.payer;
         EOS_ASSERT( payer_account.good(), token_action_validate_exception, "invalid payer account name" );
         EOS_ASSERT( txfee_action.fee.is_valid(), token_action_validate_exception, "invalid fee quantity" );

         const share_type txfee_amount = txfee_action.fee.get_amount();
         EOS_ASSERT( txfee_amount > 0, token_action_validate_exception, "tx fee amount must be greater than 0" );

         auto& db = context.db;
         auto& standard_token_manager = context.control.get_mutable_standard_token_manager();

         auto* token_meta_obj_ptr = standard_token_manager.get_token_meta_object(token_id);
         EOS_ASSERT( token_meta_obj_ptr, token_not_yet_created_exception, "token not yet created for the account ${account}", ("account", token_id) );
         auto token_meta_obj = *token_meta_obj_ptr;

         EOS_ASSERT( txfee_action.fee.get_symbol() == token_meta_obj.sym, token_symbol_mismatch_exception,
                     "token symbol of fee quantity field mismatches with the symbol(${sym}) of the registered token metadata",
                     ("sym", token_meta_obj.sym.to_string()) );

         auto* payer_account_obj_ptr = db.find<account_object, by_name>( payer_account );
         EOS_ASSERT( payer_account_obj_ptr != nullptr, no_token_target_account_exception,
                     "tx fee payer account ${account} does not exist", ("account", payer_account) );

         context.require_authorization( payer_account );

         standard_token_manager.subtract_token_balance( context, token_id, payer_account, txfee_amount );
         standard_token_manager.add_token_balance( context, token_id, infrablockchain_sys_tx_fee_account_name, txfee_amount );

         // notify 'txfee' action to payer account //and tx-fee system account
         context.require_recipient( payer_account );
         //context.require_recipient( INFRABLOCKCHAIN_SYS_TX_FEE_ACCOUNT );

      } FC_CAPTURE_AND_RETHROW( (txfee_action) )
   }


   /**
    * 'retire' standard token action apply handler
    */
   void apply_infrablockchain_built_in_action_retire( apply_context& context ) {

      auto retire_action = context.get_action().data_as_built_in_common_action<retire>();
      try {
         const token_id_type token_id = context.get_action().account;
         EOS_ASSERT( token_id == context.get_receiver(), token_action_validate_exception, "retire built-in standard token action handler should be invoked only in first receiver context" );

         EOS_ASSERT( retire_action.quantity.is_valid(), token_action_validate_exception, "invalid token redemption quantity" );
         EOS_ASSERT( retire_action.memo.size() <= 256, token_action_validate_exception, "memo has more than 256 bytes" );
         // action parameter validation will be done in context.issue_token()

         const share_type retire_amount = retire_action.quantity.get_amount();

         auto& standard_token_manager = context.control.get_mutable_standard_token_manager();

         auto* token_meta_obj_ptr = standard_token_manager.get_token_meta_object(token_id);
         EOS_ASSERT( token_meta_obj_ptr, token_not_yet_created_exception, "token not yet created for the account ${account}", ("account", token_id) );
         auto token_meta_obj = *token_meta_obj_ptr;

         EOS_ASSERT( retire_action.quantity.get_symbol() == token_meta_obj.sym, token_symbol_mismatch_exception,
                     "token symbol of redemption quantity field mismatches with the symbol(${sym}) of the registered token metadata",
                     ("sym", token_meta_obj.sym.to_string()) );

         // only the token account owner can retire(burn) tokens held by token account
         context.require_authorization( token_id );

         // update token balance and ram usage
         context.retire_token( retire_amount );

      } FC_CAPTURE_AND_RETHROW( (retire_action) )
   }

} } // namespace infrablockchain::chain
