/**
 *  @file
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <yosemitelib/native_token_symbol.hpp>
#include <yosemitelib/system_accounts.hpp>

#define YOSEMITE_TX_FEE_OP_NAME_NEW_ACCOUNT     N(tf.newacc)
#define YOSEMITE_TX_FEE_OP_NAME_REG_PRODUCER    N(tf.regprod)
#define YOSEMITE_TX_FEE_OP_NAME_REG_SYS_DEPO    N(tf.regsysdep)
#define YOSEMITE_TX_FEE_OP_NAME_REG_ID_AUTH     N(tf.regidauth)

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

    const eosio::asset& get_transaction_fee(const operation_name operation_name) {
        tx_fee_idx fee_table(YOSEMITE_TX_FEE_ACCOUNT, YOSEMITE_TX_FEE_ACCOUNT);
        return eosio::asset(fee_table.get(operation_name).fee, YOSEMITE_NATIVE_TOKEN_SYMBOL);
    }

    int64_t get_transaction_fee_amount(const operation_name operation_name) {
        tx_fee_idx fee_table(YOSEMITE_TX_FEE_ACCOUNT, YOSEMITE_TX_FEE_ACCOUNT);
        return fee_table.get(operation_name).fee;
    }

}