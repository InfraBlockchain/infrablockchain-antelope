/**
 *  @file infrablockchain/chain/transaction_extensions.hpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */
#pragma once

#include <eosio/chain/types.hpp>

namespace infrablockchain { namespace chain {

   using namespace eosio::chain;

      enum transaction_extension_id {
         infrablockchain_transaction_fee_payer_tx_ext_id = 10,
         infrablockchain_transaction_vote_tx_ext_id = 11

         // @see eosio::chain::transaction_extension_id in eosio/chain/transaction.hpp
      };

   /**
    * InfraBlockchain Transaction Fee Payer
    *
    * A transaction message for InfraBlockchain can contain 'transaction fee payer' account in a transaction extension.
    * The transaction fee payer need to sign the transaction message
    * for which the transaction fee is charged to the payer account.
    * In blockchain-based production service, blockchain service provider can pay the transaction fees
    * on behalf of service users. By paying user's blockchain transaction fee, service provider
    * can enhance user experience and attract more customers.
    *
    * To support 'Transaction Fee Payer', InfraBlockchain provides
    * 'transaction fee payer' field for blockchain transactions.
    * If 'transaction fee payer' field is specified in a submitted transaction,
    * the transaction fee generated from the transaction is charged to the 'transaction fee payer'.
    * The submitted transaction message must contain crypto signature of 'transaction fee payer' account.
    *
    * Example of json representation blockchain transaction with InfraBlockchain Transaction Fee Payer
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
    *   "transaction_extensions": [
    *     [
    *       10,
    *       "00c00257219de8ad"
    *     ],
    *     [
    *       11,
    *       "0011223344aabbcc"
    *     ]
    *   ],
    *   "signatures": [
    *     "SIG_K1_K24Pe1TVtdCzY2s4NvfvPd28NL1jHoFUEsugxPCXqonoSJDZ4XD9S7CMBP8WyTbhkBFweWT9GLBDDJgQCv9qSaL8SLbR2s"
    *     "SIG_K1_De23QFuw98F32f98havdoaug2fp7gu23fiuadsvo97g2eoiASDFWEoihWEFscygoa8yg2eDF32d7gsadfuyg23fDw3fgsW"
    *   ],
    *   "context_free_data": []
    * }
    */
   struct transaction_fee_payer_tx_ext : fc::reflect_init {
      static constexpr uint16_t extension_id() { return transaction_extension_id::infrablockchain_transaction_fee_payer_tx_ext_id; }
      static constexpr bool     enforce_unique() { return true; }

      transaction_fee_payer_tx_ext() = default;

      transaction_fee_payer_tx_ext( account_name fee_payer )
      :fee_payer( fee_payer ) {}

      void reflector_init();

      account_name fee_payer;
   };


   /**
    * InfraBlockchain Transaction-as-a-Vote protocol uses "transaction_extensions" field of EOSIO transaction binary data format.
    * The transaction-extension field code for InfraBlockchain TaaV is 1026,
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
    *       11,
    *       "00c00257219de8ad"
    *     ]
    *   ],
    *   "signatures": [
    *     "SIG_K1_K24Pe1TVtdCzY2s4NvfvPd28NL1jHoFUEsugxPCXqonoSJDZ4XD9S7CMBP8WyTbhkBFweWT9GLBDDJgQCv9qSaL8SLbR2s"
    *   ],
    *   "context_free_data": []
    * }
    */
   struct transaction_vote_tx_ext : fc::reflect_init {
      static constexpr uint16_t extension_id() { return transaction_extension_id::infrablockchain_transaction_vote_tx_ext_id; }
      static constexpr bool     enforce_unique() { return true; }

      transaction_vote_tx_ext() = default;

      transaction_vote_tx_ext( account_name vote_to )
      :vote_to( vote_to ) {}

      void reflector_init();

      account_name vote_to;
   };

} } /// infrablockchain::chain

FC_REFLECT(infrablockchain::chain::transaction_fee_payer_tx_ext, (fee_payer) )
FC_REFLECT(infrablockchain::chain::transaction_vote_tx_ext, (vote_to) )