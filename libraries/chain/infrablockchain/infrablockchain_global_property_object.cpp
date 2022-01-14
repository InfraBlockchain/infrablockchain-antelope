/**
 *  @file chain/infrablockchain/infrablockchain_global_property_object.cpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */

#include <infrablockchain/chain/infrablockchain_global_property_object.hpp>

namespace eosio { namespace chain {

   namespace detail {

      using namespace infrablockchain::chain;

      snapshot_infrablockchain_global_property_object
      snapshot_row_traits<infrablockchain_global_property_object>::to_snapshot_row( const infrablockchain_global_property_object& value,
                                                                                    const chainbase::database& db )
      {
         snapshot_infrablockchain_global_property_object res;

         res.system_token_list = value.system_token_list.to_system_token_list();
         res.total_tx_votes_weighted = value.total_tx_votes_weighted;
         res.total_tx_votes = value.total_tx_votes;
         res.reserved1 = value.reserved1;
         res.reserved2 = value.reserved2;

         return res;
      }

      void
      snapshot_row_traits<infrablockchain_global_property_object>::from_snapshot_row( snapshot_infrablockchain_global_property_object&& row,
                                                                                      infrablockchain_global_property_object& value,
                                                                                      chainbase::database& db )
      {
         value.system_token_list = row.system_token_list;
         value.total_tx_votes_weighted = row.total_tx_votes_weighted;
         value.total_tx_votes = row.total_tx_votes;
         value.reserved1 = row.reserved1;
         value.reserved2 = row.reserved2;
      }

   }

}}
