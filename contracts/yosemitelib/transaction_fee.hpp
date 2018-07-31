/**
 *  @file
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <yosemitelib/native_token_symbol.hpp>
#include <yosemitelib/system_accounts.hpp>

// yx.system
#define YOSEMITE_TX_FEE_OP_NAME_SYSTEM_NEW_ACCOUNT     N(tf.newacc)
#define YOSEMITE_TX_FEE_OP_NAME_SYSTEM_REG_PRODUCER    N(tf.regprod)
#define YOSEMITE_TX_FEE_OP_NAME_SYSTEM_REG_SYS_DEPO    N(tf.regsysdep)
#define YOSEMITE_TX_FEE_OP_NAME_SYSTEM_REG_ID_AUTH     N(tf.regidauth)

// yx.ntoken
#define YOSEMITE_TX_FEE_OP_NAME_NTOKEN_ISSUE           N(tf.nissue)
#define YOSEMITE_TX_FEE_OP_NAME_NTOKEN_REDEEM          N(tf.nredeem)
#define YOSEMITE_TX_FEE_OP_NAME_NTOKEN_TRANSFER        N(tf.transfer)  // = wptransfer
#define YOSEMITE_TX_FEE_OP_NAME_NTOKEN_NTRANSFER       N(tf.ntransfer) // = wpntransfer

// yx.token
#define YOSEMITE_TX_FEE_OP_NAME_TOKEN_CREATE           N(tf.tcreate)
#define YOSEMITE_TX_FEE_OP_NAME_TOKEN_ISSUE            N(tf.tissue)
#define YOSEMITE_TX_FEE_OP_NAME_TOKEN_REDEEM           N(tf.tredeem)
#define YOSEMITE_TX_FEE_OP_NAME_TOKEN_TRANSFER         N(tf.ttransfer)  // = wptransfer
#define YOSEMITE_TX_FEE_OP_NAME_TOKEN_SETKYCRULE       N(tf.tsetkyc)

// yx.dcontract
#define YOSEMITE_TX_FEE_OP_NAME_DCONTRACT_CREATE       N(tf.dccreate)
#define YOSEMITE_TX_FEE_OP_NAME_DCONTRACT_ADDSIGNERS   N(tf.dcaddsign)
#define YOSEMITE_TX_FEE_OP_NAME_DCONTRACT_SIGN         N(tf.dcsign)
#define YOSEMITE_TX_FEE_OP_NAME_DCONTRACT_UPADDDOCHASH N(tf.dcupadd)
#define YOSEMITE_TX_FEE_OP_NAME_DCONTRACT_REMOVE       N(tf.dcremove)


namespace yosemite {

    typedef uint64_t operation_name;

    struct tx_fee_info {
        operation_name operation;
        int64_t fee; // transaction fee amount in YOSEMITE_NATIVE_TOKEN_SYMBOL

        uint64_t primary_key() const { return operation; }

        // explicit serialization macro is not necessary, used here only to improve compilation time
        EOSLIB_SERIALIZE( tx_fee_info, (operation)(fee) )
    };

    typedef eosio::multi_index<N(txfees), tx_fee_info> tx_fee_idx;

    eosio::asset get_transaction_fee(const operation_name operation_name) {
        tx_fee_idx fee_table(YOSEMITE_TX_FEE_ACCOUNT, YOSEMITE_TX_FEE_ACCOUNT);
        return eosio::asset(fee_table.get(operation_name, "operation for fee not found").fee, YOSEMITE_NATIVE_TOKEN_SYMBOL);
    }

    int64_t get_transaction_fee_amount(const operation_name operation_name) {
        tx_fee_idx fee_table(YOSEMITE_TX_FEE_ACCOUNT, YOSEMITE_TX_FEE_ACCOUNT);
        return fee_table.get(operation_name, "operation for fee not found").fee;
    }

}
