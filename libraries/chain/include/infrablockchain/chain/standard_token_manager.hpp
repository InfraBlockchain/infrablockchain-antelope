/**
 *  @file infrablockchain/chain/standard_token_manager.hpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */
#pragma once

#include <eosio/chain/types.hpp>
#include <eosio/chain/snapshot.hpp>
#include <eosio/chain/trace.hpp>
#include <eosio/chain/transaction_context.hpp>
#include <eosio/chain/apply_context.hpp>
#include <eosio/chain/symbol.hpp>
#include <chainbase/chainbase.hpp>

#include <infrablockchain/chain/standard_token_action_types.hpp>
#include <infrablockchain/chain/standard_token_database.hpp>
#include <infrablockchain/chain/system_token_list.hpp>

namespace infrablockchain { namespace chain {

   using namespace eosio::chain;

   struct token_meta_info {
      token_meta_info() {}
      token_meta_info( name t, symbol s, asset ts, const char* u_cstr, size_t u_size, const char* d_cstr, size_t d_size)
      : token_id(t), sym(s), total_supply(ts), url(u_cstr, u_size), desc(d_cstr, d_size) {}

      name   token_id;
      symbol sym;
      asset  total_supply;
      std::string url;
      std::string desc;
   };

   struct token_balance {
      token_balance(){}
      token_balance( name n, asset a ):t(n),qty(a){}
      name  t; // token id
      asset qty; // quantity
   };

   struct system_token_balance {
      asset total;
      vector<token_balance> sys_tokens;
   };

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

      int64_t set_system_token_list( vector<system_token> system_tokens );
      int get_system_token_count() const;
      vector<system_token> get_system_token_list() const;

      system_token_balance get_system_token_balance( const account_name& account ) const;

   private:
      chainbase::database &_db;
   };

} } /// infrablockchain::chain

FC_REFLECT(infrablockchain::chain::token_meta_info, (token_id)(sym)(total_supply)(url)(desc) )
FC_REFLECT(infrablockchain::chain::token_balance, (t)(qty) )
FC_REFLECT(infrablockchain::chain::system_token_balance, (total)(sys_tokens) )
