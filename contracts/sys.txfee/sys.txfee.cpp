/**
 *  @file contracts/sys.txfee/sys.txfee.cpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */

#include "sys.txfee.hpp"

#include <infrablockchainlib/system_accounts.hpp>
#include <infrablockchainlib/native_token_symbol.hpp>
#include <infrablockchainlib/transaction_fee.hpp>

namespace infrablockchain {

    void txfee_contract::settxfee(const operation_name operation, const asset &fee) {

        require_auth(INFRABLOCKCHAIN_SYSTEM_ACCOUNT); // should be signed by the active producers

        eosio_assert(fee.is_valid(), "wrong fee format");
        eosio_assert(fee.amount >= 0, "negative fee is not allowed");
        eosio_assert(fee.symbol.value == INFRABLOCKCHAIN_NATIVE_TOKEN_SYMBOL, "only the native token is allowed for fee");

        // self == INFRABLOCKCHAIN_SYS_TX_FEE_ACCOUNT
        tx_fee_idx fee_table(get_self(), get_self());

        auto fee_info_it = fee_table.find(operation);
        if (fee_info_it == fee_table.end()) {
            fee_table.emplace(get_self(), [&](tx_fee_info& info) {
                info.operation = operation;
                info.fee = fee.amount;
            });
        } else {
            fee_table.modify(fee_info_it, 0, [&](tx_fee_info& info) {
                info.fee = fee.amount;
            });
        }
    }
}

EOSIO_ABI(infrablockchain::txfee_contract, (settxfee))
