/**
 *  @copyright defined in LICENSE.txt
 */

#pragma once

#include <eosiolib/eosio.hpp>
#include <yosemitelib/identity.hpp>

#include <string>

namespace yosemite {

    /**
     * For each blockchain account, Identity Authority can manage below information
     *  - account type (e.g. normal, system, ...)
     *  - KYC info. (e.g. email, phone, real name, bank account, ...)
     *  - account state (e.g. blacklisted account, ...)
     *  - Identity-Authority specific data (arbitrary string data)
     *
     * Once an Identity Authority sets identity info. for an account, only the Identity Authority account can modified the identity info for the account.
     */
    class identity_contract : public eosio::contract {
    public:
        explicit identity_contract(account_name self) : contract(self) {
        }

        void setidinfo(const account_name account, const account_name identity_authority, uint16_t type, uint16_t kyc, uint32_t state, const std::string& data);
        void delidinfo(account_name account);
        void settype(account_name account, uint16_t type);
        void setkyc(account_name account, uint16_t kyc);
        void setstate(account_name account, uint32_t state);
        void setdata(account_name account, const std::string& data);
    };
}
