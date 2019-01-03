/**
 *  @file contracts/yosemitex.fiat.stable.token/yosemitex.fiat.stable.token.cpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#include "yosemitex.fiat.stable.token.hpp"

#include <yosemitelib/identity.hpp>

#define YOSEMITE_FIAT_KYC_AUTH YOSEMITE_ID_KYC_REAL_NAME_AUTH | YOSEMITE_ID_KYC_BANK_ACCOUNT_AUTH

namespace yosemitex { namespace contract {

   using namespace eosio;
   using namespace yosemite::identity;
   using std::string;

   void fiat_stable_token::issue( account_name /*t*/, account_name to, asset /*qty*/, string /*tag*/ ) {

      /// built-in action handler for 'issue' standard token action has already processed basic 'issue' operations

      eosio_assert( has_all_kyc_status( to, YOSEMITE_FIAT_KYC_AUTH ),
         "issue.to account failed to satisfy KYC constraints" );
   }

   void fiat_stable_token::transfer( account_name /*t*/, account_name from, account_name to, asset /*qty*/, string /*tag*/ ) {

      /// built-in action handler for 'transfer' standard token action has already processed basic 'transfer' operations

      eosio_assert( has_all_kyc_status( from, YOSEMITE_FIAT_KYC_AUTH ),
         "transfer.from account failed to satisfy KYC constraints" );
      eosio_assert( has_all_kyc_status( to, YOSEMITE_FIAT_KYC_AUTH ),
         "transfer.to account failed to satisfy KYC constraints" );
   }

   void fiat_stable_token::txfee( account_name /*t*/, account_name payer, asset /*fee*/ ) {

      /// built-in action handler for 'txfee' standard token action has already processed basic 'txfee' operations

      eosio_assert( has_all_kyc_status( payer, YOSEMITE_FIAT_KYC_AUTH ),
         "transfer.to account failed to satisfy KYC constraints" );
   }

} } /// namespace yosemitex::contract

EOSIO_ABI( yosemitex::contract::fiat_stable_token, (issue)(transfer)(txfee) )
