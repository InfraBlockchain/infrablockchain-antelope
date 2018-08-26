/**
 *  @file
 *  @copyright defined in yosemite/LICENSE
 */
#pragma once

#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/http_plugin/http_plugin.hpp>
#include <appbase/application.hpp>

namespace eosio {
    using namespace appbase;

    namespace event_notification_api {
        struct operation {
            string name; // subscribe, unsubscribe, ...
            fc::variant params; // json object for each operation
        };

        struct subscribe_request {
            optional<account_name> subscriber;
        };

        struct tx_irreversibility_request {
            transaction_id_type tx_id;
        };
    }

    class event_notification_plugin : public appbase::plugin<event_notification_plugin> {
    public:
        APPBASE_PLUGIN_REQUIRES((chain_plugin)(http_plugin))

        event_notification_plugin();
        virtual ~event_notification_plugin();

        void set_program_options(options_description &, options_description &cfg) override;
        void plugin_initialize(const variables_map &options);
        void plugin_startup();
        void plugin_shutdown();

        template <typename SocketType>
        void handle_message(ws_connection<SocketType> ws_conn, ws_message<SocketType> ws_msg);

    private:
        std::unordered_set<size_t> subscribed_connections{};

        template <typename SocketType>
        void subscribe(ws_connection<SocketType> ws_conn, event_notification_api::subscribe_request &&params);

        template <typename SocketType>
        void unsubscribe(ws_connection <SocketType> ws_conn);

        template <typename SocketType>
        void tx_irreversibility(ws_connection<SocketType> ws_conn, event_notification_api::tx_irreversibility_request &&params);
    };

}

FC_REFLECT(eosio::event_notification_api::operation, (name)(params))
FC_REFLECT(eosio::event_notification_api::subscribe_request, (subscriber))