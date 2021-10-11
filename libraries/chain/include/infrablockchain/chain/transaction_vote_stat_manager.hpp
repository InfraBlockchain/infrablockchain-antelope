/**
 *  @file infrablockchain/chain/transaction_vote_stat_manager.hpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */
#pragma once

#include <eosio/chain/types.hpp>
#include <eosio/chain/snapshot.hpp>
#include <eosio/chain/transaction_context.hpp>
#include <chainbase/chainbase.hpp>

#include <infrablockchain/chain/transaction_as_a_vote.hpp>
#include <infrablockchain/chain/received_transaction_votes_object.hpp>

namespace infrablockchain { namespace chain {

   using namespace eosio::chain;

   struct tx_vote_stat_for_account {
      tx_vote_stat_for_account( account_name a, tx_votes_sum_weighted_type wv, tx_votes_sum_type v )
      : account(a), tx_votes_weighted(wv), tx_votes(v) {}

      account_name                account;
      tx_votes_sum_weighted_type  tx_votes_weighted;
      tx_votes_sum_type           tx_votes;
   };

   struct tx_vote_receiver_list_result {
      vector<tx_vote_stat_for_account>  tx_vote_receiver_list;
      tx_votes_sum_weighted_type        total_tx_votes_weighted = 0.0;
      tx_votes_sum_type                 total_tx_votes = 0;
      bool more = false;
   };

   class transaction_vote_stat_manager {
   public:
      explicit transaction_vote_stat_manager( chainbase::database &db );

      void add_indices();
      void initialize_database();
      void add_to_snapshot( const snapshot_writer_ptr &snapshot ) const;
      void read_from_snapshot( const snapshot_reader_ptr &snapshot );

      void add_transaction_vote_amount_to_target_account( transaction_context& context, const account_name vote_target_account, const transaction_vote_amount_type tx_vote_amount );

      tx_votes_sum_weighted_type get_total_weighted_transaction_vote_amount() const;

      tx_vote_stat_for_account get_transaction_vote_stat_for_account( const transaction_vote_target_name_type vote_target_account ) const;

      tx_vote_receiver_list_result get_top_sorted_transaction_vote_receivers( const uint32_t offset_rank, const uint32_t limit, const bool retrieve_total_votes ) const;

   private:
      double weighted_tx_vote_time_decayed( uint32_t curren_block_time_sec, uint32_t vote );

      chainbase::database &_db;
   };

} } /// infrablockchain::chain

FC_REFLECT(infrablockchain::chain::tx_vote_stat_for_account, (account)(tx_votes_weighted)(tx_votes))
FC_REFLECT(infrablockchain::chain::tx_vote_receiver_list_result, (tx_vote_receiver_list)(total_tx_votes_weighted)(total_tx_votes)(more))
