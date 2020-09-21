/**
 *  @file contracts/yosemitex.credit.token/yosemitex.credit.token.cpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#include "yosemitex.credit.token.hpp"

#include <infrablockchainlib/identity.hpp>
#include <infrablockchainlib/token_api.h>

#include <eosiolib/symbol.hpp>
#include <eosiolib/action.hpp>

#define YOSEMITE_CREDIT_KYC_AUTH INFRABLOCKCHAIN_ID_KYC_REAL_NAME_AUTH | INFRABLOCKCHAIN_ID_KYC_BANK_ACCOUNT_AUTH

namespace yosemitex { namespace contract {

   using namespace eosio;
   using namespace infrablockchain::identity;
   using std::string;

   void credit_token::issue( const account_name /*t*/, const account_name to, const asset& /*qty*/, const string& /*tag*/ ) {

      /// built-in action handler for 'issue' standard token action has already processed basic 'issue' operations

      eosio_assert( has_all_kyc_status( to, YOSEMITE_CREDIT_KYC_AUTH ),
                    "issue.to account failed to satisfy KYC constraints" );
   }

   void credit_token::transfer( const account_name /*t*/, const account_name from, const account_name to, const asset& /*qty*/, const string& /*tag*/ ) {

      /// built-in action handler for 'transfer' standard token action has already processed basic 'transfer' operations

      eosio_assert( has_all_kyc_status( from, YOSEMITE_CREDIT_KYC_AUTH ),
                    "transfer.from account failed to satisfy KYC constraints" );
      eosio_assert( has_all_kyc_status( to, YOSEMITE_CREDIT_KYC_AUTH ),
                    "transfer.to account failed to satisfy KYC constraints" );
   }

   void credit_token::txfee( const account_name /*t*/, const account_name payer, const asset& /*fee*/ ) {

      /// built-in action handler for 'txfee' standard token action has already processed basic 'txfee' operations

      eosio_assert( has_all_kyc_status( payer, YOSEMITE_CREDIT_KYC_AUTH ),
                    "transfer.to account failed to satisfy KYC constraints" );
   }

   void credit_token::creditlimit( const account_name account, const asset& credit_limit, const string& tag ) {

      symbol_type token_symbol(get_token_symbol( _self )); // if current token account is not a valid token, exception will be thrown in this call
      int64_t limit_amount = credit_limit.amount;

      eosio_assert( token_symbol.is_valid() && token_symbol.value == credit_limit.symbol.value, "token symbol of credit_limit quantity mismatches the token symbol of current contract" );
      eosio_assert( limit_amount >= 0, "credit limit amount must be zero or positive value" );
      eosio_assert( tag.size() <= 256, "too long tag string, max tag string size is 256 bytes" );

      eosio_assert( has_all_kyc_status( account, YOSEMITE_CREDIT_KYC_AUTH ),
                    "credit-limit target account failed to satisfy KYC constraints" );

      // only the token account owner can set user account's credit limit
      require_auth( _self );

      credit_offering_table_idx credit_offering_table( _self, _self );

      auto credit_offering_info_it = credit_offering_table.find( account );
      if ( credit_offering_info_it == credit_offering_table.end() ) {
         eosio_assert( limit_amount > 0, "credit limit must be positive value" );
         credit_offering_table.emplace( _self, [&](credit_offering_info& info) {
            info.account = account;
            info.credit_limit = limit_amount;
            info.credit_issued = 0;
         });
      } else {
         auto current_info = *credit_offering_info_it;
         eosio_assert( limit_amount >= current_info.credit_issued, "credit-limit less than credit-issued not allowed" );
         if ( limit_amount  == 0 ) {
            credit_offering_table.erase( credit_offering_info_it );
         } else {
            credit_offering_table.modify( credit_offering_info_it, 0, [&](credit_offering_info& info) {
               info.credit_limit = limit_amount;
            });
         }
      }

      // notify 'creditlimit' action to the target account
      require_recipient( account );
   }

   void credit_token::creditissue( const account_name issuer, const account_name to, const asset& qty, const string& tag ) {

      symbol_type token_symbol(get_token_symbol( _self )); // if current token account is not a valid token, exception will be thrown in this call
      int64_t issue_amount = qty.amount;

      eosio_assert( token_symbol.is_valid() && token_symbol.value == qty.symbol.value, "token symbol of credit issue quantity parameter mismatches the token symbol of current contract" );
      eosio_assert( issue_amount > 0, "credit issue amount must be positive value" );
      eosio_assert( tag.size() <= 256, "too long tag string, max tag string size is 256 bytes" );

      eosio_assert( has_all_kyc_status( issuer, YOSEMITE_CREDIT_KYC_AUTH ),
                    "credit-issue issuer failed to satisfy KYC constraints" );

      // account who is trying to issue new token needs to sign
      require_auth( issuer );

      credit_offering_table_idx credit_offering_table( _self, _self );

      auto& credit_info = credit_offering_table.get( issuer );
      eosio_assert( credit_info.credit_limit - credit_info.credit_issued >= issue_amount, "new token issuance exceeds the issuer's credit limit" );

      // issue new tokens to the issuer
      issue_token( issuer, issue_amount );

      credit_offering_table.modify( credit_info, 0, [&](credit_offering_info& info) {
         info.credit_issued += issue_amount;
      });

      // notify 'creditissue' action to the issuer account
      require_recipient( issuer );

      if ( issuer != to ) {
         SEND_INLINE_ACTION( *this, transfer, {issuer, N(codetransfer)}, {_self, issuer, to, qty, tag} );
      }
   }

   void credit_token::creditredeem( const account_name account, const asset& qty, const string& tag ) {

      symbol_type token_symbol(get_token_symbol( _self )); // if current token account is not a valid token, exception will be thrown in this call
      int64_t redeem_amount = qty.amount;

      eosio_assert( token_symbol.is_valid() && token_symbol.value == qty.symbol.value, "token symbol of credit redeem quantity parameter mismatches the token symbol of current contract" );
      eosio_assert( redeem_amount > 0, "credit redeem amount must be positive value" );
      eosio_assert( tag.size() <= 256, "too long tag string, max tag string size is 256 bytes" );

      // only the token account owner can change user account's credit issued
      require_auth( _self );

      credit_offering_table_idx credit_offering_table( _self, _self );

      auto& credit_info = credit_offering_table.get( account );
      eosio_assert( redeem_amount <= credit_info.credit_issued, "redeem amount must be less than or equal to current credit-issued amount" );

      credit_offering_table.modify( credit_info, 0, [&](credit_offering_info& info) {
         info.credit_issued -= redeem_amount;
      });

      // notify 'creditredeem' action to the target account
      require_recipient( account );
   }

   void credit_token::offtransfer( const account_name t, const account_name to, const string& tag ) {
      eosio_assert( t == _self, "invalid token id" );
      eosio_assert( is_account( to ), "to account does not exist" );
      eosio_assert( tag.size() <= 256, "too long tag string, max tag string size is 256 bytes" );

      // only the credit token owner can log off-chain token transfer events on-chain.
      require_auth( _self );

      // notify 'earntoken' action to the account who receives token in the corresponding off-chain token transfer
      require_recipient( to );
   }

} } /// namespace yosemitex::contract

EOSIO_ABI( yosemitex::contract::credit_token,
           (issue)(transfer)(txfee)
           (creditlimit)(creditissue)(creditredeem)(offtransfer) )
