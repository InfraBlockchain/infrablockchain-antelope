/**
 *  @file transaction_extensions.hpp
 *  @brief YOSEMITE Transaction Extensions
 *  @author bezalel@yosemitex.com
 *  @copyright defined in LICENSE.txt
 */
#pragma once

/**
 * Transaction Extensions Data Structure
 *
 * struct transaction_header {
 *    time_point_sec         expiration;   ///< the time at which a transaction expires
 *    uint16_t               ref_block_num       = 0U; ///< specifies a block num in the last 2^16 blocks.
 *    uint32_t               ref_block_prefix    = 0UL; ///< specifies the lower 32 bits of the blockid at get_ref_blocknum
 *    fc::unsigned_int       max_net_usage_words = 0UL; /// upper limit on total network bandwidth (in 8 byte words) billed for this transaction
 *    uint8_t                max_cpu_usage_ms    = 0; /// upper limit on the total CPU time billed for this transaction
 *    fc::unsigned_int       delay_sec           = 0UL; /// number of seconds to delay this transaction for during which it may be canceled.
 * };
 *
 * typedef vector<std::pair<uint16_t,vector<char>>> extensions_type;
 *
 * struct transaction : public transaction_header {
 *    vector<action>         context_free_actions;
 *    vector<action>         actions;
 *    extensions_type        transaction_extensions;
 * }
 *
 */

/**
 * YOSEMITE Transaction-as-a-Vote protocol uses "transaction_extensions" field of EOS transaction binary data format.
 * The transaction-extension field code for YOSEMITE TaaV is 1001,
 * and the field value should be encoded as the binary representation of 64bit base-32 encoding for blockchain account name of transaction-vote target
 *
 * Example of json representation blockchain transaction with YOSEMITE TaaV
 * {
 *   "expiration": "2018-08-16T06:17:11",
 *   "ref_block_num": 30369,
 *   "ref_block_prefix": 1005358512,
 *   "max_net_usage_words": 0,
 *   "max_cpu_usage_ms": 0,
 *   "delay_sec": 0,
 *   "context_free_actions": [],
 *   "actions": [{
 *       "account": "yx.ntoken",
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
 *       1001,
 *       "00c00257219de8ad"
 *     ]
 *   ],
 *   "signatures": [
 *     "SIG_K1_K24Pe1TVtdCzY2s4NvfvPd28NL1jHoFUEsugxPCXqonoSJDZ4XD9S7CMBP8WyTbhkBFweWT9GLBDDJgQCv9qSaL8SLbR2s"
 *   ],
 *   "context_free_data": []
 * }
 */
#define YOSEMITE_TRANSACTION_VOTE_ACCOUNT_TX_EXTENSION_FIELD 1001


/**
 * YOSEMITE Delegated Transaction Fee Payment
 * The preset (on yx.txfee contract) transaction fee amount (in native stable token) is
 * charged to the principal account of each transaction on YOSEMITE blockchain.
 * But in real service situation, blockchain service provider can pay the transaction fees
 * on behalf of service users. By paying user's blockchain transaction fee, service provider
 * can enhance user experience and attract more customers.
 *
 * To support 'Delegated Transaction Fee Payment', YOSEMITE blockchain provides
 * 'transaction fee payer' option for every blockchain transaction.
 * If 'transaction fee payer' field is specified in a submitted transaction,
 * the transaction fee generated from the transaction is charged to the 'transaction fee payer'.
 * The submitted transaction message must contain crypto signature of 'transaction fee payer' account.
 *
 * Example of json representation blockchain transaction with YOSEMITE Delegated Transaction Fee Payment
 * {
 *   "expiration": "2018-08-16T06:17:11",
 *   "ref_block_num": 30369,
 *   "ref_block_prefix": 1005358512,
 *   "max_net_usage_words": 0,
 *   "max_cpu_usage_ms": 0,
 *   "delay_sec": 0,
 *   "context_free_actions": [],
 *   "actions": [{
 *       "account": "yx.ntoken",
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
 *       1001,
 *       "00c00257219de8ad"
 *     ],
 *     [
 *       1002,
 *       "0011223344aabbcc"
 *     ]
 *   ],
 *   "signatures": [
 *     "YSG_K1_K24Pe1TVtdCzY2s4NvfvPd28NL1jHoFUEsugxPCXqonoSJDZ4XD9S7CMBP8WyTbhkBFweWT9GLBDDJgQCv9qSaL8SLbR2s"
 *     "YSG_K1_De23QFuw98F32f98havdoaug2fp7gu23fiuadsvo97g2eoiASDFWEoihWEFscygoa8yg2eDF32d7gsadfuyg23fDw3fgsW"
 *   ],
 *   "context_free_data": []
 * }
 */
#define YOSEMITE_DELEGATED_TRANSACTION_FEE_PAYER_TX_EXTENSION_FIELD 1002