/**
 *  @file
 *  @copyright defined in yosemite/LICENSE
 */
#pragma once

#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/http_plugin/http_plugin.hpp>
#include <eosio/history_plugin/history_plugin.hpp>
#include <appbase/application.hpp>
#include <fc/any.hpp>

namespace yosemite {
    using namespace appbase;
    using namespace eosio;

    namespace event_notification_api {
        struct event_base {
            string req_id;
            string name; // subscribe, unsubscribe, tx_irreversibility, ...
        };

        struct error_response {
            string req_id;
            string name;
            uint16_t code;
            string message;
        };

        struct subscribe_request {
            string req_id;
            string name;
            optional<account_name> subscriber;
        };

        struct tx_irreversibility_request {
            string req_id;
            string name;
            transaction_id_type tx_id;
            optional<uint32_t> block_num_hint;
        };

        struct tx_irreversibility_response {
            string req_id;
            string name;
            transaction_id_type tx_id;
            bool dropped;
        };
    }

    class event_notification_plugin : public appbase::plugin<event_notification_plugin> {
    public:
        APPBASE_PLUGIN_REQUIRES((chain_plugin)(http_plugin)(history_plugin))

        event_notification_plugin();
        virtual ~event_notification_plugin();

        void set_program_options(options_description &, options_description &cfg) override;
        void plugin_initialize(const variables_map &options);
        void plugin_startup();
        void plugin_shutdown();

    private:
        std::unique_ptr<class event_notification_plugin_impl> my;
    };

}

FC_REFLECT(yosemite::event_notification_api::event_base, (req_id)(name))
FC_REFLECT(yosemite::event_notification_api::error_response, (req_id)(name)(code)(message))
FC_REFLECT(yosemite::event_notification_api::subscribe_request, (req_id)(name)(subscriber))
FC_REFLECT(yosemite::event_notification_api::tx_irreversibility_request, (req_id)(name)(tx_id)(block_num_hint))
FC_REFLECT(yosemite::event_notification_api::tx_irreversibility_response, (req_id)(name)(tx_id)(dropped))

