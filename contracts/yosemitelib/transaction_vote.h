/**
 *  @file
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosiolib/types.h>

extern "C" {

   /**
    *  @defgroup YOSEMITE Transaction Vote API
    *  @ingroup contractdev
    *  @brief Defines API for Transaction-as-a-Vote implementing YOSEMITE Proof-of-Transaction
    */

   /**
    *  @defgroup YOSEMITE Transaction Vote C API
    *  @ingroup trxvoteapi
    *  @brief Defines API for Transaction-as-a-Vote implementing YOSEMITE Proof-of-Transaction
    *  @{
    */


   /**
    *  Cast Transaction Vote
    *  @brief Contribute transaction voting from an action in current transaction context.
    *         The vote amount is accumulated to the vote-candidate account specified in the current transaction context.
    *         Transaction votes from all the actions in a transaction are cast to one vote-candidate account.
    *         Only system contracts on privileged account can call transaction vote API
    *
    *
    *  @param vote_amount - Transaction vote amount (usually, the transaction fee amount paid to transaction fee pool from current action
    */

   void cast_transaction_vote( uint32_t vote_amount );

   /**
    * Get transaction vote data accumulated in the head block (previous block)
    * @brief Get transaction votes sum data for each vote-candidate account,
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

///@ } trxvoteapi
}

struct yosemite_transaction_vote {
    uint64_t candidate_name;
    uint32_t vote_amount;
};
