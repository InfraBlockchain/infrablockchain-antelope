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
        typename std::enable_if<std::is_same<SocketType, basic_socket_endpoint>::value, void>::type
        close_ws_connection(ws_connection<SocketType> ws_conn, websocketpp::close::status::value code, const string &reason = "") {
            auto& http = app().get_plugin<http_plugin>();
            http.close_ws_connection(std::move(ws_conn), code, reason);
        }

        template <typename SocketType>
        typename std::enable_if<std::is_same<SocketType, tls_socket_endpoint>::value, void>::type
        close_ws_connection(ws_connection<SocketType> ws_conn, websocketpp::close::status::value code, const string &reason = "") {
            auto& http = app().get_plugin<http_plugin>();
            http.close_wss_connection(std::move(ws_conn), code, reason);
        }

        template <typename SocketType>
        void subscribe(ws_connection<SocketType> ws_conn, ws_message<SocketType> ws_msg);
    };

}
