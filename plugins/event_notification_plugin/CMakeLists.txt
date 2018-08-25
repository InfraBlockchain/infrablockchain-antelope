file(GLOB HEADERS "include/eosio/event_notification_plugin/*.hpp")
add_library(event_notification_plugin event_notification_plugin.cpp ${HEADERS})

target_link_libraries(event_notification_plugin chain_plugin http_plugin appbase)
target_include_directories(event_notification_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
