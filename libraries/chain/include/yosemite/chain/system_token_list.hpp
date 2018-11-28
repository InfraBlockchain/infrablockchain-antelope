/**
 *  @file yosemite/chain/system_token_list.hpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosio/chain/types.hpp>
#include <chainbase/chainbase.hpp>

namespace yosemite { namespace chain {

   using namespace eosio::chain;

   using system_token_id_type = account_name;

   /**
    * system token is a authorized token used as transaction fee payment.
    * to be selected as a system token, a token should meet below criteria.
    *  - token account should be ranked within top n tokens by acquiring enough transaction votes (YOSEMITE Proof-of-Transaction / Transaction-as-a-Vote)
    *  - the token should be granted as system token used as a transaction fee token signed by (2/3)+ block producers
    */
   struct system_token {
      system_token_id_type  system_token_id; // token account name selected as system token
      uint32_t              tx_fee_multiplier; // multiplier applied to tx fee amount, 10000 = 1.0x

      static constexpr uint32_t tx_fee_multiplier_1x = 10000;

      friend bool operator == ( const system_token& lhs, const system_token& rhs ) {
         return tie( lhs.system_token_id, lhs.tx_fee_multiplier ) == tie( rhs.system_token_id, rhs.tx_fee_multiplier );
      }

      friend bool operator != (const system_token& a, const system_token& b) { return !(a == b); }

//      friend bool operator < (const system_token& a, const system_token& b)
//      {
//         return std::tie(a.system_token_id,a.tx_fee_multiplier) < std::tie(b.system_token_id,b.tx_fee_multiplier);
//      }
//
//      friend bool operator <= (const system_token& a, const system_token& b) { return (a == b) || (a < b); }
//
//      friend bool operator > (const system_token& a, const system_token& b)  { return !(a <= b); }
//
//      friend bool operator >= (const system_token& a, const system_token& b) { return !(a < b);  }
   };

   /**
    * defines the order of system tokens, system token list version
    */
   struct system_token_list_type {
      uint32_t              version = 0; ///< sequentially incrementing version number
      vector<system_token>  system_tokens;
   };

   struct shared_system_token_list_type {
      shared_system_token_list_type( chainbase::allocator<char> alloc )
      :system_tokens(alloc) {}

      shared_system_token_list_type& operator=( const system_token_list_type& a ) {
         version = a.version;
         system_tokens.clear();
         system_tokens.reserve( a.system_tokens.size() );
         for( const auto& sys_token : a.system_tokens ) {
            system_tokens.push_back(sys_token);
         }
         return *this;
      }

      operator system_token_list_type()const {
         system_token_list_type result;
         result.version = version;
         result.system_tokens.reserve(system_tokens.size());
         for( const auto& sys_token : system_tokens ) {
            result.system_tokens.push_back(sys_token);
         }
         return result;
      }

      uint32_t get_transaction_fee_multiplier(system_token_id_type token_id) {
         for( const auto& sys_token : system_tokens ) {
            if( sys_token.system_token_id == token_id ) {
               return sys_token.tx_fee_multiplier;
            }
         }
         return 0;
      }

      void clear() {
         version = 0;
         system_tokens.clear();
      }

      uint32_t                     version = 0; ///< sequentially incrementing version number
      shared_vector<system_token>  system_tokens;
   };

   inline bool operator == ( const system_token_list_type& a, const system_token_list_type& b )
   {
      if( a.version != b.version ) return false;
      if ( a.system_tokens.size() != b.system_tokens.size() ) return false;
      for( uint32_t i = 0; i < a.system_tokens.size(); ++i )
         if( a.system_tokens[i] != b.system_tokens[i] ) return false;
      return true;
   }

   inline bool operator != ( const system_token_list_type& a, const system_token_list_type& b )
   {
      return !(a==b);
   }

} } /// yosemite::chain

FC_REFLECT( yosemite::chain::system_token, (system_token_id)(tx_fee_multiplier) )
FC_REFLECT( yosemite::chain::system_token_list_type, (version)(system_tokens) )
FC_REFLECT( yosemite::chain::shared_system_token_list_type, (version)(system_tokens) )
