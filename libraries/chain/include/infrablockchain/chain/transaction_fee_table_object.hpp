/**
 *  @file infrablockchain/chain/transaction_fee_table_object.hpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */
#pragma once

#include <eosio/chain/database_utils.hpp>
#include <eosio/chain/multi_index_includes.hpp>

namespace infrablockchain { namespace chain {

   using namespace eosio::chain;

   enum tx_fee_type {
      null_tx_fee_type = 0,
      fixed_tx_fee_per_action_type  // transaction_fee_table_object::value => fixed transaction fee amount per action
   };

   using tx_fee_type_type  = uint32_t;
   using tx_fee_value_type = int32_t;

   /**
    * @brief transaction fee information of built-in actions or actions monitored by block producers
    * for which transaction fee rates and policies are predefined (authorized by block producers)
    */
   class transaction_fee_table_object : public chainbase::object<infrablockchain_transaction_fee_table_object_type, transaction_fee_table_object> {
      OBJECT_CTOR(transaction_fee_table_object)

      id_type            id;
      account_name       code;   // code account, 0 for global actions
      action_name        action; // action name
      tx_fee_value_type  value;
      tx_fee_type_type   fee_type = fixed_tx_fee_per_action_type;

      // [code == 0 and action == 0]
      //   reserved for the special 'default' transaction_fee_table_object record
      //   applied as default transaction fee policy for every action that doesn't have transaction_fee_table_object record
   };

   struct by_code_action;

   using transaction_fee_table_multi_index = chainbase::shared_multi_index_container<
      transaction_fee_table_object,
      indexed_by<
         ordered_unique< tag<by_id>, member<transaction_fee_table_object, transaction_fee_table_object::id_type, &transaction_fee_table_object::id> >,
         ordered_unique< tag<by_code_action>,
            composite_key< transaction_fee_table_object,
               member<transaction_fee_table_object, account_name, &transaction_fee_table_object::code>,
               member<transaction_fee_table_object, action_name, &transaction_fee_table_object::action>
            >
         >
      >
   >;

} } /// infrablockchain::chain


CHAINBASE_SET_INDEX_TYPE(infrablockchain::chain::transaction_fee_table_object, infrablockchain::chain::transaction_fee_table_multi_index)

FC_REFLECT(infrablockchain::chain::transaction_fee_table_object, (code)(action)(value)(fee_type) )
