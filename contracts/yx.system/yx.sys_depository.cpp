#include "yx.system.hpp"

#include <yosemitelib/system_accounts.hpp>
#include <yosemitelib/transaction_fee.hpp>

#include <eosiolib/dispatcher.hpp>
#include "../yx.ntoken/yx.ntoken.hpp"

namespace yosemitesys {

    using yosemite::yx_asset;

    void system_contract::regsysdepo( const account_name depository, const std::string& url, uint16_t location ) {

        eosio_assert( url.size() < 512, "url too long" );
        require_auth( depository );

        auto depo = _sys_depositories.find( depository );

        if ( depo != _sys_depositories.end() ) {
            _sys_depositories.modify( depo, depository, [&]( sys_depository_info& info ){
                info.is_authorized = false;
                info.url           = url;
                info.location      = location;
            });
        } else {
            _sys_depositories.emplace( depository, [&]( sys_depository_info& info ){
                info.owner         = depository;
                info.is_authorized = false;
                info.url           = url;
                info.location      = location;
            });
        }

        // pay transaction fee if not signed by system contract owner
        if (!has_auth(_self)) {
            const asset& tx_fee = yosemite::get_transaction_fee(YOSEMITE_TX_FEE_OP_NAME_REG_SYS_DEPO);
            INLINE_ACTION_SENDER(yosemite::ntoken, payfee)
                    (N(yx.ntoken), {{depository, N(active)}, {YOSEMITE_SYSTEM_ACCOUNT, N(active)}},
                     {depository, tx_fee});
        }
    }

    void system_contract::authsysdepo( const account_name depository ) {
        require_auth( _self );

        auto depo = _sys_depositories.find( depository );

        eosio_assert( depo != _sys_depositories.end(), "not found registered system depository" );

        _sys_depositories.modify( depo, 0, [&]( sys_depository_info& info ){
            info.is_authorized = true;
        });
    }

    void system_contract::rmvsysdepo( const account_name depository ) {
       require_auth( _self );

       auto depo = _sys_depositories.find( depository );

       eosio_assert( depo != _sys_depositories.end(), "not found registered system depository" );

       _sys_depositories.modify( depo, 0, [&]( sys_depository_info& info ){
           info.is_authorized = false;
       });
    }

} //namespace yosemitesys
