#pragma once

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

        void setinfo(account_name account, account_name depository, uint32_t level, const string &addendum);
        void delinfo(account_name account);
        void uplevel(account_name account, uint32_t level);
        void upaddendum(account_name account, const string &addendum);

        inline uint32_t get_kyc_level(const account_name &account) const;
    };

    /* scope = self */
    struct kyc_holder {
        account_name owner;
        account_name depository;
        uint32_t level;
        vector<char> addendum;

        uint64_t primary_key() const { return owner; }
    };

    typedef eosio::multi_index<N(kyc), kyc_holder> kyc_index; //TODO:need depository secondary index?

    uint32_t kyc::get_kyc_level(const account_name &account) const {
        kyc_index kycdb{get_self(), get_self()};
        auto info = kycdb.find(account);
        eosio_assert(static_cast<uint32_t>(info != kycdb.end()), "account's KYC information does not exist");
        return info->level;
    }
}
