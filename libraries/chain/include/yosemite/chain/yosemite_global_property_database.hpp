/**
 *  @file yosemite/chain/yosemite_global_property_database.hpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosio/chain/database_utils.hpp>
#include <eosio/chain/multi_index_includes.hpp>
#include <yosemite/chain/system_token_list.hpp>

namespace yosemite { namespace chain {

   using namespace eosio::chain;

   class yosemite_global_property_object : public chainbase::object<yosemite_global_property_object_type, yosemite_global_property_object> {
      OBJECT_CTOR(yosemite_global_property_object, (system_token_list))

      id_type                        id;
      shared_system_token_list_type  system_token_list;
   };

   using yosemite_global_property_multi_index = chainbase::shared_multi_index_container<
      yosemite_global_property_object,
      indexed_by<
         ordered_unique< tag<by_id>, member<yosemite_global_property_object, yosemite_global_property_object::id_type, &yosemite_global_property_object::id> >
      >
   >;

} } /// yosemite::chain

CHAINBASE_SET_INDEX_TYPE(yosemite::chain::yosemite_global_property_object, yosemite::chain::yosemite_global_property_multi_index)

FC_REFLECT(yosemite::chain::yosemite_global_property_object, (system_token_list))
