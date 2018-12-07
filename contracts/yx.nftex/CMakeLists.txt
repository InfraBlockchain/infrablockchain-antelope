file(GLOB ABI_FILES "*.abi")
add_wast_executable(TARGET yx.nftex
        INCLUDE_FOLDERS "${STANDARD_INCLUDE_FOLDERS}"
        LIBRARIES libc++ libc eosiolib yosemitelib
        DESTINATION_FOLDER ${CMAKE_CURRENT_BINARY_DIR})
configure_file("${ABI_FILES}" "${CMAKE_CURRENT_BINARY_DIR}" COPYONLY)

if ($ENV{CLION_IDE})
    set(CONTRACT_MODULE_NAME "yx.nftex")
    message( STATUS "CLION - contracts/${CONTRACT_MODULE_NAME}" )
    file(GLOB HEADERS "*.hpp" "*.h")
    file(GLOB SOURCES "*.cpp")
    add_library( contracts_${CONTRACT_MODULE_NAME}
            ${SOURCES}
            ${HEADERS} )
    target_include_directories(contracts_${CONTRACT_MODULE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
endif ()