/**
 *  @file contracts/yx.fiat.stable.token/yx.fiat.stable.token.cpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#include "yx.fiat.stable.token.hpp"

#include <yosemitelib/identity.hpp>

namespace yosemite { namespace contract {

   using namespace eosio;
   using std::string;

   void fiat_stable_token::issue( account_name /*t*/, account_name to, asset /*qty*/, string /*tag*/ ) {

      /// built-in action handler for 'issue' standard token action has already processed basic 'issue' operations

      eosio_assert( identity::has_all_kyc_status( to, YOSEMITE_ID_KYC_MAX_AUTH ),
         "issue.to account failed to satisfy KYC constraints" );
   }

   void fiat_stable_token::transfer( account_name /*t*/, account_name from, account_name to, asset /*qty*/, string /*tag*/ ) {

      /// built-in action handler for 'transfer' standard token action has already processed basic 'issue' operations

      eosio_assert( identity::has_all_kyc_status( from, YOSEMITE_ID_KYC_MAX_AUTH ),
         "transfer.from account failed to satisfy KYC constraints" );
      eosio_assert( identity::has_all_kyc_status( to, YOSEMITE_ID_KYC_MAX_AUTH ),
         "transfer.to account failed to satisfy KYC constraints" );
   }

   void fiat_stable_token::txfee( account_name /*t*/, account_name payer, asset /*fee*/ ) {

      /// built-in action handler for 'txfee' standard token action has already processed basic 'issue' operations

      eosio_assert( identity::has_all_kyc_status( payer, YOSEMITE_ID_KYC_MAX_AUTH ),
         "transfer.to account failed to satisfy KYC constraints" );
   }

} } /// namespace yosemite::contract

EOSIO_ABI( yosemite::contract::fiat_stable_token, (issue)(transfer)(txfee) )
