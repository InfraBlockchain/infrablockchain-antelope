/**
 *  @file yosemite/chain/yosemite_global_property_database.hpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosio/chain/database_utils.hpp>
#include <eosio/chain/multi_index_includes.hpp>
#include <yosemite/chain/system_token_list.hpp>
#include <yosemite/chain/received_transaction_votes_database.hpp>

namespace yosemite { namespace chain {

   using namespace eosio::chain;

   class yosemite_global_property_object : public chainbase::object<yosemite_global_property_object_type, yosemite_global_property_object> {
      OBJECT_CTOR(yosemite_global_property_object, (system_token_list))

      id_type                        id;
      shared_system_token_list_type  system_token_list;
      tx_votes_sum_weighted_type     total_tx_votes_weighted = 0.0; /// the sum of all transaction votes (weighted sum for vote decay)
      tx_votes_sum_type              total_tx_votes = 0; /// the sum of all transaction votes (un-weighted sum)
      uint64_t                       reserved1 = 0;
      uint64_t                       reserved2 = 0;
   };

   using yosemite_global_property_multi_index = chainbase::shared_multi_index_container<
      yosemite_global_property_object,
      indexed_by<
         ordered_unique< tag<by_id>, member<yosemite_global_property_object, yosemite_global_property_object::id_type, &yosemite_global_property_object::id> >
      >
   >;

} } /// yosemite::chain

CHAINBASE_SET_INDEX_TYPE(yosemite::chain::yosemite_global_property_object, yosemite::chain::yosemite_global_property_multi_index)

FC_REFLECT(yosemite::chain::yosemite_global_property_object, (system_token_list)(total_tx_votes_weighted)(total_tx_votes)(reserved1)(reserved2))
