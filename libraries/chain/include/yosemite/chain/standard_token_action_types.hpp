/**
 *  @file yosemite/chain/standard_token_action_types.hpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosio/chain/types.hpp>
#include <eosio/chain/exceptions.hpp>
#include <eosio/chain/symbol.hpp>

namespace yosemite { namespace chain {

   using namespace eosio::chain;

   struct settokenmeta {
      symbol       symbol;
      std::string  url;
      std::string  description;

      static action_name get_name() {
         return N(settokenmeta);
      }
   };

   bool is_yosemite_standard_token_action(action_name action);

} } /// yosemite::chain

FC_REFLECT( yosemite::chain::settokenmeta , (symbol)(url)(description) )
