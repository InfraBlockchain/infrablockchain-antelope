/**
 *  @file
 *  @copyright defined in infrablockchain/LICENSE.txt
 */
#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <infrablockchainlib/native_token_symbol.hpp>
#include <infrablockchainlib/system_accounts.hpp>

// infrasys
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_SYSTEM_NEW_ACCOUNT     N(tf.newacc)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_SYSTEM_REG_PRODUCER    N(tf.regprod)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_SYSTEM_REG_SYS_DEPO    N(tf.regsysdep)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_SYSTEM_REG_ID_AUTH     N(tf.regidauth)

// yx.ntoken
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_NTOKEN_ISSUE           N(tf.nissue)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_NTOKEN_REDEEM          N(tf.nredeem)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_NTOKEN_TRANSFER        N(tf.transfer)  // = wptransfer
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_NTOKEN_NTRANSFER       N(tf.ntransfer) // = wpntransfer

// yx.token, yx.nft
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_TOKEN_CREATE           N(tf.tcreate)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_TOKEN_ISSUE            N(tf.tissue)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_TOKEN_REDEEM           N(tf.tredeem)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_TOKEN_TRANSFER         N(tf.ttransfer)  // = wptransfer
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_TOKEN_SETKYCRULE       N(tf.tsetkyc)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_TOKEN_SETOPTIONS       N(tf.tsetopts)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_TOKEN_FREEZEACC        N(tf.tfreezeac)

#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_TOKEN_SET_USER_ISSUE_LIMIT N(tf.tsetuilim)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_TOKEN_ISSUE_BY_USER    N(tf.tissuebyu)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_TOKEN_ENTRUST_USER_ISSUE_TO N(tf.ttrustui)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_TOKEN_CHANGE_ISSUED    N(tf.tchangeis)

// yx.dcontract
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_DCONTRACT_CREATE       N(tf.dccreate)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_DCONTRACT_ADDSIGNERS   N(tf.dcaddsign)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_DCONTRACT_SIGN         N(tf.dcsign)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_DCONTRACT_UPADDDOCHASH N(tf.dcupadd)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_DCONTRACT_REMOVE       N(tf.dcremove)

// yx.escrow
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_ESCROW_ESCROW          N(tf.esescrow)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_ESCROW_PROCEED         N(tf.esproceed)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_ESCROW_CANCEL          N(tf.escancel)

// yx.nftex
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_NFT_BUY                N(tf.nftbuy)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_NFT_SELL               N(tf.nftsell)
#define INFRABLOCKCHAIN_TX_FEE_OP_NAME_NFT_SELL_CANCEL        N(tf.nftsellc)

namespace infrablockchain {

    typedef uint64_t operation_name;

    struct tx_fee_info {
        operation_name operation;
        int64_t fee; // transaction fee amount in INFRABLOCKCHAIN_NATIVE_TOKEN_SYMBOL

        uint64_t primary_key() const { return operation; }

        // explicit serialization macro is not necessary, used here only to improve compilation time
        EOSLIB_SERIALIZE( tx_fee_info, (operation)(fee) )
    };

    typedef eosio::multi_index<N(txfees), tx_fee_info> tx_fee_idx;

    eosio::asset get_transaction_fee(const operation_name operation_name) {
        tx_fee_idx fee_table(INFRABLOCKCHAIN_SYS_TX_FEE_ACCOUNT, INFRABLOCKCHAIN_SYS_TX_FEE_ACCOUNT);
        return eosio::asset(fee_table.get(operation_name, "operation for fee not found").fee, INFRABLOCKCHAIN_NATIVE_TOKEN_SYMBOL);
    }

    int64_t get_transaction_fee_amount(const operation_name operation_name) {
        tx_fee_idx fee_table(INFRABLOCKCHAIN_SYS_TX_FEE_ACCOUNT, INFRABLOCKCHAIN_SYS_TX_FEE_ACCOUNT);
        return fee_table.get(operation_name, "operation for fee not found").fee;
    }

}
