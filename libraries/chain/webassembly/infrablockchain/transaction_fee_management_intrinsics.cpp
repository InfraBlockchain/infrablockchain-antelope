/**
 *  @file chain/webassembly/infrablockchain/transaction_fee_management_intrinsics.cpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */

#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/chain/apply_context.hpp>

namespace eosio { namespace chain { namespace webassembly {

   ///////////////////////////////////////////////////////////////
   /// InfraBlockchain Transaction Fee Management Core API (Intrinsics)

   /**
    *  Set Transaction Fee For Action
    *  @brief set transaction fee for an action. the transaction fee for each code/action is determined by the 2/3+ block producers.
    *  if code == account_name(0), this sets a transaction fee for the built-in common actions (e.g. InfraBlockchain standard token actions) that every account has.
    *  if code == account_name(0) and action == action_name(0), this sets default transaction fee for actions that don't have explicit transaction fee setup.
    *
    *  @param code - account name of contract code
    *  @param action - action name
    *  @param value - transaction fee value
    *  @param fee_type - transaction fee type (1: fixed_tx_fee_per_action_type)
    */
   void interface::set_trx_fee_for_action( account_name code, action_name action, int32_t value, uint32_t fee_type ) {
      context.set_transaction_fee_for_action( code, action, value, fee_type );
   }

   /**
    *  Unset Transaction Fee For Action
    *  @brief delete transaction fee entry for an action (to the unsset status)
    *
    *  @param code - account name of contract code
    *  @param action - action name
    */
   void interface::unset_trx_fee_for_action( account_name code, action_name action ) {
      context.unset_transaction_fee_for_action( code, action );
   }

   /**
    *  Get Transaction Fee For Action
    *  @brief get transaction fee for an action,
    *  if code == account_name(0), transaction fee info for an built-in common action is retrieved.
    *  if code == account_name(0) and action == action_name(0), retrieves default transaction fee setup for actions that don't have explicit transaction fee setup.
    *
    *  @param code - account name of contract code
    *  @param action - action name
    *  @param[out] packed_trx_fee_for_action - output buffer of the packed 'infrablockchain::chain::tx_fee_for_action' object, only retrieved if sufficent size to hold packed data.
    *  @return size of the packed 'infrablockchain::chain::tx_fee_for_action' data
    */
   uint32_t interface::get_trx_fee_for_action_packed( account_name code, action_name action, legacy_span<char> packed_trx_fee_for_action ) const {
      infrablockchain::chain::tx_fee_for_action tx_fee_for_action = context.get_transaction_fee_for_action( code ,action );

      auto s = fc::raw::pack_size( tx_fee_for_action );
      if( packed_trx_fee_for_action.size() == 0 ) return s;

      if ( s <= packed_trx_fee_for_action.size() ) {
         datastream<char*> ds( packed_trx_fee_for_action.data(), s );
         fc::raw::pack(ds, tx_fee_for_action);
         return s;
      }
      return 0;
   }

   /**
    *  Get the transaction fee payer account name
    *  @brief Get the transaction fee payer account to which transaction fee is charged
    *  @return the transaction fee payer account name
    */
   account_name interface::trx_fee_payer() const {
      return context.get_transaction_fee_payer();
   }

} } } /// eosio::chain::webassembly
