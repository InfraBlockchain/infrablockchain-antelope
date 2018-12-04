/**
 *  @file
 *  @copyright defined in yosemite/LICENSE
 */
#pragma once
#include <eosio/chain/exceptions.hpp>

namespace yosemite { namespace chain {

    using namespace eosio::chain;

//FC_DECLARE_DERIVED_EXCEPTION( transaction_exception,             chain_exception,
//                              3040000, "Transaction exception" )

    FC_DECLARE_DERIVED_EXCEPTION( invalid_trx_vote_target_account, transaction_exception,
                                  3041001, "Invalid Transaction-Vote Target Account Data In Tx-Ext" )

    FC_DECLARE_DERIVED_EXCEPTION( invalid_delegated_trx_fee_payer_account, transaction_exception,
                                  3041002, "Invalid Delegated Transaction Fee Payer Account Data In Tx-Ext" )

    FC_DECLARE_DERIVED_EXCEPTION( dtfp_inside_generated_tx, transaction_exception,
                                  3041003, "Delegated-Transaction-Fee-Payment is not allowed inside generated transaction" )

  FC_DECLARE_DERIVED_EXCEPTION( yosemite_standard_token_exception, chain_exception,
                                3711000, "YOSEMITE standard token exception" )

    FC_DECLARE_DERIVED_EXCEPTION( token_action_validate_exception, yosemite_standard_token_exception,
                                  3711001, "Invalid token action data" )

    FC_DECLARE_DERIVED_EXCEPTION( token_not_yet_created_exception, yosemite_standard_token_exception,
                                  3711002, "No token metadata, token is not yet created" )

    FC_DECLARE_DERIVED_EXCEPTION( no_token_target_account_exception, yosemite_standard_token_exception,
                                  3711003, "token target account does not exist" )

    FC_DECLARE_DERIVED_EXCEPTION( token_symbol_mismatch_exception, yosemite_standard_token_exception,
                                  3711004, "Token symbol mismatch" )

    FC_DECLARE_DERIVED_EXCEPTION( invalid_token_balance_update_access_exception, yosemite_standard_token_exception,
                                  3711005, "Invalid token balance update access" )

    FC_DECLARE_DERIVED_EXCEPTION( token_balance_overflow_exception, yosemite_standard_token_exception,
                                  3711006, "Token balance overflow" )

    FC_DECLARE_DERIVED_EXCEPTION( token_balance_underflow_exception, yosemite_standard_token_exception,
                                  3711007, "Token balance underflow" )

    FC_DECLARE_DERIVED_EXCEPTION( insufficient_token_balance_exception, yosemite_standard_token_exception,
                                  3711008, "Insufficient token balance" )


}} // namespace yosemite::chain
