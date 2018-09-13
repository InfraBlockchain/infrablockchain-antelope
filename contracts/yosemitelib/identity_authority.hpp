/**
 *  @file
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <yosemitelib/system_accounts.hpp>
#include <eosiolib/eosio.hpp>
#include <string>

namespace yosemite {

    // The authorized Identity Authorities have the right for managing user info and KYC info on blockchain.
    struct identity_authority_info {
        account_name          owner;
        bool                  is_authorized = false;
        std::string           url;
        uint16_t              location = 0;

        uint64_t primary_key() const { return owner; }

        // explicit serialization macro is not necessary, used here only to improve compilation time
        EOSLIB_SERIALIZE( identity_authority_info, (owner)(is_authorized)
                (url)(location) )
    };

    typedef eosio::multi_index< N(idauthority), identity_authority_info > identity_authority_table;

    bool is_authorized_identity_authority( const account_name identity_authority ) {
        identity_authority_table idauthorities(YOSEMITE_SYSTEM_ACCOUNT, YOSEMITE_SYSTEM_ACCOUNT);
        auto idauth = idauthorities.find( identity_authority );
        return idauth != idauthorities.end() && (*idauth).is_authorized;
    }

}
