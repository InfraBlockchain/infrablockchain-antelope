/**
 *  @file contracts/sys.tokenabi/sys.tokenabi.cpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */
#include "sys.tokenabi.hpp"

#include <eosiolib/print.hpp>

namespace infrablockchain { namespace contract {

   using namespace eosio;
   using std::string;

   void tokenabi::settokenmeta( const symbol_type /*sym*/, const string& /*url*/, const string& /*desc*/ ) {
      eosio_assert( false, "INFRABLOCKCHAIN core provides built-in standard token operation, settokenmeta" );
   }

   void tokenabi::issue( const account_name /*t*/, const account_name /*to*/, const asset& /*qty*/, const string& /*tag*/ ) {
      eosio_assert( false, "INFRABLOCKCHAIN core provides built-in standard token operation, issue" );
   }

   void tokenabi::transfer( const account_name /*t*/, const account_name /*from*/, const account_name /*to*/, const asset& /*qty*/, const string& /*tag*/ ) {
      eosio_assert( false, "INFRABLOCKCHAIN core provides built-in standard token operation, transfer" );
   }

   void tokenabi::txfee( const account_name /*t*/, const account_name /*payer*/, const asset& /*fee*/ ) {
      eosio_assert( false, "INFRABLOCKCHAIN core provides built-in standard token operation, txfee" );
   }

   void tokenabi::redeem( const asset& /*qty*/, const string& /*tag*/ ) {
      eosio_assert( false, "INFRABLOCKCHAIN core provides built-in standard token operation, redeem" );
   }

} } /// namespace infrablockchain::contract

EOSIO_ABI( infrablockchain::contract::tokenabi, (settokenmeta)(issue)(transfer)(txfee)(redeem) )
