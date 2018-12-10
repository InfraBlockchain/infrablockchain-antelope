/**
 *  @file yosemite/chain/standard_token_manager.hpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosio/chain/types.hpp>
#include <eosio/chain/snapshot.hpp>
#include <eosio/chain/trace.hpp>
#include <eosio/chain/transaction_context.hpp>
#include <eosio/chain/apply_context.hpp>
#include <eosio/chain/symbol.hpp>
#include <chainbase/chainbase.hpp>

#include <yosemite/chain/standard_token_action_types.hpp>
#include <yosemite/chain/standard_token_database.hpp>

namespace yosemite { namespace chain {

   using namespace eosio::chain;

   class standard_token_manager {
   public:
      explicit standard_token_manager( chainbase::database &db );

      void add_indices();
      void initialize_database();
      void add_to_snapshot( const snapshot_writer_ptr &snapshot ) const;
      void read_from_snapshot( const snapshot_reader_ptr &snapshot );

      void set_token_meta_info( apply_context& context, const token_id_type &token_id, const token::settokenmeta &token_meta );
      const token_meta_object* get_token_meta_info( const token_id_type& token_id ) const;
      const symbol& get_token_symbol( const token_id_type& token_id ) const;
      const share_type& get_token_total_supply( const token_id_type& token_id ) const;
      void update_token_total_supply( const token_meta_object* token_meta_ptr, share_type delta );

      void add_token_balance( apply_context& context, token_id_type token_id, account_name owner, share_type value );
      void subtract_token_balance( apply_context& context, token_id_type token_id, account_name owner, share_type value );
      share_type get_token_balance( const token_id_type& token_id, const account_name& account ) const;

      void pay_transaction_fee( transaction_context& trx_context, account_name fee_payer, uint32_t fee_amount );

   private:
      chainbase::database &_db;
   };

} } /// yosemite::chain