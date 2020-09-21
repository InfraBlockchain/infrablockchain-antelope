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
   void apply_infrasys_newaccount(apply_context&);
   void apply_infrasys_updateauth(apply_context&);
   void apply_infrasys_deleteauth(apply_context&);
   void apply_infrasys_linkauth(apply_context&);
   void apply_infrasys_unlinkauth(apply_context&);

   /*
   void apply_infrasys_postrecovery(apply_context&);
   void apply_infrasys_passrecovery(apply_context&);
   void apply_infrasys_vetorecovery(apply_context&);
   */

   void apply_infrasys_setcode(apply_context&);
   void apply_infrasys_setabi(apply_context&);

   void apply_infrasys_canceldelay(apply_context&);
   ///@}  end action handlers

} } /// namespace eosio::chain
