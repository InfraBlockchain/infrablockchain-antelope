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
    * @defgroup native_standard_token_action_handlers Native Standard Token Action Handlers
    */
   ///@{

   void apply_yosemite_built_in_action_settokenmeta( apply_context& context);

   void apply_yosemite_built_in_action_issue( apply_context& context );

   void apply_yosemite_built_in_action_transfer( apply_context& context );

   void apply_yosemite_built_in_action_txfee( apply_context& context );

   void apply_yosemite_built_in_action_redeem( apply_context& context );

   ///@}  end action handlers

} } /// namespace yosemite::chain::stdtoken
