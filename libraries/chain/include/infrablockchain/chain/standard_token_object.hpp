/**
 *  @file infrablockchain/chain/standard_token_object.hpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */
#pragma once

#include <eosio/chain/database_utils.hpp>
#include <eosio/chain/multi_index_includes.hpp>
#include <eosio/chain/symbol.hpp>
#include <eosio/chain/config.hpp>

namespace infrablockchain { namespace chain {

   using namespace eosio::chain;

   using token_id_type = account_name;

   /**
    * @brief token information object
    */
   class token_meta_object : public chainbase::object<infrablockchain_token_meta_object_type, token_meta_object> {
      OBJECT_CTOR(token_meta_object, (url)(desc))

      id_type        id;
      token_id_type  token_id; // token id = token account name
      symbol         sym; // symbol name and precision
      share_type     total_supply = 0; // total token supply amount issued(+) and redeemed(-) by token account
      shared_string  url; // website url for token information managed by token issuer (owner of token account)
      shared_string  desc; // token description
   };

   struct by_token_id;

   using token_meta_multi_index = chainbase::shared_multi_index_container<
      token_meta_object,
      indexed_by<
         ordered_unique< tag<by_id>, member<token_meta_object, token_meta_object::id_type, &token_meta_object::id> >,
         ordered_unique< tag<by_token_id>, member<token_meta_object, token_id_type, &token_meta_object::token_id> >
      >
   >;


   /**
    * @brief token balance object for each account holding a specific token
    */
   class token_balance_object : public chainbase::object<infrablockchain_token_balance_object_type, token_balance_object> {
      OBJECT_CTOR(token_balance_object)

      id_type        id;
      token_id_type  token_id;
      account_name   account;
      share_type     balance;
   };

   struct by_token_account;

   using token_balance_multi_index = chainbase::shared_multi_index_container<
      token_balance_object,
      indexed_by<
         ordered_unique< tag<by_id>, member<token_balance_object, token_balance_object::id_type, &token_balance_object::id> >,
         ordered_unique< tag<by_token_account>,
            composite_key< token_balance_object,
               member<token_balance_object, token_id_type, &token_balance_object::token_id>,
               member<token_balance_object, account_name, &token_balance_object::account>
            >
         >
      >
   >;

} } /// infrablockchain::chain

namespace eosio { namespace chain { namespace config {

   template<>
   struct billable_size<infrablockchain::chain::token_meta_object> {
      static const uint64_t overhead = overhead_per_row_per_index_ram_bytes * 2;  ///< overhead for 2x indices internal-key(id) and token_id
      static const uint64_t value = 32 + 2*(8+4+256) + overhead; ///< 32 bytes for constant size fields + 2 x upto-256-string(including 8 for pointer to vector data, 4 for size of vector) + overhead
   };

   template<>
   struct billable_size<infrablockchain::chain::token_balance_object> {
      static const uint64_t overhead = overhead_per_row_per_index_ram_bytes * 2;  ///< overhead for 2x indices internal-key(id) and token_id
      static const uint64_t value = 32 + overhead; ///< 32 bytes for constant size fields + overhead
   };

} } } /// eosio::chain::config


CHAINBASE_SET_INDEX_TYPE(infrablockchain::chain::token_meta_object, infrablockchain::chain::token_meta_multi_index)
CHAINBASE_SET_INDEX_TYPE(infrablockchain::chain::token_balance_object, infrablockchain::chain::token_balance_multi_index)

FC_REFLECT(infrablockchain::chain::token_meta_object, (token_id)(sym)(total_supply)(url)(desc) )
FC_REFLECT(infrablockchain::chain::token_balance_object, (token_id)(account)(balance) )
