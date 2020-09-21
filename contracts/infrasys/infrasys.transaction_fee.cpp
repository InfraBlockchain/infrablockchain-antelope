/**
 *  @file contracts/infrasys/infrasys.transaction_fee.cpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */

#include "infrasys.hpp"

#include <infrablockchainlib/trx_fee_api.h>

namespace infrablockchainsys {

   using namespace eosio;

   /**
    *  Set Transaction Fee For Action
    *  @brief set transaction fee for an action. the transaction fee for each code/action is determined by the 2/3+ block producers.
    *  if code == account_name(0), this sets a transaction fee for the built-in common actions (e.g. INFRABLOCKCHAIN standard token actions) that every account has.
    *  if code == account_name(0) and action == action_name(0), this sets default transaction fee for actions that don't have explicit transaction fee setup.
    *
    *  @param code - account name of contract code
    *  @param action - action name
    *  @param value - transaction fee value
    *  @param feetype - transaction fee type (1: fixed_tx_fee_per_action_type)
    */
   void system_contract::settxfee( const account_name code, const action_name action, const int32_t value, const uint32_t feetype ) {
      set_trx_fee_for_action( code, action, value, feetype );
   }

   /**
    *  Unset Transaction Fee For Action
    *  @brief delete transaction fee entry for an action (to the unsset status)
    *
    *  @param code - account name of contract code
    *  @param action - action name
    */
   void system_contract::unsettxfee( const account_name code, const action_name action ) {
      unset_trx_fee_for_action( code, action );
   }

} //namespace infrablockchainsys
