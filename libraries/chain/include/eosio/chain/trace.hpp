/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosio/chain/action.hpp>
#include <eosio/chain/action_receipt.hpp>
#include <eosio/chain/block.hpp>

#include <yosemite/chain/transaction_as_a_vote.hpp>

namespace eosio { namespace chain {

   struct account_delta {
      account_delta( const account_name& n, int64_t d):account(n),delta(d){}
      account_delta(){}

      account_name account;
      int64_t delta = 0;

      friend bool operator<( const account_delta& lhs, const account_delta& rhs ) { return lhs.account < rhs.account; }
   };

   struct base_action_trace {
      base_action_trace( const action_receipt& r ):receipt(r){}
      base_action_trace(){}

      action_receipt       receipt;
      action               act;
      bool                 context_free = false;
      fc::microseconds     elapsed;
      uint64_t             cpu_usage = 0;
      string               console;

      uint64_t             total_cpu_usage = 0; /// total of inline_traces[x].cpu_usage + cpu_usage
      transaction_id_type  trx_id; ///< the transaction that generated this action
      uint32_t             block_num = 0;
      block_timestamp_type block_time;
      fc::optional<block_id_type>     producer_block_id;
      flat_set<account_delta>         account_ram_deltas;
   };

   struct action_trace : public base_action_trace {
      using base_action_trace::base_action_trace;

      vector<action_trace> inline_traces;
   };

   struct transaction_trace;
   using transaction_trace_ptr = std::shared_ptr<transaction_trace>;

   struct transaction_trace {
      transaction_id_type                        id;
      uint32_t                                   block_num = 0;
      block_timestamp_type                       block_time;
      fc::optional<block_id_type>                producer_block_id;
      fc::optional<transaction_receipt_header>   receipt;
      fc::microseconds                           elapsed;
      uint64_t                                   net_usage = 0;
      bool                                       scheduled = false;
      vector<action_trace>                       action_traces; ///< disposable

      /// YOSEMITE Proof-of-Transaction
      /// tracking transaction vote amount generated from current transaction.
      /// transaction votes collected from each transaction are accumulated in the (pending) 'block state' of each block.
      /// this field is also used for transaction-vote logging in secondary log store
      fc::optional<yosemite_core::transaction_vote> trx_vote;

      /// YOSEMITE Delegated Transaction Fee Payment
      /// YOSEMITE blockchain provides 'transaction fee payer' option for every blockchain transaction.
      /// If 'transaction fee payer' field is specified in a submitted transaction, transaction fee is charged to
      /// the specified transaction fee payer who additionally signed the transaction on behalf of the initial transaction sender
      fc::optional<account_name>                 fee_payer;

      transaction_trace_ptr                      failed_dtrx_trace;
      fc::optional<fc::exception>                except;
      std::exception_ptr                         except_ptr;
   };

} }  /// namespace eosio::chain

FC_REFLECT( eosio::chain::account_delta,
            (account)(delta) )

FC_REFLECT( eosio::chain::base_action_trace,
                    (receipt)(act)(context_free)(elapsed)(cpu_usage)(console)(total_cpu_usage)(trx_id)
                    (block_num)(block_time)(producer_block_id)(account_ram_deltas) )

FC_REFLECT_DERIVED( eosio::chain::action_trace,
                    (eosio::chain::base_action_trace), (inline_traces) )

FC_REFLECT( eosio::chain::transaction_trace, (id)(block_num)(block_time)(producer_block_id)
                                             (receipt)(elapsed)(net_usage)(scheduled)
                                             (action_traces)(trx_vote)(fee_payer)(failed_dtrx_trace)(except) )
