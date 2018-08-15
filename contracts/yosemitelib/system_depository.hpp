/**
 *  @file
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <yosemitelib/system_accounts.hpp>
#include <eosiolib/eosio.hpp>
#include <string>

namespace yosemite {

    // system depository issuing native stable token
    struct sys_depository_info {
        account_name          owner;
        bool                  is_authorized = false;
        std::string           url;
        uint16_t              location = 0;

        uint64_t primary_key() const { return owner; }

        // explicit serialization macro is not necessary, used here only to improve compilation time
        EOSLIB_SERIALIZE( sys_depository_info, (owner)(is_authorized)
                (url)(location) )
    };

    typedef eosio::multi_index< N(sysdepos), sys_depository_info > sys_depository_table;

    bool is_authorized_sys_depository( const account_name depository ) {
        sys_depository_table depositories(YOSEMITE_SYSTEM_ACCOUNT, YOSEMITE_SYSTEM_ACCOUNT);
        auto depo = depositories.find( depository );
        return depo != depositories.end() && (*depo).is_authorized;
    }
}
