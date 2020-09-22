/**
 *  @file
 *  @copyright defined in infrablockchain/LICENSE.txt
 */
#pragma once

#include <eosiolib/types.h>

extern "C" {

   /**
    *  @defgroup InfraBlockchain Transaction Vote API
    *  @ingroup contractdev
    *  @brief Defines API for Transaction-as-a-Vote implementing InfraBlockchain Proof-of-Transaction
    */

   /**
    *  @defgroup InfraBlockchain Transaction Vote C API
    *  @ingroup trxvoteapi
    *  @brief Defines API for Transaction-as-a-Vote implementing InfraBlockchain Proof-of-Transaction
    *  @{
    */


   /**
    *  [Deprecated]
    *  Cast Transaction Vote
    *  @brief Contribute transaction voting from an action in current transaction context.
    *         The vote amount is accumulated to the vote-to(candidate) account specified in the current transaction context.
    *         Transaction votes from all the actions in a transaction are cast to one vote-to(candidate) account.
    *         Only system contracts on privileged account can call transaction vote API
    *
    *
    *  @param vote_amount - Transaction vote amount (usually, the transaction fee amount paid to transaction fee pool from current action
    */

   void cast_transaction_vote( uint32_t vote_amount );

   /**
    * [Deprecated]
    * Get transaction vote data accumulated in the head block (previous block)
    * @brief Get transaction votes sum data for each vote-to(candidate) account,
    *        accumulated from the transactions in the previous block right before current pending block
    *
    * @param buffer - a buffer to write the accumulated transaction votes data to
    * @param size - the size of the buffer, 0 to return required size
    *
    * @return the size of the transaction votes data written to the buffer, or number of bytes that can be copied if size==0 passed
    * @pre `buffer` is a valid pointer to a range of memory at least `size` bytes long
    * @post `buffer` is filled with packed transaction votes data
    */
   size_t read_head_block_trx_votes_data(char *buffer, size_t size);


   /**
    * Get Top Transaction Vote Receivers
    * @brief Retrieve top transaction vote receiver list from blockchain core.
    *        Transaction votes are processed and accrued for each vote target account on blockchain core by InfraBlockchain Proof-of-Transaction/Transaction-as-a-Vote protocol
    *        Smart contract code including system contract can retrieve the top transaction vote receiver list
    *        sorted by the accumulated time-decaying weighted transaction vote amount for each tx vote receiver account.
    *        The whole list of transaction vote receivers can be arbitrarily long,
    *        so the sorted list can be retrieved by multiple function call with different offset_rank and limit parameter values.
    *
    * @param buffer - a buffer to write the top sorted transaction vote receiver list data to,
    *                 returned buffer memory contains array of 'infrablockchain_tx_vote_stat_for_account' struct
    * @param size - the size of the buffer, 0 to return required size
    * @param offset_rank - offset-rank of first item in the returned list. offset-rank n means the returned list starts from the rank n+1 tx vote receiver.
    *                      e.g. if offset_rank = 0, the first item in the returned list is the top 1 vote receiver.
    * @param limit - max limit of the returned item count
    *
    * @return the size of the sorted transaction vote receiver list data written to the buffer (array of 'infrablockchain_tx_vote_stat_for_account' struct),
    *         or number of bytes that can be copied if size==0 passed
    * @pre `buffer` is a valid pointer to a range of memory at least `size` bytes long
    * @post `buffer` is filled with packed transaction vote receiver list data
    */
   size_t get_top_transaction_vote_receivers( char *buffer, size_t size, uint32_t offset_rank, uint32_t limit );

   /**
    * Get Total Weighted Transaction Votes
    * @brief Retrieve the total weighted (time-decaying) transaction vote amount accumulated for all blockchain transactions in the blockchain history
    *
    * @return total weighted transaction vote amount
    */
   double get_total_weighted_transaction_votes();


///@ } trxvoteapi
}

struct infrablockchain_transaction_vote {
    uint64_t to; // account name
    uint32_t amount;
};

struct infrablockchain_tx_vote_stat_for_account {
   uint64_t  account;
   double    tx_votes_weighted;
   uint64_t  tx_votes;
};
