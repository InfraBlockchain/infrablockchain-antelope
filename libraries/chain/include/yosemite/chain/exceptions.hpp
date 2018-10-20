/**
 *  @file
 *  @copyright defined in yosemite/LICENSE
 */
#pragma once
#include <eosio/chain/exceptions.hpp>

namespace yosemite { namespace chain {

    using namespace eosio::chain;

    FC_DECLARE_DERIVED_EXCEPTION( invalid_trx_vote_target_account, transaction_exception,
                                  3041001, "Invalid Transaction-Vote Target Account Data In Tx-Ext" )

    FC_DECLARE_DERIVED_EXCEPTION( invalid_delegated_trx_fee_payer_account, transaction_exception,
                                  3041002, "Invalid Delegated Transaction Fee Payer Account Data In Tx-Ext" )


}} // namespace yosemite::chain
