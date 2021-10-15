/**
 *  @file chain/webassembly/infrablockchain/transaction_fee_management_intrinsics.cpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */

#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/chain/apply_context.hpp>

namespace eosio { namespace chain { namespace webassembly {

   ///////////////////////////////////////////////////////////////
   /// InfraBlockchain Proof-of-Transaction Core API (Intrinsics)

   /**
    * Get Top Transaction Vote Receivers
    * @brief Retrieve top transaction vote receiver list from blockchain core.
    *        Transaction votes are processed and accrued for each vote target account on blockchain core by InfraBlockchain Proof-of-Transaction/Transaction-as-a-Vote protocol
    *        Smart contract code including system contract can retrieve the top transaction vote receiver list
    *        sorted by the accumulated time-decaying weighted transaction vote amount for each tx vote receiver account.
    *        The whole list of transaction vote receivers can be arbitrarily long,
    *        so the sorted list can be retrieved by multiple function call with different offset_rank and limit parameter values.
    *
    * @param[out] packed_vote_receivers - output buffer of the top sorted transaction vote receiver list (vector<infrablockchain::chain::tx_vote_stat_for_account>),
    *                                    output data retrieved only if the output buffer has sufficient size to hold the packed data.
    * @param offset_rank - offset-rank of first item in the returned list. offset-rank n means the returned list starts from the rank n+1 tx vote receiver.
    *                      e.g. if offset_rank = 0, the first item in the returned list is the top 1 vote receiver.
    * @param limit - max limit of the returned item count
    *
    * @return the size of the sorted transaction vote receiver list data written to the buffer (array of 'infrablockchain_tx_vote_stat_for_account' struct),
    *         or number of bytes required if the output buffer is empty.
    */
   uint32_t interface::get_top_transaction_vote_receivers_packed( legacy_span<char> packed_vote_receiver_list, uint32_t offset_rank, uint32_t limit ) const {
      auto trx_vote_receivers = context.get_top_transaction_vote_receivers( offset_rank, limit );

      auto s = fc::raw::pack_size( trx_vote_receivers );
      if( packed_vote_receiver_list.size() == 0 ) return s;

      if ( s <= packed_vote_receiver_list.size() ) {
         datastream<char*> ds( packed_vote_receiver_list.data(), s );
         fc::raw::pack(ds, trx_vote_receivers);
         return s;
      }
      return 0;
   }

   /**
    * Get Total Weighted Transaction Votes
    * @brief Retrieve the total weighted (time-decaying) transaction vote amount accumulated for all blockchain transactions in the blockchain history
    *
    * @return total weighted transaction vote amount
    */
   double interface::get_total_weighted_transaction_votes() const {
      return context.get_total_weighted_transaction_votes();
   }

} } } /// eosio::chain::webassembly
