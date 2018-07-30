/**
 *  @copyright defined in LICENSE.txt
 */

#pragma once

#include <yosemitelib/yx_kyc.hpp>
#include <yosemitelib/yx_fee.hpp>
#include <string>

namespace yosemite {

    using namespace eosio;
    using std::string;

    class kyc : public contract {
    public:
        explicit kyc(account_name self) : contract(self) {
        }

        void setinfo(account_name account, account_name depository, uint32_t kycvector, const string &addendum);
        void delinfo(account_name account);
        void upkycvector(account_name account, uint32_t kycvector);
        void upaddendum(account_name account, const string &addendum);

        static inline uint32_t get_kyc_vector(const account_name &account, bool assert = true);
    };

    /* scope = self */
    struct kyc_holder {
        account_name owner;
        account_name depository;
        uint32_t kycvector;
        vector<char> addendum;

        uint64_t primary_key() const { return owner; }
    };

    typedef eosio::multi_index<N(kyc), kyc_holder> kyc_index;

    uint32_t kyc::get_kyc_vector(const account_name &account, bool assert) {
        // for all system accounts
        if (account == FEEDIST_ACCOUNT_NAME) {
            return KYC_VECTOR_MAX_VALUE;
        }

        kyc_index kycdb{N(yx.kyc), N(yx.kyc)};
        auto info = kycdb.find(account);
        if (assert) {
            eosio_assert(static_cast<uint32_t>(info != kycdb.end()), "account's KYC information does not exist");
        } else if (info == kycdb.end()) {
            return KYC_VECTOR_NO_AUTH;
        }
        return info->kycvector;
    }
}
