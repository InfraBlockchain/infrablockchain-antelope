/**
 *  @file contracts/yx.system/yx.system_token.cpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */

#include "yx.system.hpp"

#include <infrablockchainlib/token_api.h>
#include <infrablockchainlib/system_token_api.h>

#include <eosiolib/symbol.hpp>
#include <eosiolib/memory.hpp>
#include <eosiolib/datastream.hpp>

namespace yosemitesys {

   using namespace eosio;

   void system_contract::addsystoken( const account_name token, const uint32_t weight ) {

      // need 2/3+ block producer's signatures (yosemite@active permission required)
      require_auth( _self );

      eosio_assert( symbol_type(get_token_symbol(token)).is_valid(), "invalid token account" );

      vector<yosemite_system_token> new_system_token_list;
      bool already_sys_token = false;

      size_t sys_token_count = get_system_token_count();
      if ( sys_token_count > 0 ) {
         uint32_t sys_token_list_packed_size = get_system_token_list( nullptr, 0 );
         if ( sys_token_list_packed_size > 0 ) {
            /// allocate buffer memory through memory manager
            char* buf_sys_token_list_data_packed = static_cast<char*>(malloc(sys_token_list_packed_size));
            eosio_assert( buf_sys_token_list_data_packed != nullptr, "malloc failed for buf_sys_token_list_data_packed" );
            get_system_token_list( buf_sys_token_list_data_packed, sys_token_list_packed_size );

            vector<yosemite_system_token> current_system_tokens;
            eosio::datastream<const char*> ds( buf_sys_token_list_data_packed, sys_token_list_packed_size );
            ds >> current_system_tokens;

            for ( const auto& sys_token : current_system_tokens ) {
               if ( sys_token.token_id == token ) {
                  already_sys_token = true;
                  new_system_token_list.push_back( yosemite_system_token{token, weight} );
               } else {
                  new_system_token_list.push_back( sys_token );
               }
            }

            free(buf_sys_token_list_data_packed);
         }
      }

      if ( !already_sys_token ) {
         new_system_token_list.push_back( yosemite_system_token{token, weight} );
      }

      bytes packed_system_token_list = pack(new_system_token_list);

      set_system_token_list( packed_system_token_list.data(), packed_system_token_list.size() );
   }

   void system_contract::rmvsystoken( const account_name token ) {

      // need 2/3+ block producer's signatures (yosemite@active permission required)
      require_auth( _self );

      eosio_assert( symbol_type(get_token_symbol(token)).is_valid(), "invalid token account" );

      vector<yosemite_system_token> new_system_token_list;
      bool removed = false;

      size_t sys_token_count = get_system_token_count();
      if ( sys_token_count > 0 ) {
         uint32_t sys_token_list_packed_size = get_system_token_list( nullptr, 0 );
         if ( sys_token_list_packed_size > 0 ) {
            /// allocate buffer memory through memory manager
            char* buf_sys_token_list_data_packed = static_cast<char*>(malloc(sys_token_list_packed_size));
            eosio_assert( buf_sys_token_list_data_packed != nullptr, "malloc failed for buf_sys_token_list_data_packed" );
            get_system_token_list( buf_sys_token_list_data_packed, sys_token_list_packed_size );

            vector<yosemite_system_token> current_system_tokens;
            eosio::datastream<const char*> ds( buf_sys_token_list_data_packed, sys_token_list_packed_size );
            ds >> current_system_tokens;

            for ( const auto& sys_token : current_system_tokens ) {
               if ( sys_token.token_id == token ) {
                  removed = true;
               } else {
                  new_system_token_list.push_back( sys_token );
               }
            }

            new_system_token_list.insert( new_system_token_list.end(), current_system_tokens.begin(), current_system_tokens.end() );

            free(buf_sys_token_list_data_packed);
         }
      }

      eosio_assert( removed, "token is not currently system token" );

      bytes packed_system_token_list = pack(new_system_token_list);

      set_system_token_list( packed_system_token_list.data(), packed_system_token_list.size() );
   }

} //namespace yosemitesys
