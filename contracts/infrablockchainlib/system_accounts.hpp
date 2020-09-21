#pragma once

#include <eosiolib/types.hpp>

#define INFRABLOCKCHAIN_SYSTEM_ACCOUNT N(infrasys)
#define INFRABLOCKCHAIN_SYS_TX_FEE_ACCOUNT N(sys.txfee)
#define INFRABLOCKCHAIN_SYS_IDENTITY_ACCOUNT N(sys.identity)

// DEPRECATED
#define INFRABLOCKCHAIN_SYS_NATIVE_TOKEN_ACCOUNT N(yx.ntoken)
// DEPRECATED
#define INFRABLOCKCHAIN_SYS_USER_TOKEN_ACCOUNT N(yx.token)

#define INFRABLOCKCHAIN_SYS_DIGITAL_CONTRACT_ACCOUNT N(yx.dcontract)
#define INFRABLOCKCHAIN_SYS_NON_FUNGIBLE_TOKEN_ACCOUNT N(yx.nft)
#define INFRABLOCKCHAIN_SYS_NON_FUNGIBLE_TOKEN_EXCHANGE_ACCOUNT N(yx.nftex)
#define INFRABLOCKCHAIN_SYS_TOKEN_ESCROW_ACCOUNT N(yx.escrow)


namespace infrablockchain {

    bool is_system_account(const account_name account) {
        return (account == INFRABLOCKCHAIN_SYSTEM_ACCOUNT
                || account == INFRABLOCKCHAIN_SYS_NATIVE_TOKEN_ACCOUNT
                || account == INFRABLOCKCHAIN_SYS_USER_TOKEN_ACCOUNT
                || account == INFRABLOCKCHAIN_SYS_TX_FEE_ACCOUNT
                || account == INFRABLOCKCHAIN_SYS_IDENTITY_ACCOUNT
                || account == INFRABLOCKCHAIN_SYS_DIGITAL_CONTRACT_ACCOUNT
                || account == INFRABLOCKCHAIN_SYS_NON_FUNGIBLE_TOKEN_ACCOUNT
                || account == INFRABLOCKCHAIN_SYS_NON_FUNGIBLE_TOKEN_EXCHANGE_ACCOUNT
                || account == INFRABLOCKCHAIN_SYS_TOKEN_ESCROW_ACCOUNT
        );
    }

}
