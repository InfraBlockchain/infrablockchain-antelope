/**
 *  @file chain/infrablockchain/standard_token_object.cpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */

#include <infrablockchain/chain/standard_token_object.hpp>

namespace eosio { namespace chain {

   namespace detail {

      using namespace infrablockchain::chain;

      snapshot_token_meta_object
      snapshot_row_traits<token_meta_object>::to_snapshot_row( const token_meta_object& value,
                                                               const chainbase::database& db )
      {
         snapshot_token_meta_object res;

         res.token_id = value.token_id;
         res.sym = value.sym;
         res.total_supply = value.total_supply;
         res.url = std::string(value.url.data(), value.url.size());
         res.desc = std::string(value.desc.data(), value.desc.size());

         return res;
      }

      void
      snapshot_row_traits<token_meta_object>::from_snapshot_row( snapshot_token_meta_object&& row,
                                                                 token_meta_object& value,
                                                                 chainbase::database& db )
      {
         value.token_id = row.token_id;
         value.sym = row.sym;
         value.total_supply = row.total_supply;
         value.url.assign( row.url.data(), row.url.size() );
         value.desc.assign( row.desc.data(), row.desc.size() );
      }

   }

}}
