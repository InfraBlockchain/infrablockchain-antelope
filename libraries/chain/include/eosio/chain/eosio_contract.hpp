/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#pragma once

#include <eosio/chain/types.hpp>
#include <eosio/chain/contract_types.hpp>

namespace eosio { namespace chain {

   class apply_context;

   /**
    * @defgroup native_action_handlers Native Action Handlers
    */
   ///@{
   void apply_yosemite_newaccount(apply_context&);
   void apply_yosemite_updateauth(apply_context&);
   void apply_yosemite_deleteauth(apply_context&);
   void apply_yosemite_linkauth(apply_context&);
   void apply_yosemite_unlinkauth(apply_context&);

   /*
   void apply_yosemite_postrecovery(apply_context&);
   void apply_yosemite_passrecovery(apply_context&);
   void apply_yosemite_vetorecovery(apply_context&);
   */

   void apply_yosemite_setcode(apply_context&);
   void apply_yosemite_setabi(apply_context&);

   void apply_yosemite_canceldelay(apply_context&);
   ///@}  end action handlers

} } /// namespace eosio::chain
