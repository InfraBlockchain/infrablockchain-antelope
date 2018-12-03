/**
 *  @file yosemite/chain/standard_token_action_handlers.hpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosio/chain/types.hpp>
#include <eosio/chain/contract_types.hpp>
#include <eosio/chain/apply_context.hpp>

namespace yosemite { namespace chain {

   using namespace eosio::chain;

   /**
    * @defgroup native_action_handlers Native Action Handlers
    */
   ///@{

   void apply_yosemite_built_in_action_settokenmeta(apply_context&);

   ///@}  end action handlers

} } /// namespace yosemite::chain
