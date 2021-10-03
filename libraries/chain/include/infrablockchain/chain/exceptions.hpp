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

// FC_DECLARE_DERIVED_EXCEPTION( infrablockchain_transaction_fee_exception, chain_exception,
//                               3712000, "InfraBlockchain transaction fee exception" )


}} // namespace infrablockchain::chain
