#pragma once

#include <eosiolib/types.hpp>

#define YOSEMITE_SYSTEM_ACCOUNT N(eosio)
#define YOSEMITE_TX_FEE_ACCOUNT N(yx.txfee)
#define YOSEMITE_IDENTITY_ACCOUNT N(yx.identity)
#define YOSEMITE_NATIVE_TOKEN_ACCOUNT N(yx.ntoken)
#define YOSEMITE_USER_TOKEN_ACCOUNT N(yx.token)


namespace yosemite {

    bool is_system_account(const account_name account) {
        return (account == YOSEMITE_SYSTEM_ACCOUNT
                || account == YOSEMITE_TX_FEE_ACCOUNT
                || account == YOSEMITE_IDENTITY_ACCOUNT
                || account == YOSEMITE_NATIVE_TOKEN_ACCOUNT
                || account == YOSEMITE_USER_TOKEN_ACCOUNT
        );
    }

}
