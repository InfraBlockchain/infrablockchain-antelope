/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <yosemite/chain/transaction_as_a_vote.hpp>
#include <eosio/chain/block_header_state.hpp>
#include <eosio/chain/block.hpp>
#include <eosio/chain/transaction_metadata.hpp>
#include <eosio/chain/action_receipt.hpp>

namespace eosio { namespace chain {

   struct block_state : public block_header_state {
      block_state( const block_header_state& cur ):block_header_state(cur){}
      block_state( const block_header_state& prev, signed_block_ptr b, bool trust = false );
      block_state( const block_header_state& prev, block_timestamp_type when );
      block_state() = default;

      /// weak_ptr prev_block_state....
      signed_block_ptr                                    block;
      bool                                                validated = false;
      bool                                                in_current_chain = false;

      /// this data is redundant with the data stored in block, but facilitates
      /// recapturing transactions when we pop a block
      vector<transaction_metadata_ptr>                    trxs;

      /// YOSEMITE Transaction-as-a-Vote for Proof-of-Transaction
      /// cumulated transaction votes in this block
      yosemite_core::transaction_votes_in_block           trx_votes;
   };

   using block_state_ptr = std::shared_ptr<block_state>;

} } /// namespace eosio::chain

FC_REFLECT_DERIVED( eosio::chain::block_state, (eosio::chain::block_header_state), (block)(validated)(in_current_chain) )
