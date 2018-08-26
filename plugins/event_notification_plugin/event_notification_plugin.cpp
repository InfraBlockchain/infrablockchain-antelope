/**
 *  @file
 *  @copyright defined in yosemite/LICENSE
 */
#include <functional>
#include <eosio/event_notification_plugin/event_notification_plugin.hpp>
#include <fc/exception/exception.hpp>
#include <fc/io/json.hpp>

namespace eosio {
    using namespace websocketpp::close;

    static appbase::abstract_plugin& _event_notification_plugin = app().register_plugin<event_notification_plugin>();

    event_notification_plugin::event_notification_plugin() {}
    event_notification_plugin::~event_notification_plugin() {}

    void event_notification_plugin::set_program_options(options_description&, options_description& cfg) {
    }

    void event_notification_plugin::plugin_initialize(const variables_map& options) {
    }

    void event_notification_plugin::plugin_startup() {
        auto& http = app().get_plugin<http_plugin>();
        http.add_ws_handler("/v1/event_notification",
                std::bind(&eosio::event_notification_plugin::handle_message<basic_socket_endpoint>, this, std::placeholders::_1, std::placeholders::_2));
        http.add_wss_handler("/v1/event_notification",
                std::bind(&eosio::event_notification_plugin::handle_message<tls_socket_endpoint>, this, std::placeholders::_1, std::placeholders::_2));
    }

    void event_notification_plugin::plugin_shutdown() {
    }

    template <typename SocketType>
    void event_notification_plugin::handle_message(ws_connection<SocketType> ws_conn, ws_message<SocketType> ws_msg) {
        auto param = fc::json::from_string(ws_msg->get_payload()).template as<event_notification_api::operation>();
        if (param.name == "subscribe") {
            subscribe<SocketType>(ws_conn, std::move(param.params.template as<event_notification_api::subscribe_request>()));
        } else if (param.name == "unsubscribe") {
            unsubscribe<SocketType>(ws_conn);
        } else if (param.name == "tx_irreversibility") {
            tx_irreversibility<SocketType>(ws_conn, std::move(param.params.template as<event_notification_api::tx_irreversibility_request>()));
        } else {
            ws_conn->close(status::invalid_subprotocol_data, "unknown operation");
        }
    }

    template <typename SocketType>
    void event_notification_plugin::subscribe(ws_connection<SocketType> ws_conn, event_notification_api::subscribe_request &&params) {
        /* nothing to do yet */
    }

    template <typename SocketType>
    void event_notification_plugin::unsubscribe(ws_connection <SocketType> ws_conn) {
        ws_conn->close(status::normal, ""); // good bye~
    }

    template <typename SocketType>
    void event_notification_plugin::tx_irreversibility(ws_connection<SocketType> ws_conn, event_notification_api::tx_irreversibility_request &&params) {
        /* nothing to do yet */
    }

} // namepsace eosio
