/**
 *  @file infrablockchain/chain/transaction_fee_table_manager.hpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */
#pragma once

#include <eosio/chain/exceptions.hpp>
#include <eosio/chain/types.hpp>
#include <eosio/chain/snapshot.hpp>
#include <eosio/chain/trace.hpp>
#include <chainbase/chainbase.hpp>

#include <infrablockchain/chain/transaction_fee_object.hpp>

namespace infrablockchain { namespace chain {

   using namespace eosio::chain;

   struct tx_fee_for_action {
      tx_fee_value_type value;
      tx_fee_type_type fee_type;
   };

   class transaction_fee_table_manager {
   public:
      explicit transaction_fee_table_manager(chainbase::database& db);

      void add_indices();
      void initialize_database();
      void add_to_snapshot( const snapshot_writer_ptr& snapshot ) const;
      void read_from_snapshot( const snapshot_reader_ptr& snapshot );

      void set_tx_fee_for_action(const account_name& code, const action_name& action, const tx_fee_value_type value, const tx_fee_type_type fee_type = fixed_tx_fee_per_action_type);
      void set_tx_fee_for_common_action(const action_name& action, const tx_fee_value_type value, tx_fee_type_type const fee_type = fixed_tx_fee_per_action_type);
      void set_default_tx_fee(const tx_fee_value_type value, const tx_fee_type_type fee_type = fixed_tx_fee_per_action_type);

      void unset_tx_fee_entry_for_action(const account_name& code, const action_name& action );

      tx_fee_for_action get_tx_fee_for_action(const account_name& code, const action_name& action) const;
      tx_fee_for_action get_tx_fee_for_common_action(const action_name& action) const;
      tx_fee_for_action get_default_tx_fee() const;

   private:
      chainbase::database& _db;
   };

} } /// infrablockchain::chain

FC_REFLECT(infrablockchain::chain::tx_fee_for_action, (value)(fee_type) )
