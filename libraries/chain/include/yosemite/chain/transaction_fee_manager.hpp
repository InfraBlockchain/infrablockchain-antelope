/**
 *  @file yosemite/chain/transaction_fee_manager.hpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosio/chain/exceptions.hpp>
#include <eosio/chain/types.hpp>
#include <eosio/chain/snapshot.hpp>
#include <chainbase/chainbase.hpp>

#include <yosemite/chain/transaction_fee_database.hpp>

namespace yosemite { namespace chain {

   using namespace eosio::chain;

   struct tx_fee_for_action {
      tx_fee_value_type value;
      tx_fee_type_type fee_type;
   };

   class transaction_fee_manager {
   public:
      explicit transaction_fee_manager(chainbase::database& db);

      void add_indices();
      void initialize_database();
      void add_to_snapshot( const snapshot_writer_ptr& snapshot ) const;
      void read_from_snapshot( const snapshot_reader_ptr& snapshot );

      void set_tx_fee_for_action(account_name code, action_name action, tx_fee_value_type value, tx_fee_type_type fee_type = fixed_tx_fee_per_action_type);
      void set_tx_fee_for_common_action(action_name action, tx_fee_value_type value, tx_fee_type_type fee_type = fixed_tx_fee_per_action_type);
      void set_default_tx_fee(tx_fee_value_type value, tx_fee_type_type fee_type = fixed_tx_fee_per_action_type);

      tx_fee_for_action get_tx_fee_for_action(account_name code, action_name action) const;
      tx_fee_for_action get_tx_fee_for_common_action(action_name action) const;
      tx_fee_for_action get_default_tx_fee() const;

   private:
      chainbase::database& _db;
   };

} } /// yosemite::chain