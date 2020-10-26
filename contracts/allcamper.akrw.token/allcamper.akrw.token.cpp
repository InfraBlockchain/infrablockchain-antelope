#include "allcamper.akrw.token.hpp"

#include <infrablockchainlib/identity.hpp>

#define ALLCAMPER_AKRW_KYC_AUTH INFRABLOCKCHAIN_ID_KYC_REAL_NAME_AUTH | INFRABLOCKCHAIN_ID_KYC_BANK_ACCOUNT_AUTH

namespace allcamper {

   using namespace eosio;
   using namespace infrablockchain::identity;
   using std::string;

   void akrw_token::issue( const account_name /*t*/, const account_name to, const asset& /*qty*/, const string& /*tag*/ ) {

      /// built-in action handler for 'issue' standard token action has already processed basic 'issue' operations

      eosio_assert( has_all_kyc_status( to, ALLCAMPER_AKRW_KYC_AUTH ),
         "issue.to account failed to satisfy KYC constraints" );
   }

   void akrw_token::transfer( const account_name /*t*/, const account_name from, const account_name to, const asset& /*qty*/, const string& /*tag*/ ) {

      /// built-in action handler for 'transfer' standard token action has already processed basic 'transfer' operations

      eosio_assert( has_all_kyc_status( from, ALLCAMPER_AKRW_KYC_AUTH ),
         "transfer.from account failed to satisfy KYC constraints" );
      eosio_assert( has_all_kyc_status( to, ALLCAMPER_AKRW_KYC_AUTH ),
         "transfer.to account failed to satisfy KYC constraints" );
   }

   void akrw_token::txfee( const account_name /*t*/, const account_name payer, const asset& /*fee*/ ) {

      /// built-in action handler for 'txfee' standard token action has already processed basic 'txfee' operations

      eosio_assert( has_all_kyc_status( payer, ALLCAMPER_AKRW_KYC_AUTH ),
         "transfer.to account failed to satisfy KYC constraints" );
   }

} /// namespace allcamper

EOSIO_ABI( allcamper::akrw_token, (issue)(transfer)(txfee) )
