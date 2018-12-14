/**
 *  @file contracts/yx.tokenabi/yx.tokenabi.cpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#include "yx.tokenabi.hpp"

#include <eosiolib/print.hpp>

namespace yosemite { namespace contract {

   using namespace eosio;

   void tokenabi::settokenmeta( symbol_type symbol, string url, string description ) {
      print(symbol); print(":"); print(url); print(":"); print(description);
      eosio_assert( false, "YOSEMITE blockchain core provides built-in standard token operation, settokenmeta" );
   }

   void tokenabi::issue( account_name t, account_name to, asset qty, string tag ) {
      print(t); print(":"); print(to); print(":"); print(qty); print(":"); print(tag);
      eosio_assert( false, "YOSEMITE blockchain core provides built-in standard token operation, issue" );
   }

   void tokenabi::transfer( account_name t, account_name from, account_name to, asset qty, string tag ) {
      print(t); print(":"); print(from); print(":"); print(to); print(":"); print(qty); print(":"); print(tag);
      eosio_assert( false, "YOSEMITE blockchain core provides built-in standard token operation, transfer" );
   }

   void tokenabi::txfee( account_name t, account_name payer, asset fee ) {
      print(t); print(":"); print(payer); print(":"); print(fee);
      eosio_assert( false, "YOSEMITE blockchain core provides built-in standard token operation, txfee" );
   }

   void tokenabi::redeem( asset qty, string tag ) {
      print(qty); print(":"); print(tag);
      eosio_assert( false, "YOSEMITE blockchain core provides built-in standard token operation, redeem" );
   }

} } /// namespace yosemite::contract

EOSIO_ABI( yosemite::contract::tokenabi, (settokenmeta)(issue)(transfer)(txfee)(redeem) )
