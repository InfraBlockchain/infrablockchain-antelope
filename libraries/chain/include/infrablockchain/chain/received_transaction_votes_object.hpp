/**
 *  @file infrablockchain/chain/received_transaction_votes_object.hpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */
#pragma once

#include <eosio/chain/database_utils.hpp>
#include <eosio/chain/multi_index_includes.hpp>

namespace infrablockchain { namespace chain {

   using namespace eosio::chain;

   using tx_votes_sum_type = uint64_t;
   using tx_votes_sum_weighted_type = double;

   /**
    * @brief received transaction votes statistics for an account
    */
   class received_transaction_votes_object : public chainbase::object<infrablockchain_received_transaction_votes_object_type, received_transaction_votes_object> {
      OBJECT_CTOR(received_transaction_votes_object)

      id_type                     id;
      account_name                account;                          // transaction vote target account
      tx_votes_sum_type           tx_votes = 0;                     // sum of received transaction votes
      tx_votes_sum_weighted_type  tx_votes_weighted = 0.0;          // weighted (time-decaying) sum of received transaction votes
      uint16_t                    tx_votes_weighted_unique_idx = 0; // shared_multi_index_container does not allow ordered_non_unique index, making tx_votes_weighted value to be uniquely indexed
   };

   struct by_tx_votes_account;
   struct by_tx_votes_weighted;

   using received_transaction_votes_multi_index = chainbase::shared_multi_index_container<
      received_transaction_votes_object,
      indexed_by<
         ordered_unique< tag<by_id>, member<received_transaction_votes_object, received_transaction_votes_object::id_type, &received_transaction_votes_object::id> >,
         ordered_unique< tag<by_tx_votes_account>, member<received_transaction_votes_object, account_name, &received_transaction_votes_object::account>>,
//         ordered_non_unique< tag<by_tx_votes_weighted>, member<received_transaction_votes_object, tx_votes_sum_weighted_type, &received_transaction_votes_object::tx_votes_weighted>>
         ordered_unique< tag<by_tx_votes_weighted>,
            composite_key< received_transaction_votes_object,
               member<received_transaction_votes_object, tx_votes_sum_weighted_type, &received_transaction_votes_object::tx_votes_weighted>,
               member<received_transaction_votes_object, uint16_t, &received_transaction_votes_object::tx_votes_weighted_unique_idx>
            >
         >
      >
   >;

} } /// infrablockchain::chain


CHAINBASE_SET_INDEX_TYPE(infrablockchain::chain::received_transaction_votes_object, infrablockchain::chain::received_transaction_votes_multi_index)

FC_REFLECT(infrablockchain::chain::received_transaction_votes_object, (account)(tx_votes_weighted)(tx_votes)(tx_votes_weighted_unique_idx))
