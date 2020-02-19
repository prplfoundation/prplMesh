# Configure the prplMesh helper script
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/prplmesh_utils.sh.in"
        "${CMAKE_CURRENT_BINARY_DIR}/prplmesh_utils.sh"
        @ONLY
        )

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/beerocks_watchdog.sh.in"
        "${CMAKE_CURRENT_BINARY_DIR}/beerocks_watchdog.sh"
        @ONLY
        )

set(scripts ${CMAKE_CURRENT_BINARY_DIR}/prplmesh_utils.sh)

if ((NOT TARGET_PLATFORM STREQUAL "linux") AND (NOT TARGET_PLATFORM STREQUAL "econet" ))
list(APPEND scripts ${CMAKE_CURRENT_BINARY_DIR}/beerocks_watchdog.sh)
endif()

install(PROGRAMS ${scripts} DESTINATION scripts)
file(COPY ${scripts} DESTINATION ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/scripts)
