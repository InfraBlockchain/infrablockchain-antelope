/**
 *  @file infrablockchain/chain/system_token_list.hpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */
#pragma once

#include <eosio/chain/types.hpp>
#include <chainbase/chainbase.hpp>

namespace infrablockchain { namespace chain {

   using namespace eosio::chain;

   using system_token_id_type = account_name;

   const static int max_system_tokens = 25;

   /**
    * system token is a authorized token used as transaction fee payment.
    * to be selected as a system token, a token should meet below criteria.
    *  - the token should be granted as system token used as a transaction fee token signed by (2/3)+ block producers
    *  - (optional, currently not implemented) token account should be ranked within top n tokens by acquiring enough transaction votes (InfraBlockchain Proof-of-Transaction / Transaction-as-a-Vote)
    */
   struct system_token {
      system_token_id_type  token_id; // token account name selected as system token
      uint32_t              token_weight; // token value weight as transaction fee payment, 10000 = 1.0x, 5000 = 0.5x (tx fee is charged 2x)

      bool valid() const {
         return token_id.good() && token_weight != 0;
      }

      static constexpr uint32_t token_weight_1x = 10000;

      friend bool operator == ( const system_token& lhs, const system_token& rhs ) {
         return std::tie( lhs.token_id, lhs.token_weight ) == std::tie( rhs.token_id, rhs.token_weight );
      }

      friend bool operator != (const system_token& a, const system_token& b) { return !(a == b); }
   };

   /**
    * defines the order of system tokens, system token list version
    */
   struct system_token_list {
      uint32_t              version = 0; ///< sequentially incrementing version number
      vector<system_token>  system_tokens;
   };

   struct shared_system_token_list {
      shared_system_token_list( chainbase::allocator<char> alloc )
      :system_tokens(alloc) {}

      shared_system_token_list& operator=( const system_token_list& a ) {
         version = a.version;
         system_tokens = shared_vector<system_token>(a.system_tokens.begin(), a.system_tokens.end(), system_tokens.get_allocator());
         return *this;
      }

      operator system_token_list()const {
         system_token_list result;
         result.version = version;
         result.system_tokens.reserve(system_tokens.size());
         for( const auto& sys_token : system_tokens ) {
            result.system_tokens.emplace_back( sys_token );
         }
         return result;
      }

      uint32_t get_token_weight(system_token_id_type token_id) {
         for( const auto& sys_token : system_tokens ) {
            if( sys_token.token_id == token_id ) {
               return sys_token.token_weight;
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

   inline bool operator == ( const system_token_list& a, const system_token_list& b )
   {
      if( a.version != b.version ) return false;
      if ( a.system_tokens.size() != b.system_tokens.size() ) return false;
      for( uint32_t i = 0; i < a.system_tokens.size(); ++i )
         if( a.system_tokens[i] != b.system_tokens[i] ) return false;
      return true;
   }

   inline bool operator != ( const system_token_list& a, const system_token_list& b )
   {
      return !(a==b);
   }

} } /// infrablockchain::chain

FC_REFLECT( infrablockchain::chain::system_token, (token_id)(token_weight) )
FC_REFLECT( infrablockchain::chain::system_token_list, (version)(system_tokens) )
FC_REFLECT( infrablockchain::chain::shared_system_token_list, (version)(system_tokens) )
