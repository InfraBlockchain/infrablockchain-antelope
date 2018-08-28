/**
 *  @file
 *  @copyright defined in yosemite/LICENSE
 */
#include <eosio/event_notification_plugin/event_notification_plugin.hpp>
#include <fc/exception/exception.hpp>
#include <fc/io/json.hpp>

namespace eosio {
    using namespace websocketpp::close;
    using std::unordered_multimap;

    static appbase::abstract_plugin& _event_notification_plugin = app().register_plugin<event_notification_plugin>();

    class event_notification_plugin_impl {
    public:
        chain_plugin *chain_plug = nullptr;
        history_plugin *history_plug = nullptr;
        //unordered_set<size_t> subscribed_connections{};

        unordered_multimap<ws_connection<basic_socket_endpoint>, transaction_id_type,
                ws_connection_hash<basic_socket_endpoint>, ws_connection_equal<basic_socket_endpoint>> conn_to_tx_id_map;
        unordered_multimap<ws_connection<tls_socket_endpoint>, transaction_id_type,
                ws_connection_hash<tls_socket_endpoint>, ws_connection_equal<tls_socket_endpoint>> tls_conn_to_tx_id_map;
        unordered_multimap<transaction_id_type, ws_connection<basic_socket_endpoint>> tx_irreversibility_waits;
        unordered_multimap<transaction_id_type, ws_connection<tls_socket_endpoint>> tx_irreversibility_waits_tls;

        event_notification_plugin_impl() {}
        ~event_notification_plugin_impl() {}

        void clear_maps() {
            conn_to_tx_id_map.clear();
            tls_conn_to_tx_id_map.clear();
            tx_irreversibility_waits.clear();
            tx_irreversibility_waits_tls.clear();
        }

        template<typename SocketType>
        typename std::enable_if<std::is_same<SocketType, basic_socket_endpoint>::value, decltype(conn_to_tx_id_map) &>::type
        get_conn_to_tx_id_map() {
            return conn_to_tx_id_map;
        }

        template<typename SocketType>
        typename std::enable_if<std::is_same<SocketType, tls_socket_endpoint>::value, decltype(tls_conn_to_tx_id_map) &>::type
        get_conn_to_tx_id_map() {
            return tls_conn_to_tx_id_map;
        }

        template<typename SocketType>
        typename std::enable_if<std::is_same<SocketType, basic_socket_endpoint>::value, decltype(tx_irreversibility_waits) &>::type
        get_tx_irreversibility_waits_map() {
            return tx_irreversibility_waits;
        }

        template<typename SocketType>
        typename std::enable_if<std::is_same<SocketType, tls_socket_endpoint>::value, decltype(tx_irreversibility_waits_tls) &>::type
        get_tx_irreversibility_waits_map() {
            return tx_irreversibility_waits_tls;
        }

        template <typename SocketType>
        void handle_message(ws_connection<SocketType> ws_conn, ws_message<SocketType> ws_msg) {
            auto param = fc::json::from_string(ws_msg->get_payload()).template as<event_notification_api::event_request>();
            if (param.name == "tx_irreversibility") {
                tx_irreversibility<SocketType>(ws_conn, std::move(param.params.template as<event_notification_api::tx_irreversibility_request>()));
            } else if (param.name == "subscribe") {
                subscribe<SocketType>(ws_conn, std::move(param.params.template as<event_notification_api::subscribe_request>()));
            } else if (param.name == "unsubscribe") {
                unsubscribe<SocketType>(ws_conn);
            } else {
                ws_conn->close(status::invalid_subprotocol_data, "unknown operation");
            }
        }

        template <typename SocketType>
        void subscribe(ws_connection<SocketType> ws_conn, event_notification_api::subscribe_request &&params) {
            /* nothing to do yet */
        }

        template <typename SocketType>
        void unsubscribe(ws_connection<SocketType> ws_conn) {
            unregister_ws_connection<SocketType>(ws_conn);
            ws_conn->close(status::normal, ""); // good bye~
        }

        template <typename SocketType>
        void register_tx_irreversibility(ws_connection<SocketType> ws_conn, transaction_id_type tx_id) {
            auto &conn_to_tx_id_map = get_conn_to_tx_id_map<SocketType>();
            auto range = conn_to_tx_id_map.equal_range(ws_conn);
            for (auto itr = range.first; itr != range.second; ++itr) {
                if (tx_id == itr->second) {
                    EOS_THROW(duplicated_tx_id_exception, "duplicated transaction : ${id}", ("id", tx_id));
                }
            }
            conn_to_tx_id_map.insert(std::make_pair(ws_conn, tx_id));

            auto &waits_map = get_tx_irreversibility_waits_map<SocketType>();
            waits_map.insert(std::make_pair(tx_id, ws_conn));
        }

        template <typename SocketType>
        void unregister_ws_connection(ws_connection<SocketType> ws_conn) {
            auto &conn_to_tx_id_map = get_conn_to_tx_id_map<SocketType>();
            auto &waits_map = get_tx_irreversibility_waits_map<SocketType>();

            auto range = conn_to_tx_id_map.equal_range(ws_conn);
            for (auto itr = range.first; itr != range.second;) {
                // get the range iterator to ws_connections via tx_id
                auto waits_range = waits_map.equal_range(itr->second);
                for (auto waits_itr = waits_range.first; waits_itr != waits_range.second; waits_itr++) {
                    if (waits_itr->second == ws_conn) {
                        waits_map.erase(waits_itr);
                        break;
                    }
                }
                itr = conn_to_tx_id_map.erase(itr);
            }
        }

