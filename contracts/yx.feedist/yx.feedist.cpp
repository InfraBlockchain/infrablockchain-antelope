#include "yx.feedist.hpp"
#include <eosiolib/transaction.hpp>
#include <eosiolib/chain.h>

namespace yosemite {
    namespace config {
        bool get(sysconfig &out, const account_name &self) {
            auto it = db_find_i64(self, self, N(config), sysconfig::key);
            if (it != -1) {
                auto size = db_get_i64(it, (char *) &out, sizeof(sysconfig));
                eosio_assert(static_cast<uint32_t>(size == sizeof(sysconfig)), "Wrong record size");
                return true;
            } else {
                return false;
            }
        }

        void store(const sysconfig &in, const account_name &self) {
            auto it = db_find_i64(self, self, N(config), sysconfig::key);
            if (it != -1) {
                db_update_i64(it, self, (const char *) &in, sizeof(sysconfig));
            } else {
                db_store_i64(self, N(config), self, sysconfig::key, (const char *) &in, sizeof(sysconfig));
            }
        }
    }

    void feedist::send_deferred_feedist_action(const uint32_t &delay_sec, const uint64_t &id) {
        action deferred_act{};
        deferred_act.account = get_self();
        deferred_act.name = N(dofeedist);
        deferred_act.authorization = vector<permission_level>{permission_level{get_self(), N(active)}};

        transaction tx(time_point_sec(now() + 60000 + delay_sec));
        tx.actions.emplace_back(deferred_act);
        tx.delay_sec = delay_sec;
        tx.send(id, get_self());
    }

    void feedist::startfeedist(uint32_t days) {
        require_auth(get_self());  //TODO:multisig yx.prods
        //eosio_assert(static_cast<uint32_t>(days >= sysconfig::MIN_FEEDIST_DAYS), "minimum 7 days are required");
        eosio_assert(static_cast<uint32_t>(days <= sysconfig::MAX_FEEDIST_DAYS), "maximum 60 days");

        sysconfig cfg{};

        //eosio_assert(static_cast<uint32_t>(!config::get(cfg, get_self())), "already started");
        //cfg.feedist_period_in_sec = sysconfig::DAY_TO_SEC * days;
        cfg.feedist_period_in_sec = days; // TODO:for test only
        cfg.remained_count = days; // TODO:days * 24

        auto id = cfg.next_id++;
        config::store(cfg, get_self());

        send_deferred_feedist_action(cfg.feedist_period_in_sec, id);
    }

    void feedist::dofeedist() {
        require_auth(get_self());

        constexpr size_t max_stack_buffer_size = 512;
        auto size = ::get_active_producers(nullptr, 0);
        auto active_prods = static_cast<account_name *>(max_stack_buffer_size < size ? malloc(size) : alloca(size));
        auto size2 = ::get_active_producers(active_prods, size);
        eosio_assert(static_cast<uint32_t>(size == static_cast<size_t>(size2)), "get_active_producers failed");

        const int active_prods_count = size / sizeof(account_name);
        print("size=", size, ", active_producer_count=", active_prods_count, "\n");
        eosio_assert(static_cast<uint32_t>(active_prods_count > 0), "illegal active producer count");

        int64_t balance = yosemite::token(N(yx.token)).get_total_native_token_balance(FEEDIST_ACCOUNT_NAME);
        if (balance > 0) { // TODO: Let's set minimum instead of 0
            print("yx.feedist native balance : ", balance, "\n");

            int64_t fee_to_distribute = balance / active_prods_count;
            if (fee_to_distribute > 0) {
                print("feedist balance : ", fee_to_distribute, "\n");

                feedist_index feedist_idx(get_self(), get_self());

                for (int i = 0; i < active_prods_count; ++i) {
                    printn(active_prods[i]);
                    print("\n");

                    const auto itr = feedist_idx.find(active_prods[i]);
                    if (itr == feedist_idx.end()) {
                        feedist_idx.emplace(get_self(), [&](auto& a) {
                            a.owner = active_prods[i];
                            a.fee_amount = fee_to_distribute;
                        });
                    } else {
                        feedist_idx.modify(itr, 0, [&](auto& a) {
                            a.fee_amount = fee_to_distribute;
                        });
                    }
                }
            }
        }

        sysconfig cfg{};
        eosio_assert(static_cast<uint32_t>(config::get(cfg, get_self())), "reading config failed from database");

        uint32_t remained_count = --cfg.remained_count;
        if (remained_count <= 0) {
            feedist_index feedist_idx(get_self(), get_self());
            yx_symbol native_symbol{NATIVE_TOKEN, 0};
            std::string memo{"feedist"};

            for (auto itr = feedist_idx.begin(); itr != feedist_idx.end(); ) {
                print("transfer feedist balance : ", itr->fee_amount, " to ");
                printn(itr->owner);
                print(" from ");
                printn(get_self());
                print("\n");

                INLINE_ACTION_SENDER(yosemite::token, transfer)
                        (N(yx.token), {get_self(), N(eosio.code)},
                         { get_self(), itr->owner, yx_asset{itr->fee_amount, native_symbol}, memo, get_self() });

                itr = feedist_idx.erase(itr);
            }
            cfg.remained_count = cfg.feedist_period_in_sec; // TODO: convert to hour
        }

        auto id = cfg.next_id++;
        config::store(cfg, get_self());

        // schedule this operation again by itself
        send_deferred_feedist_action(cfg.feedist_period_in_sec, id);
    }

    void feedist::stopfeedist() {
        require_auth(get_self()); //TODO:multisig yx.prods

        sysconfig cfg{};
        eosio_assert(static_cast<uint32_t>(config::get(cfg, get_self())), "reading config failed from database");
        eosio_assert(static_cast<uint32_t>(cancel_deferred(cfg.next_id - 1)), "feedist not scheduled");
    }
}

EOSIO_ABI(yosemite::feedist, (startfeedist)(dofeedist)(stopfeedist))
