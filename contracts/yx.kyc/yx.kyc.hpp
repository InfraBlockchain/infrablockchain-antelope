#pragma once

#include <eosiolib/yx_kyc.hpp>
#include <yx.token/yx.token.hpp>
#include <string>

namespace yosemite {

    using namespace eosio;
    using std::string;
    using boost::container::flat_map;
    using boost::container::flat_set;

    class kyc : public contract {
    public:
        explicit kyc(account_name self) : contract(self) {
        }

        void setinfo(account_name account, account_name depository, uint32_t authvector, const string &addendum);
        void delinfo(account_name account);
        void upauthvector(account_name account, uint32_t authvector);
        void upaddendum(account_name account, const string &addendum);

        inline uint32_t get_kyc_authvector(const account_name &account) const;
    };

    /* scope = self */
    struct kyc_holder {
        account_name owner;
        account_name depository;
        uint32_t authvector;
        vector<char> addendum;

        uint64_t primary_key() const { return owner; }
    };

    typedef eosio::multi_index<N(kyc), kyc_holder> kyc_index;

    uint32_t kyc::get_kyc_authvector(const account_name &account) const {
        // for all system accounts
        if (account == FEEDIST_ACCOUNT_NAME) {
            return KYC_AUTHVECTOR_MAX_VALUE;
        }

        kyc_index kycdb{get_self(), get_self()};
        auto info = kycdb.find(account);
        eosio_assert(static_cast<uint32_t>(info != kycdb.end()), "account's KYC information does not exist");
        return info->authvector;
    }
}