        template <typename SocketType>
        void tx_irreversibility(ws_connection<SocketType> ws_conn, event_notification_api::tx_irreversibility_request &&params) {
            try {
                history_apis::read_only::get_transaction_params get_tx_params;
                get_tx_params.id = params.tx_id;
                get_tx_params.block_num_hint = params.block_num_hint;

                auto get_tx_result = history_plug->get_read_only_api().get_transaction(get_tx_params);
                if (get_tx_result.block_num <= get_tx_result.last_irreversible_block) {
                    event_notification_api::event_response result;
                    result.name = "tx_irreversibility";
                    to_variant(event_notification_api::tx_irreversibility_response{params.tx_id}, result.response);
                    ws_conn->send(fc::json::to_string(result));

                    return;
                }

                register_tx_irreversibility<SocketType>(ws_conn, params.tx_id); // will be notified by irreversible_block
            } catch (const tx_not_found &ex) {
                chain::controller &chain = chain_plug->chain();

                if (chain.is_known_unexpired_transaction(params.tx_id)) {
                    register_tx_irreversibility<SocketType>(ws_conn, params.tx_id); // will be notified by irreversible_block
                } else {
                    event_notification_api::error_response result{500, ex.to_string()};
                    ws_conn->send(fc::json::to_string(result));
                }
            } catch (const fc::exception &ex) {
                auto error_info = ex.to_string();
                elog("Exception in handling tx_irreversibility : ${s}", ("s", error_info));

                event_notification_api::error_response result{500, error_info};
                ws_conn->send(fc::json::to_string(result));
            } catch (const std::exception &ex) {
                elog("Exception in handling tx_irreversibility : ${s}", ("s", ex.what()));

                event_notification_api::error_response result{500, ex.what()};
                ws_conn->send(fc::json::to_string(result));
            } catch (...) {
                elog("Undefined Exception in handling tx_irreversibility");

                event_notification_api::error_response result{500, "undefined error"};
                ws_conn->send(fc::json::to_string(result));
            }
            /* NEVER DO ANYTHING FOR WS_CONN HERE */
        }

        void on_irreversible_block(const block_state_ptr &block) {
            for (auto &tx_meta : block->trxs) {
                auto waits_range = tx_irreversibility_waits.equal_range(tx_meta->id);
                for (auto waits_itr = waits_range.first; waits_itr != waits_range.second;) {
                    event_notification_api::event_response result;
                    result.name = "tx_irreversibility";
                    to_variant(event_notification_api::tx_irreversibility_response{tx_meta->id}, result.response);
                    waits_itr->second->send(fc::json::to_string(result));

                    //remove tx_id from conn_to_tx_id_map
                    auto range = conn_to_tx_id_map.equal_range(waits_itr->second);
                    for (auto itr = range.first; itr != range.second; ++itr) {
                        if (itr->second == tx_meta->id) {
                            conn_to_tx_id_map.erase(itr);
                            break;
                        }
                    }

                    waits_itr = tx_irreversibility_waits.erase(waits_itr);
                }

                auto tls_waits_range = tx_irreversibility_waits_tls.equal_range(tx_meta->id);
                for (auto waits_itr = tls_waits_range.first; waits_itr != tls_waits_range.second;) {
                    event_notification_api::event_response result;
                    result.name = "tx_irreversibility";
                    to_variant(event_notification_api::tx_irreversibility_response{tx_meta->id}, result.response);
                    waits_itr->second->send(fc::json::to_string(result));

                    //remove tx_id from conn_to_tx_id_map
                    auto range = tls_conn_to_tx_id_map.equal_range(waits_itr->second);
                    for (auto itr = range.first; itr != range.second; ++itr) {
                        if (itr->second == tx_meta->id) {
                            tls_conn_to_tx_id_map.erase(itr);
                            break;
                        }
                    }

                    waits_itr = tx_irreversibility_waits_tls.erase(waits_itr);
                }
            }
        }
    };

    event_notification_plugin::event_notification_plugin() : my(new event_notification_plugin_impl()) {}
    event_notification_plugin::~event_notification_plugin() {}

    void event_notification_plugin::set_program_options(options_description&, options_description& cfg) {
    }

    void event_notification_plugin::plugin_initialize(const variables_map& options) {
        my->chain_plug = app().find_plugin<chain_plugin>();
        EOS_ASSERT(my->chain_plug, chain::missing_chain_plugin_exception, "");

        my->history_plug = app().find_plugin<history_plugin>();
        EOS_ASSERT(my->history_plug, chain::missing_history_plugin_exception, "");

        my->clear_maps();
    }

    void event_notification_plugin::plugin_startup() {
        auto& http = app().get_plugin<http_plugin>();
        http.add_ws_handler("/v1/event_notification",
                            boost::bind(&event_notification_plugin_impl::handle_message<basic_socket_endpoint>, my.get(), _1, _2));
        http.add_wss_handler("/v1/event_notification",
                             boost::bind(&event_notification_plugin_impl::handle_message<tls_socket_endpoint>, my.get(), _1, _2));
        http.set_ws_connection_close_handler(
              boost::bind(&event_notification_plugin_impl::unregister_ws_connection<basic_socket_endpoint>, my.get(), _1));
        http.set_wss_connection_close_handler(
              boost::bind(&event_notification_plugin_impl::unregister_ws_connection<tls_socket_endpoint>, my.get(), _1));

        chain::controller &cc = my->chain_plug->chain();
        cc.irreversible_block.connect(boost::bind(&event_notification_plugin_impl::on_irreversible_block, my.get(), _1));
    }

    void event_notification_plugin::plugin_shutdown() {
        my->clear_maps();
    }

} // namepsace eosio
