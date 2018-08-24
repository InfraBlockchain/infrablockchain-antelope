configure_file(${CMAKE_CURRENT_SOURCE_DIR}/native_token_symbol.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/native_token_symbol.hpp)

add_wast_library(TARGET yosemitelib
  INCLUDE_FOLDERS "${STANDARD_INCLUDE_FOLDERS}" ${CMAKE_SOURCE_DIR}/externals/magic_get/include
  DESTINATION_FOLDER ${CMAKE_CURRENT_BINARY_DIR}
)

if ($ENV{CLION_IDE})
    set(CONTRACT_MODULE_NAME "yosemitelib")
    message( STATUS "CLION - contracts/${CONTRACT_MODULE_NAME}" )
    file(GLOB HEADERS "*.hpp" "*.h")
    file(GLOB SOURCES "*.cpp")
    add_library( contracts_${CONTRACT_MODULE_NAME}
            ${SOURCES}
            ${HEADERS} )
    target_include_directories(contracts_${CONTRACT_MODULE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
endif ()
