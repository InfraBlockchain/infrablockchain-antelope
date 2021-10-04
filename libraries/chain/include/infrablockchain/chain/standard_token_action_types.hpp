/**
 *  @file infrablockchain/chain/standard_token_action_types.hpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */
#pragma once

#include <eosio/chain/types.hpp>
#include <eosio/chain/exceptions.hpp>
#include <eosio/chain/symbol.hpp>
#include <eosio/chain/asset.hpp>

namespace infrablockchain { namespace chain { namespace standard_token {

   using namespace eosio::chain;

   /**
    * Every account on InfraBlockchain can process built-in standard token actions (settokenmeta, issue, transfer, redeem)
    * without custom smart contract code deployed to an account.
    * An account can have optional token contract code inheriting built-in token actions.
    */

   struct settokenmeta {
      symbol        sym;  // token symbol
      std::string   url;
      std::string   desc; // token description

      static action_name get_name() {
         return "settokenmeta"_n;
      }
   };

   struct issue {
      account_name  to;  // token receiver account
      asset         qty; // token quantity
      std::string   tag;

      static action_name get_name() {
         return "issue"_n;
      }
   };

   struct transfer {
      account_name  from; // token sender account
      account_name  to;   // token receiver account
      asset         qty;  // token quantity
      std::string   tag;

      static action_name get_name() {
         return "transfer"_n;
      }
   };

   struct txfee {
      account_name  payer;  // transaction fee payer
      asset         fee;    // token quantity to pay tx fee

      static action_name get_name() {
         return "txfee"_n;
      }
   };

   struct redeem {
      asset         qty;  // token quantity to redeem(burn)
      std::string   tag;

      static action_name get_name() {
         return "redeem"_n;
      }
   };

   struct utils {
      static bool is_infrablockchain_standard_token_action(action_name action) {
         return action == transfer::get_name()
                || action == issue::get_name()
                || action == redeem::get_name()
                || action == txfee::get_name()
                || action == settokenmeta::get_name();
      }
   };

} } } /// infrablockchain::chain::standard_token

FC_REFLECT( infrablockchain::chain::standard_token::settokenmeta , (sym)(url)(desc) )
FC_REFLECT( infrablockchain::chain::standard_token::issue, (to)(qty)(tag) )
FC_REFLECT( infrablockchain::chain::standard_token::transfer, (from)(to)(qty)(tag) )
FC_REFLECT( infrablockchain::chain::standard_token::txfee, (payer)(fee) )
FC_REFLECT( infrablockchain::chain::standard_token::redeem, (qty)(tag) )
