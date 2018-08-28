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

namespace eosio {
    using namespace appbase;

    namespace event_notification_api {
        struct event_request {
            string req_id;
            string name; // subscribe, unsubscribe, tx_irreversibility, ...
            fc::variant params; // json object for each operation
        };

        struct event_response {
            string req_id;
            string name; // tx_irreversibility, ...
            fc::variant response;
        };

        struct error_response {
            uint16_t code;
            string message;
        };

        struct subscribe_request {
            optional<account_name> subscriber;
        };

        struct tx_irreversibility_request {
            transaction_id_type tx_id;
            optional<uint32_t> block_num_hint;
        };

        struct tx_irreversibility_response {
            transaction_id_type tx_id;
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
        unique_ptr<class event_notification_plugin_impl> my;
    };

}

FC_REFLECT(eosio::event_notification_api::event_request, (req_id)(name)(params))
FC_REFLECT(eosio::event_notification_api::event_response, (req_id)(name)(response))
FC_REFLECT(eosio::event_notification_api::error_response, (code)(message))

FC_REFLECT(eosio::event_notification_api::subscribe_request, (subscriber))

FC_REFLECT(eosio::event_notification_api::tx_irreversibility_request, (tx_id)(block_num_hint))
FC_REFLECT(eosio::event_notification_api::tx_irreversibility_response, (tx_id))

