#include "yx.kyc.hpp"
#include <yx.ntoken/yx.ntoken.hpp>

namespace yosemite {
    void kyc::setinfo(account_name account, account_name depository, uint32_t level, const string &addendum) {
        eosio_assert(static_cast<uint32_t>(addendum.size() <= 256), "addendum has more than 256 bytes");
        eosio_assert(static_cast<uint32_t>(is_account(account)), "account does not exist");

        //TODO:fee

        require_auth(depository);
        eosio_assert(static_cast<uint32_t>(yosemite::ntoken(N(yx.ntoken)).is_kyc_depository(depository)),
                     "depository account is not the native token depository");

        kyc_index kycdb{get_self(), get_self()};
        eosio_assert(static_cast<uint32_t>(kycdb.find(account) == kycdb.end()), "account's KYC information already exists");

        kycdb.emplace(get_self(), [&](auto &i) {
            i.owner = account;
            i.depository = depository;
            i.level = level;
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

    void kyc::uplevel(account_name account, uint32_t level) {
        kyc_index kycdb{get_self(), get_self()};
        auto info = kycdb.find(account);
        eosio_assert(static_cast<uint32_t>(info != kycdb.end()), "account's KYC information does not exist");

        require_auth(info->depository);

        kycdb.modify(info, 0, [&](auto &i) {
            i.level = level;
        });
    }

    void kyc::upaddendum(account_name account, const string &addendum) {
        eosio_assert(static_cast<uint32_t>(addendum.size() <= 256), "addendum has more than 256 bytes");

        kyc_index kycdb{get_self(), get_self()};
        auto info = kycdb.find(account);
        eosio_assert(static_cast<uint32_t>(info != kycdb.end()), "account's KYC information does not exist");

        require_auth(info->depository);

        kycdb.modify(info, 0, [&](auto &i) {
            i.addendum.clear();
            std::copy(addendum.begin(), addendum.end(), std::back_inserter(i.addendum));
        });
    }
}

EOSIO_ABI(yosemite::kyc, (setinfo)(delinfo)(uplevel)(upaddendum))
