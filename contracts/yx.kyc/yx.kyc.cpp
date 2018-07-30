/**
 *  @copyright defined in LICENSE.txt
 */

#include "yx.kyc.hpp"
#include <yosemitelib/system_depository.hpp>

namespace yosemite {
    void kyc::setinfo(account_name account, account_name depository, uint32_t kycvector, const string &addendum) {
        eosio_assert(static_cast<uint32_t>(addendum.size() <= 256), "addendum has more than 256 bytes");
        eosio_assert(static_cast<uint32_t>(is_account(account)), "account does not exist");
        eosio_assert(static_cast<uint32_t>(kycvector <= KYC_VECTOR_MAX_VALUE), "invalid kycvector value");

        //TODO:call yx.system contract
        eosio_assert(static_cast<uint32_t>(is_authorized_sys_depository(depository)),
                     "account is not registered as the depository");

        require_auth(depository);

        kyc_index kycdb{get_self(), get_self()};
        eosio_assert(static_cast<uint32_t>(kycdb.find(account) == kycdb.end()), "account's KYC information already exists");

        kycdb.emplace(get_self(), [&](auto &i) {
            i.owner = account;
            i.depository = depository;
            i.kycvector = kycvector;
            std::copy(addendum.begin(), addendum.end(), std::back_inserter(i.addendum));
        });
    }

    void kyc::delinfo(account_name account) {
        kyc_index kycdb{get_self(), get_self()};
        auto info = kycdb.find(account);
        eosio_assert(static_cast<uint32_t>(info != kycdb.end()), "account's KYC information does not exist");

        require_auth(info->depository);

        kycdb.erase(info);
    }

    void kyc::upkycvector(account_name account, uint32_t kycvector) {
        eosio_assert(static_cast<uint32_t>(kycvector <= KYC_VECTOR_MAX_VALUE), "invalid kycvector value");

        kyc_index kycdb{get_self(), get_self()};
        auto info = kycdb.find(account);
        eosio_assert(static_cast<uint32_t>(info != kycdb.end()), "account's KYC information does not exist");

        require_auth(info->depository);
        eosio_assert(static_cast<uint32_t>(info->kycvector != kycvector), "the same value");

        kycdb.modify(info, 0, [&](auto &i) {
            i.kycvector = kycvector;
        });
    }

    void kyc::upaddendum(account_name account, const string &addendum) {
        eosio_assert(static_cast<uint32_t>(addendum.size() <= 256), "addendum has more than 256 bytes");

        kyc_index kycdb{get_self(), get_self()};
        auto info = kycdb.find(account);
        eosio_assert(static_cast<uint32_t>(info != kycdb.end()), "account's KYC information does not exist");

        require_auth(info->depository);
        eosio_assert(static_cast<uint32_t>(!std::equal(info->addendum.begin(), info->addendum.end(), addendum.begin())), "the same value");

        kycdb.modify(info, 0, [&](auto &i) {
            i.addendum.clear();
            std::copy(addendum.begin(), addendum.end(), std::back_inserter(i.addendum));
        });
    }
}

EOSIO_ABI(yosemite::kyc, (setinfo)(delinfo)(upkycvector)(upaddendum))
