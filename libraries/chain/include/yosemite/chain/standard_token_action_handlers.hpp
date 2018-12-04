/**
 *  @file yosemite/chain/standard_token_action_handlers.hpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosio/chain/types.hpp>
#include <eosio/chain/contract_types.hpp>
#include <eosio/chain/apply_context.hpp>
#include <yosemite/chain/standard_token_database.hpp>

namespace yosemite { namespace chain {

   using namespace eosio::chain;

   /**
    * @defgroup native_action_handlers Native Action Handlers
    */
   ///@{

   void apply_yosemite_built_in_action_settokenmeta( apply_context& context);

   void apply_yosemite_built_in_action_issue( apply_context& context );

   void apply_yosemite_built_in_action_transfer( apply_context& context );

   ///@}  end action handlers

   void add_token_balance( apply_context& context, token_id_type token_id, account_name owner, share_type value );
   void subtract_token_balance( apply_context& context, token_id_type token_id, account_name owner, share_type value );

} } /// namespace yosemite::chain::stdtoken
