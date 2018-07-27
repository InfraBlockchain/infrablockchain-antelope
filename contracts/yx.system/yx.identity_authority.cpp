#include "yx.system.hpp"
#include <yosemitelib/system_accounts.hpp>
#include <eosiolib/dispatcher.hpp>
#include "../yx.ntoken/yx.ntoken.hpp"

namespace yosemitesys {

    using yosemite::yx_asset;

    void system_contract::regidauth( const account_name identity_authority, const std::string& url, uint16_t location ) {

        eosio_assert( url.size() < 512, "url too long" );
        require_auth( identity_authority );

        auto idauth = _identity_authorities.find( identity_authority );

        if ( idauth != _identity_authorities.end() ) {
            _identity_authorities.modify( idauth, identity_authority, [&]( identity_authority_info& info ){
                info.is_authorized = false;
                info.url           = url;
                info.location      = location;
            });
        } else {
            _identity_authorities.emplace( identity_authority, [&]( identity_authority_info& info ){
                info.owner         = identity_authority;
                info.is_authorized = false;
                info.url           = url;
                info.location      = location;
            });
        }

        // charge transaction fee if not signed by system contract owner
        if (!has_auth(_self)) {
            INLINE_ACTION_SENDER(yosemite::ntoken, transfer)
                    (N(yx.ntoken), {{identity_authority, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                     { identity_authority, YOSEMITE_SYSTEM_ACCOUNT, yx_asset{YOSEMITE_REG_ID_AUTH_DEPO_TX_FEE, 0}, identity_authority, "txfee regidauth" });
        }
    }

    void system_contract::authidauth( const account_name identity_authority ) {
        require_auth( _self );

        auto idauth = _identity_authorities.find( identity_authority );

        eosio_assert( idauth != _identity_authorities.end(), "not found registered identity authority" );

        _identity_authorities.modify( idauth, 0, [&]( identity_authority_info& info ){
            info.is_authorized = true;
        });
    }

    void system_contract::rmvidauth( const account_name identity_authority ) {
       require_auth( _self );

       auto idauth = _identity_authorities.find( identity_authority );

       eosio_assert( idauth != _identity_authorities.end(), "not found registered identity authority" );

        _identity_authorities.modify( idauth, 0, [&]( identity_authority_info& info ){
           info.is_authorized = false;
       });
    }

} //namespace yosemitesys
