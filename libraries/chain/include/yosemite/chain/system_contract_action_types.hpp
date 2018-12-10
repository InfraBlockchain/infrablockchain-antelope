/**
 *  @file yosemite/chain/system_contract_action_types.hpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosio/chain/types.hpp>
#include <eosio/chain/config.hpp>

namespace yosemite { namespace chain { namespace system_contract {

   using namespace eosio::chain;

   /**
    * Authorize a token as system token which can be used as transaction fee payment
    */
   struct authsystoken {
      account_name token; // token id to set as system token
      uint32_t     weight; // token value weight as transaction fee payment, 10000 = 1.0x, 5000 = 0.5x (tx fee is charged 2x)

      static account_name get_account() {
         return config::system_account_name;
      }

      static action_name get_name() {
         return N(authsystoken);
      }
   };

   /**
    * Remove (unauthorize) a system token
    */
   struct rmvsystoken {
      account_name token; // token id to set as system token

      static account_name get_account() {
         return config::system_account_name;
      }

      static action_name get_name() {
         return N(rmvsystoken);
      }
   };

} } } /// yosemite::chain::token

FC_REFLECT( yosemite::chain::system_contract::authsystoken , (token)(weight) )
FC_REFLECT( yosemite::chain::system_contract::rmvsystoken, (token) )
