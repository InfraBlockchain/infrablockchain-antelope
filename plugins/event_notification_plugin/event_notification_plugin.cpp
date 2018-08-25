/**
 *  @file
 *  @copyright defined in yosemite/LICENSE
 */
#include <eosio/event_notification_plugin/event_notification_plugin.hpp>
#include <fc/exception/exception.hpp>
#include <functional>

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
        http.add_ws_handler("/v1/event_notification/subscribe",
                std::bind(&eosio::event_notification_plugin::subscribe<basic_socket_endpoint>, this, std::placeholders::_1, std::placeholders::_2));
        http.add_wss_handler("/v1/event_notification/subscribe",
                std::bind(&eosio::event_notification_plugin::subscribe<tls_socket_endpoint>, this, std::placeholders::_1, std::placeholders::_2));
    }

    void event_notification_plugin::plugin_shutdown() {
    }

    template <typename SocketType>
    void event_notification_plugin::subscribe(ws_connection<SocketType> ws_conn, ws_message<SocketType> ws_msg) {
        close_ws_connection<SocketType>(ws_conn, status::normal, "handle_websocket_message called");
    }

} // namepsace eosio
