/**
 *  @file yosemite/chain/received_transaction_votes_manager.hpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <yosemite/chain/transaction_as_a_vote.hpp>
#include <yosemite/chain/received_transaction_votes_database.hpp>

#include <eosio/chain/types.hpp>
#include <eosio/chain/snapshot.hpp>
#include <eosio/chain/transaction_context.hpp>

#include <chainbase/chainbase.hpp>

namespace yosemite { namespace chain {

   using namespace eosio::chain;

   struct transaction_vote_receiver {
      transaction_vote_receiver( account_name a, tx_votes_sum_weighted_type wv, tx_votes_sum_type v )
      : account(a), tx_votes_weighted(wv), tx_votes(v) {}

      account_name                account;
      tx_votes_sum_weighted_type  tx_votes_weighted;
      tx_votes_sum_type           tx_votes;
   };

   class transaction_vote_stat_manager {
   public:
      explicit transaction_vote_stat_manager( chainbase::database &db );

      void add_indices();
      void initialize_database();
      void add_to_snapshot( const snapshot_writer_ptr &snapshot ) const;
      void read_from_snapshot( const snapshot_reader_ptr &snapshot );

      void add_transaction_vote_to_target_account( transaction_context& context, const account_name vote_target_account, const transaction_vote_amount_type tx_vote_amount );

      transaction_vote_receiver get_transaction_vote_stat_for_account( const transaction_vote_to_name_type vote_target_account ) const;

      vector<transaction_vote_receiver> get_top_sorted_transaction_vote_receivers(const uint32_t offset, const uint32_t size) const;

   private:
      double weighted_tx_vote_time_decayed( uint32_t now, uint32_t vote );

      chainbase::database &_db;
   };

} } /// yosemite::chain

FC_REFLECT(yosemite::chain::transaction_vote_receiver, (account)(tx_votes_weighted)(tx_votes))
