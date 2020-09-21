/**
 *  @file infrablockchainlib/trx_fee_api.h
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */
#pragma once

#include <eosiolib/types.h>

extern "C" {

/**
 *  @defgroup INFRABLOCKCHAIN Transaction Fee API
 *  @ingroup contractdev
 *  @brief Defines API for INFRABLOCKCHAIN-Transaction-Fee
 */

/**
 *  @defgroup INFRABLOCKCHAIN Transaction Fee C API
 *  @ingroup trxfeeapi
 *  @brief Defines API for INFRABLOCKCHAIN-Transaction-Fee
 *  @{
 */


/**
 *  Set Transaction Fee For Action
 *  @brief set transaction fee for an action. the transaction fee for each code/action is determined by the 2/3+ block producers.
 *  if code == account_name(0), this sets a transaction fee for the built-in common actions (e.g. INFRABLOCKCHAIN standard token actions) that every account has.
 *  if code == account_name(0) and action == action_name(0), this sets default transaction fee for actions that don't have explicit transaction fee setup.
 *
 *  @param code - account name of contract code
 *  @param action - action name
 *  @param value - transaction fee value
 *  @param fee_type - transaction fee type (1: fixed_tx_fee_per_action_type)
 */
void set_trx_fee_for_action( account_name code, action_name action, int32_t value, uint32_t fee_type );

/**
 *  Unset Transaction Fee For Action
 *  @brief delete transaction fee entry for an action (to the unsset status)
 *
 *  @param code - account name of contract code
 *  @param action - action name
 */
void unset_trx_fee_for_action( account_name code, action_name action );

/**
 *  Get Transaction Fee For Action
 *  @brief get transaction fee for an action,
 *  if code == account_name(0), transaction fee info for an built-in common action is retrieved.
 *  if code == account_name(0) and action == action_name(0), retrieves default transaction fee setup for actions that don't have explicit transaction fee setup.
 *
 *  @param code - account name of contract code
 *  @param action - action name
 *  @param data - output buffer of the packed 'infrablockchain_tx_fee_for_action' object, only retrieved if sufficent size to hold packed data.
 *  @param datalen - size of the data buffer, 0 to report required size.
 *  @return size of the packed 'tx_fee_for_action' data
 *  @pre `data` is a valid pointer to a range of memory at least `datalen` bytes long
 *  @post `data` is filled with packed 'infrablockchain_tx_fee_for_action' object
 */
uint32_t get_trx_fee_for_action( account_name code, action_name action, char* data, uint32_t datalen );

/**
 *  Get the transaction fee payer account name
 *  @brief Get the transaction fee payer account to which transaction fee is charged
 *  @return the transaction fee payer account name
 */
account_name trx_fee_payer();

///@ } trxfeeapi
}

struct infrablockchain_tx_fee_for_action {
   int32_t value;
   uint32_t fee_type;
};
