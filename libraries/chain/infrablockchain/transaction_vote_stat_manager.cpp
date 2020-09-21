/**
 *  @file chain/infrablockchain/transaction_vote_stat_manager.cpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */

#include <infrablockchain/chain/transaction_vote_stat_manager.hpp>
#include <infrablockchain/chain/softfloat64.hpp>
#include <infrablockchain/chain/infrablockchain_global_property_database.hpp>
#include <infrablockchain/chain/transaction_as_a_vote.hpp>

#include <eosio/chain/database_utils.hpp>

namespace infrablockchain { namespace chain {

   using namespace eosio::chain;

   using transaction_vote_stat_db_index_set = index_set<
      received_transaction_votes_multi_index
   >;

   transaction_vote_stat_manager::transaction_vote_stat_manager( chainbase::database& db )
      : _db(db) {
   }

   void transaction_vote_stat_manager::add_indices() {
      transaction_vote_stat_db_index_set::add_indices(_db);
   }

   void transaction_vote_stat_manager::initialize_database() {

   }

   void transaction_vote_stat_manager::add_to_snapshot( const snapshot_writer_ptr& snapshot ) const {
      transaction_vote_stat_db_index_set::walk_indices([this, &snapshot]( auto utils ){
         snapshot->write_section<typename decltype(utils)::index_t::value_type>([this]( auto& section ){
            decltype(utils)::walk(_db, [this, &section]( const auto &row ) {
               section.add_row(row, _db);
            });
         });
      });
   }

   void transaction_vote_stat_manager::read_from_snapshot( const snapshot_reader_ptr& snapshot ) {
      transaction_vote_stat_db_index_set::walk_indices([this, &snapshot]( auto utils ){
         snapshot->read_section<typename decltype(utils)::index_t::value_type>([this]( auto& section ) {
            bool more = !section.empty();
            while(more) {
               decltype(utils)::create(_db, [this, &section, &more]( auto &row ) {
                  more = section.read_row(row, _db);
               });
            }
         });
      });
   }

   const int64_t   __tx_vote_weight_timestamp_epoch__ = 1514764800ll;  // 01/01/2018 @ 12:00am (UTC)
   const uint32_t  __seconds_per_week__ = 24 * 3600 * 7;

   double transaction_vote_stat_manager::weighted_tx_vote_time_decayed( uint32_t curren_block_time_sec, uint32_t vote ) {
      float64_t weight = f64_div( i64_to_f64(int64_t( curren_block_time_sec - __tx_vote_weight_timestamp_epoch__ )), i64_to_f64( __seconds_per_week__ * 52 ) );
      return from_softfloat64( f64_mul( ui32_to_f64(vote), softfloat64::pow( ui32_to_f64(2), weight ) ) );
   }

   void transaction_vote_stat_manager::add_transaction_vote_to_target_account( transaction_context& context, const transaction_vote_to_name_type vote_target_account, const transaction_vote_amount_type tx_vote_amount ) {

      if (tx_vote_amount == 0 || tx_vote_amount > INFRABLOCKCHAIN_MAX_TRANSACTION_FEE_AMOUNT_PER_TRANSACTION) {
         return;
      }

      uint32_t curren_block_time_sec = (uint32_t)( static_cast<uint64_t>( context.control.pending_block_time().time_since_epoch().count() ) / 1000000 );
      tx_votes_sum_weighted_type weighted_tx_vote_amount = weighted_tx_vote_time_decayed( curren_block_time_sec, tx_vote_amount );

      auto* tx_votes_ptr = _db.find<received_transaction_votes_object, by_tx_votes_account>( vote_target_account );
      if ( tx_votes_ptr ) {
         _db.modify<received_transaction_votes_object>( *tx_votes_ptr, [&]( received_transaction_votes_object& tx_votes_obj ) {
            tx_votes_obj.tx_votes_weighted = softfloat64::add( tx_votes_obj.tx_votes_weighted, weighted_tx_vote_amount );
            tx_votes_obj.tx_votes += tx_vote_amount;
         });
      } else {
         _db.create<received_transaction_votes_object>( [&](received_transaction_votes_object& tx_votes_obj) {
            tx_votes_obj.account = vote_target_account;
            tx_votes_obj.tx_votes_weighted = weighted_tx_vote_amount;
            tx_votes_obj.tx_votes = tx_vote_amount;
         });
      }

      auto& ygpo = _db.get<infrablockchain_global_property_object>();
      _db.modify( ygpo, [&]( auto& ygp ) {
         ygp.total_tx_votes += tx_vote_amount;
         ygp.total_tx_votes_weighted = softfloat64::add( ygp.total_tx_votes_weighted, weighted_tx_vote_amount );
      });
   }

   tx_votes_sum_weighted_type transaction_vote_stat_manager::get_total_weighted_transaction_vote_amount() const {
      return _db.get<infrablockchain_global_property_object>().total_tx_votes_weighted;
   }

   tx_vote_stat_for_account transaction_vote_stat_manager::get_transaction_vote_stat_for_account( const transaction_vote_to_name_type vote_target_account ) const {

      auto* tx_votes_ptr = _db.find<received_transaction_votes_object, by_tx_votes_account>( vote_target_account );
      if ( tx_votes_ptr ) {
         return tx_vote_stat_for_account( vote_target_account, (*tx_votes_ptr).tx_votes_weighted, (*tx_votes_ptr).tx_votes );
      } else {
         return tx_vote_stat_for_account( vote_target_account, 0.0, 0 );
      }
   }

   tx_vote_receiver_list_result transaction_vote_stat_manager::get_top_sorted_transaction_vote_receivers( const uint32_t offset_rank, const uint32_t limit, const bool retrieve_total_votes ) const {

      tx_vote_receiver_list_result result;
      vector<tx_vote_stat_for_account>& tx_vote_receivers = result.tx_vote_receiver_list;
      tx_vote_receivers.reserve( limit );

      const auto& idx = _db.get_index<received_transaction_votes_multi_index, by_tx_votes_weighted>();
      auto itr = idx.rbegin();
      auto itr_rend = idx.rend();

      uint32_t skip_cnt = 0;

      while ( itr != itr_rend && tx_vote_receivers.size() < limit ) {
         if ( skip_cnt == offset_rank ) {
            tx_vote_receivers.emplace_back( itr->account, itr->tx_votes_weighted, itr->tx_votes );
         } else {
            skip_cnt++;
         }
         itr++;
      }

      if (itr != itr_rend) {
         result.more = true;
      }

      if ( retrieve_total_votes ) {
         auto& ygpo = _db.get<infrablockchain_global_property_object>();
         result.total_tx_votes_weighted = ygpo.total_tx_votes_weighted;
         result.total_tx_votes = ygpo.total_tx_votes;
      }

      return result;
   }


} } // namespace infrablockchain::chain
