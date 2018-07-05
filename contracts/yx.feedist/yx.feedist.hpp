#pragma once

#include <yx.ntoken/yx.ntoken.hpp>
#include <string>

namespace yosemite {

    using namespace eosio;
    using std::string;
    using boost::container::flat_map;
    using boost::container::flat_set;

    class feedist : public contract {
    public:
        explicit feedist(account_name self) : contract(self) {
        }

        void startfeedist(uint32_t days);
        void dofeedist(); /* called only as deferred transaction */
        void stopfeedist();

    private:
        void send_deferred_feedist_action(const uint32_t &delay_sec, const uint64_t &id);
    };

    struct sysconfig {
        const static uint32_t INTERIM_DIVIEND_PERIOD_IN_SEC = 3600; /* 1 hour */
        const static uint32_t MIN_FEEDIST_DAYS = 7;
        const static uint32_t MAX_FEEDIST_DAYS = 60;
        const static uint32_t DAY_TO_SEC = 86400;

        constexpr static uint64_t key = N(sysconfig);

        uint64_t next_id = 0;
        uint32_t feedist_period_in_sec = 0;
        uint32_t remained_count = 0;
    };

    struct feedist_holder {
        account_name owner;
        int64_t fee_amount;

        uint64_t primary_key() const { return owner; }
    };

    typedef eosio::multi_index<N(feedist), feedist_holder> feedist_index;
}
