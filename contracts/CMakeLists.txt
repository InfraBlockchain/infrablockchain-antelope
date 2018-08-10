# will be implictly used for any compilation unit if not overrided by SYSTEM_INCLUDE_FOLDERS parameter
# these directories go as -isystem <dir> to avoid warnings from code of third-party libraries
set(DEFAULT_SYSTEM_INCLUDE_FOLDERS ${CMAKE_SOURCE_DIR}/contracts/libc++/upstream/include ${CMAKE_SOURCE_DIR}/contracts/musl/upstream/include ${Boost_INCLUDE_DIR})

set(STANDARD_INCLUDE_FOLDERS ${CMAKE_SOURCE_DIR}/contracts ${CMAKE_BINARY_DIR}/contracts ${CMAKE_SOURCE_DIR}/externals/magic_get/include)

add_subdirectory(eosiolib)
add_subdirectory(musl)
add_subdirectory(libc++)

add_subdirectory(eosio.bios) # For internal test
add_subdirectory(eosio.token) # For internal test
add_subdirectory(eosio.msig)
add_subdirectory(eosio.sudo)

add_subdirectory(multi_index_test)
add_subdirectory(stltest)
add_subdirectory(test.inline)
add_subdirectory(test_api)
add_subdirectory(test_api_mem)
add_subdirectory(test_api_db)
add_subdirectory(test_api_multi_index)
add_subdirectory(test_ram_limit)

add_subdirectory(yosemitelib)
add_subdirectory(yx.system)
add_subdirectory(yx.txfee)
add_subdirectory(yx.identity)
add_subdirectory(yx.ntoken)
add_subdirectory(yx.token)
add_subdirectory(yx.dcontract)
add_subdirectory(yx.auction)
