/**
 *  @file contracts/yx.tokenabi/yx.tokenabi.hpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/symbol.hpp>

#include <string>

namespace yosemite { namespace contract {

   using namespace eosio;
   using std::string;

   /**
    * Every account on YOSEMITE blockchain can process built-in standard token actions (settokenmeta, issue, transfer, redeem)
    * without custom smart contract code deployed to an account.
    * This system contract (yx.tokenabi) provide standard application binary interface(abi) for standard token actions.
    * Client tools such as clyos encoding YOSEMITE blockchain transactions could refer 'yx.tokenabi' contract account
    * to retrieve abi information for standard token actions.
    */
   class tokenabi : public eosio::contract {
   public:
      explicit tokenabi( account_name self ) : contract(self) {}

      void settokenmeta( symbol_type symbol, string url, string description );

      void issue( account_name t, account_name to, asset qty, string tag );

      void transfer( account_name t, account_name from, account_name to, asset qty, string tag );

      void txfee( account_name t, account_name payer, asset fee );

      void redeem( asset qty, string tag );
   };

} } /// namespace yosemite::contract
