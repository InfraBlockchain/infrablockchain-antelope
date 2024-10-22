/**
 *  @file infrablockchain/chain/transaction_as_a_vote.hpp
 *  @brief InfraBlockchain Transaction-as-a-vote Data Structure
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in LICENSE.txt
 */
#pragma once

#include <map>
#include <vector>
#include <eosio/chain/name.hpp>
#include <fc/reflect/reflect.hpp>

#include <infrablockchain/chain/exceptions.hpp>

namespace infrablockchain { namespace chain {

    /**
     *
     * InfraBlockchain Proof-of-Transaction(PoT) and Transaction-as-a-Vote (TaaV)
     *
     * Proof-of-Transaction (PoT) incentivizes the application
     * service providers directly contributing to the blockchain ecosystem by generating actual
     * blockchain transactions, quantitatively measured by Transaction-as-a-Vote (TaaV).
     *
     * The key idea that makes Proof-of-Transaction a novel blockchain consensus mechanism is the
     * concept of Transaction-as-a-Vote (TaaV). Transactions generated by the client side of the
     * blockchain applications can optionally include a vote for a block producer candidate who can
     * potentially participate or is already participating in the blockchain consensus process. In
     * blockchain transaction messages which would incur some transaction fee, there is an optional
     * "transaction vote" field where a blockchain account address can be specified as a vote. That
     * means the entity generating the transaction on blockchain wants the blockchain core node
     * having the voted address to be a block producer.
     *
     *
     * ### Transaction-as-a-Vote message protocol
     *
     * InfraBlockchain Transaction-as-a-Vote protocol uses "transaction_extensions" field of EOSIO transaction binary data format.
     * The transaction-extension field code for InfraBlockchain TaaV is 1025,
     * and the field value should be encoded as the binary representation of 64bit base-32 encoding for blockchain account name of transaction-vote target
     *
     * Example of json representation blockchain transaction with InfraBlockchain TaaV
     * {
     *   "expiration": "2018-08-16T06:17:11",
     *   "ref_block_num": 30369,
     *   "ref_block_prefix": 1005358512,
     *   "max_net_usage_words": 0,
     *   "max_cpu_usage_ms": 0,
     *   "delay_sec": 0,
     *   "context_free_actions": [],
     *   "actions": [{
     *       "account": "infra.token",
     *       "name": "transfer",
     *       "authorization": [{
     *           "actor": "useraccount3",
     *           "permission": "active"
     *         }
     *       ],
     *       "data": "30f2d414217315d620f2d414217315d600e1f5050000000004444b5257000000046d656d6f"
     *     }
     *   ],
     *   "transaction_extensions": [[
     *       1025,
     *       "00c00257219de8ad"
     *     ]
     *   ],
     *   "signatures": [
     *     "SIG_K1_K24Pe1TVtdCzY2s4NvfvPd28NL1jHoFUEsugxPCXqonoSJDZ4XD9S7CMBP8WyTbhkBFweWT9GLBDDJgQCv9qSaL8SLbR2s"
     *   ],
     *   "context_free_data": []
     * }
     *
     */

    using transaction_vote_target_name_type = eosio::chain::name;
    using transaction_vote_amount_type = uint32_t;

    struct transaction_vote {

        explicit transaction_vote(transaction_vote_target_name_type t, transaction_vote_amount_type a) : to(t), amt(a) {}

        transaction_vote_target_name_type to; // eosio::chain::name
        transaction_vote_amount_type amt;

        bool has_vote() const {
            return !to.empty() && amt != 0;
        }
    };

    struct transaction_votes_in_block {

        // ordered map
        std::map<transaction_vote_target_name_type, transaction_vote_amount_type> tx_votes;

        void add_transaction_vote(const transaction_vote& tx_vote) {

            auto vote_it = tx_votes.find(tx_vote.to);
            if (vote_it == tx_votes.end()) {
                tx_votes[tx_vote.to] = tx_vote.amt;
            } else {
                auto current = tx_votes[tx_vote.to];
                auto new_val = current + tx_vote.amt;
                EOS_ASSERT( new_val > current, transaction_vote_amount_overflow, "overflow of transaction vote amount");
                tx_votes[tx_vote.to] =  new_val;
            }
        }

        std::vector<transaction_vote> get_tx_vote_list() {

            std::vector<transaction_vote> tx_vote_list;
            tx_vote_list.reserve( tx_votes.size() );

            // tx_votes : ordered map
            for (auto i = tx_votes.begin(); i != tx_votes.end(); i++) {
                tx_vote_list.push_back( transaction_vote{ i->first, i->second } );
            }

            return tx_vote_list;
        }

        bool has_transaction_votes() {
            return tx_votes.size() > 0;
        }
    };

} } /// infrablockchain::chain

FC_REFLECT(infrablockchain::chain::transaction_vote, (to)(amt))
FC_REFLECT(infrablockchain::chain::transaction_votes_in_block, (tx_votes))