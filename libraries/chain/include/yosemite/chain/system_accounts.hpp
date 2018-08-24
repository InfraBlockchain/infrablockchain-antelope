#pragma once

#include <eosio/chain/name.hpp>

#define YOSEMITE_TX_FEE_ACCOUNT N(yx.txfee)
#define YOSEMITE_IDENTITY_ACCOUNT N(yx.identity)
#define YOSEMITE_NATIVE_TOKEN_ACCOUNT N(yx.ntoken)
#define YOSEMITE_USER_TOKEN_ACCOUNT N(yx.token)
#define YOSEMITE_DIGITAL_CONTRACT_ACCOUNT N(yx.dcontract)


namespace yosemite { namespace chain {

    bool is_system_account(const account_name account) {
        return (account == config::system_account_name
                || account == config::producers_account_name
                || account == YOSEMITE_TX_FEE_ACCOUNT
                || account == YOSEMITE_IDENTITY_ACCOUNT
                || account == YOSEMITE_NATIVE_TOKEN_ACCOUNT
                || account == YOSEMITE_USER_TOKEN_ACCOUNT
                || account == YOSEMITE_DIGITAL_CONTRACT_ACCOUNT
        );
    }

}} // namespace yosemite::chain
