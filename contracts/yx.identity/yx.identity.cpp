/**
 *  @copyright defined in LICENSE.txt
 */

#include "yx.identity.hpp"

#include <eosiolib/action.h>
#include <yosemitelib/system_accounts.hpp>
#include <yosemitelib/identity.hpp>
#include <yosemitelib/identity_authority.hpp>
#include <yx.system/yx.system.hpp>

namespace yosemite { namespace identity {

    void identity_contract::setidinfo(const account_name account, const account_name identity_authority, uint16_t type, uint16_t kyc, uint32_t state, const std::string& data) {

        eosio_assert( is_account(account), "account does not exist");
        eosio_assert( is_authorized_identity_authority(identity_authority),
                      "identity_authority is not authorized identity authority");
//        eosio_assert( is_valid_account_type(type), "invalid account type value");
//        eosio_assert( is_valid_kyc_status(kyc), "invalid kyc status value");
//        eosio_assert( is_valid_account_state(state), "invalid account state value");
        eosio_assert( data.size() < 256, "data too long" );

        require_auth( identity_authority );

        // self == YOSEMITE_IDENTITY_ACCOUNT
        identity_idx identity_table(get_self(),get_self());

        auto id_it = identity_table.find(account);

        if (id_it != identity_table.end()) {
            eosio_assert( id_it->identity_authority == identity_authority, "not signed by the identity authority who has right to manage this account");
            identity_table.modify( id_it, 0, [&]( identity_info& info ){
                info.type  = type;
                info.kyc   = kyc;
                info.state = state;
                info.data  = data;
            });
        } else {
            identity_table.emplace( identity_authority, [&]( identity_info& info ){
                info.account            = account;
                info.identity_authority = identity_authority;
                info.type  = type;
                info.kyc   = kyc;
                info.state = state;
                info.data  = data;
            });
        }
    }

    void identity_contract::delidinfo(account_name account) {

        // self == YOSEMITE_IDENTITY_ACCOUNT
        identity_idx identity_table(get_self(),get_self());
        auto id_it = identity_table.find(account);
        eosio_assert( id_it != identity_table.end(), "not found identity_info" );

        const account_name &existing_id_auth = id_it->identity_authority;
        eosio_assert( is_authorized_identity_authority(existing_id_auth),
                      "existing identity authority is not authorized identity authority");
        require_auth( existing_id_auth );

        identity_table.erase(id_it);
    }

    void identity_contract::settype(account_name account, uint16_t type) {

//        eosio_assert( is_valid_account_type(type), "invalid account type value");

        // self == YOSEMITE_IDENTITY_ACCOUNT
        identity_idx identity_table(get_self(),get_self());
        auto id_it = identity_table.find(account);
        eosio_assert( id_it != identity_table.end(), "not found identity_info" );

        const account_name &existing_id_auth = id_it->identity_authority;
        eosio_assert( is_authorized_identity_authority(existing_id_auth),
                      "existing identity authority is not authorized identity authority");
        require_auth( existing_id_auth );

        eosio_assert( id_it->type != type, "same type value" );

        identity_table.modify( id_it, 0, [&]( identity_info& info ){
            info.type  = type;
        });
    }

    void identity_contract::setkyc(account_name account, uint16_t kyc) {

//        eosio_assert( is_valid_kyc_status(kyc), "invalid kyc status value");

        // self == YOSEMITE_IDENTITY_ACCOUNT
        identity_idx identity_table(get_self(),get_self());
        auto id_it = identity_table.find(account);
        eosio_assert( id_it != identity_table.end(), "not found identity_info" );

        const account_name &existing_id_auth = id_it->identity_authority;
        eosio_assert( is_authorized_identity_authority(existing_id_auth),
                      "existing identity authority is not authorized identity authority");
        require_auth( existing_id_auth );

        eosio_assert( id_it->kyc != kyc, "same kyc value" );

        identity_table.modify( id_it, 0, [&]( identity_info& info ){
            info.kyc  = kyc;
        });
    }

    void identity_contract::setstate(account_name account, uint32_t state) {

//        eosio_assert( is_valid_account_state(state), "invalid account state value");

        // self == YOSEMITE_IDENTITY_ACCOUNT
        identity_idx identity_table(get_self(),get_self());
        auto id_it = identity_table.find(account);
        eosio_assert( id_it != identity_table.end(), "not found identity_info" );

        const account_name &existing_id_auth = id_it->identity_authority;
        eosio_assert( is_authorized_identity_authority(existing_id_auth),
                      "existing identity authority is not authorized identity authority");
        require_auth( existing_id_auth );

        eosio_assert( id_it->state != state, "same state value" );

        identity_table.modify( id_it, 0, [&]( identity_info& info ){
            info.state  = state;
        });
    }

    void identity_contract::setdata(account_name account, const std::string& data) {

        eosio_assert( data.size() < 256, "data too long" );

        // self == YOSEMITE_IDENTITY_ACCOUNT
        identity_idx identity_table(get_self(),get_self());
        auto id_it = identity_table.find(account);
        eosio_assert( id_it != identity_table.end(), "not found identity_info" );

        const account_name &existing_id_auth = id_it->identity_authority;
        eosio_assert( is_authorized_identity_authority(existing_id_auth),
                      "existing identity authority is not authorized identity authority");
        require_auth( existing_id_auth );

        eosio_assert( id_it->data != data, "same data value" );

        identity_table.modify( id_it, 0, [&]( identity_info& info ){
            info.data  = data;
        });
    }
}}

EOSIO_ABI(yosemite::identity::identity_contract,
        (setidinfo)(delidinfo)
        (settype)(setkyc)(setstate)(setdata)
)
