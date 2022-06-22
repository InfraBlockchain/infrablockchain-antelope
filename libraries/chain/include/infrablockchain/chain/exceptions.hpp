/**
 *  @file
 *  @copyright defined in infrablockchain/LICENSE
 */
#pragma once
#include <eosio/chain/exceptions.hpp>

namespace infrablockchain { namespace chain {

   using namespace eosio::chain;

// FC_DECLARE_DERIVED_EXCEPTION( transaction_exception,             chain_exception,
//                               3040000, "Transaction exception" )


   FC_DECLARE_DERIVED_EXCEPTION( infrablockchain_standard_token_exception, chain_exception,
                               3711000, "InfraBlockchain standard token exception" )

      FC_DECLARE_DERIVED_EXCEPTION( token_meta_validate_exception, infrablockchain_standard_token_exception,
                                    3711001, "Invalid token meta info" )

      FC_DECLARE_DERIVED_EXCEPTION( token_action_validate_exception, infrablockchain_standard_token_exception,
                                    3711002, "Invalid token action data" )

      FC_DECLARE_DERIVED_EXCEPTION( token_not_yet_created_exception, infrablockchain_standard_token_exception,
                                    3711003, "No token metadata, token is not yet created" )

      FC_DECLARE_DERIVED_EXCEPTION( no_token_target_account_exception, infrablockchain_standard_token_exception,
                                    3711004, "token target account does not exist" )

      FC_DECLARE_DERIVED_EXCEPTION( token_symbol_mismatch_exception, infrablockchain_standard_token_exception,
                                    3711005, "Token symbol mismatch" )

      FC_DECLARE_DERIVED_EXCEPTION( invalid_token_balance_update_access_exception, infrablockchain_standard_token_exception,
                                    3711006, "Invalid token balance update access" )

      FC_DECLARE_DERIVED_EXCEPTION( token_balance_overflow_exception, infrablockchain_standard_token_exception,
                                    3711007, "Token balance overflow" )

      FC_DECLARE_DERIVED_EXCEPTION( token_balance_underflow_exception, infrablockchain_standard_token_exception,
                                    3711008, "Token balance underflow" )

      FC_DECLARE_DERIVED_EXCEPTION( insufficient_token_balance_exception, infrablockchain_standard_token_exception,
                                    3711009, "Insufficient token balance" )

      FC_DECLARE_DERIVED_EXCEPTION( weighted_system_token_balance_overflow_exception, infrablockchain_standard_token_exception,
                                    3711010, "Weighted system token balance overflow" )

      FC_DECLARE_DERIVED_EXCEPTION( weighted_total_system_token_balance_per_account_overflow_exception, infrablockchain_standard_token_exception,
                                    3711011, "Weighted total system token balance per account overflow" )


   FC_DECLARE_DERIVED_EXCEPTION( infrablockchain_transaction_fee_exception, chain_exception,
                                 3712000, "InfraBlockchain transaction fee exception" )

      FC_DECLARE_DERIVED_EXCEPTION( ill_formed_transaction_fee_payer_tx_ext, infrablockchain_transaction_fee_exception,
                                    3712001, "Transaction includes an ill-formed transaction fee payer extension" )

      FC_DECLARE_DERIVED_EXCEPTION( invalid_transaction_fee_payer_account, infrablockchain_transaction_fee_exception,
                                    3712002, "Invalid transaction fee payer Account name" )

//    FC_DECLARE_DERIVED_EXCEPTION( invalid_tx_fee_setup_exception, infrablockchain_transaction_fee_exception,
//                                  3712001, "Invalid transaction fee setup" )

   FC_DECLARE_DERIVED_EXCEPTION( infrablockchain_proof_of_transaction_exception, chain_exception,
                                 3713000, "InfraBlockchain Proof-of-Transaction exception" )

      FC_DECLARE_DERIVED_EXCEPTION( ill_formed_transaction_vote_tx_ext, infrablockchain_proof_of_transaction_exception,
                                    3713001, "Transaction includes an ill-formed transaction vote extension" )

      FC_DECLARE_DERIVED_EXCEPTION( transaction_vote_amount_overflow, infrablockchain_proof_of_transaction_exception,
                                    3713002, "Overflow in transaction vote amount calculation" )

      FC_DECLARE_DERIVED_EXCEPTION( tx_votes_weighted_unique_idx_value_overflow, infrablockchain_proof_of_transaction_exception,
                                    3713003, "received_transaction_votes_object::tx_votes_weighted_unique_idx value overflow" )

}} // namespace infrablockchain::chain
