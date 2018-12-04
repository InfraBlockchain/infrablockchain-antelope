/**
 *  @file yosemite/chain/standard_token_action_types.hpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosio/chain/types.hpp>
#include <eosio/chain/exceptions.hpp>
#include <eosio/chain/symbol.hpp>
#include <eosio/chain/asset.hpp>

namespace yosemite { namespace chain { namespace token {

   using namespace eosio::chain;

   /**
    * Every account can process built-in token actions (settokenmeta, issue, transfer, redeem)
    * An account can have optional token contract code inheriting built-in token actions
    */

   struct settokenmeta {
      symbol       symbol;
      std::string  url;
      std::string  description;

      static action_name get_name() {
         return N(settokenmeta);
      }
   };

   struct issue {
      account_name  to;
      asset         qty; // token quantity
      std::string   memo;

      static action_name get_name() {
         return N(issue);
      }
   };

   struct transfer {
      account_name  from;
      account_name  to;
      asset         qty; // token quantity
      std::string   memo;

      static action_name get_name() {
         return N(transfer);
      }
   };

   struct txfee {
      account_name  payer;
      asset         fee; // token quantity to pay tx fee

      static action_name get_name() {
         return N(txfee);
      }
   };

   struct redeem {
      asset         qty; // token quantity to redeem(burn)
      std::string   memo;

      static action_name get_name() {
         return N(redeem);
      }
   };

   bool is_yosemite_standard_token_action(action_name action);

} } } /// yosemite::chain::stdtoken

FC_REFLECT( yosemite::chain::token::settokenmeta , (symbol)(url)(description) )
FC_REFLECT( yosemite::chain::token::issue, (to)(qty)(memo) )
FC_REFLECT( yosemite::chain::token::transfer, (from)(to)(qty)(memo) )
FC_REFLECT( yosemite::chain::token::txfee, (payer)(fee) )
FC_REFLECT( yosemite::chain::token::redeem, (qty)(memo) )
