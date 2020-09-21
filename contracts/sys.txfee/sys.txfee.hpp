/**
 *  @file contracts/sys.txfee/sys.txfee.hpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */

#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <infrablockchainlib/transaction_fee.hpp>

namespace infrablockchain {

    using eosio::asset;

    /***
     * System contract managing the transaction fee amount for each charged system transaction.
     * Transaction fee amounts should be agreed by the active block producers by signing txfee_contract::settxfee transaction
     * This contract is deployed on the system transaction fee account (INFRABLOCKCHAIN_SYS_TX_FEE_ACCOUNT)
     * on which transaction fees generated on the blockchain will be saved
     */
    class txfee_contract : public eosio::contract {
    public:
        explicit txfee_contract(account_name self) : contract(self) {
        }

        void settxfee(const operation_name operation, const asset &fee);
    };
}
