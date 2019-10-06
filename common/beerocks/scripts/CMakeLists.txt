# Default shell commands
set(PS_CMD "ps -aux")
set(AWK_CMD "awk '{print $2}'")

# Platform specific definitions
if (TARGET_PLATFORM STREQUAL "linux")
    set(PLATFORM_DB_PATH ${CMAKE_INSTALL_PREFIX}/share)
    set(PLATFORM_DB_PATH_TEMP /tmp)
elseif (TARGET_PLATFORM STREQUAL "turris-omnia")
    set(PLATFORM_DB_PATH "/opt/beerocks/share")
    set(PLATFORM_DB_PATH_TEMP "/tmp/beerocks")
    set(PS_CMD "ps")
    set(AWK_CMD "awk '{print $1}'")
endif()

# Configure the prplMesh helper script
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/prplmesh_utils.sh.in"
        "${CMAKE_CURRENT_BINARY_DIR}/prplmesh_utils.sh"
        @ONLY
        )

set(scripts ${CMAKE_CURRENT_BINARY_DIR}/prplmesh_utils.sh)
if (NOT TARGET_PLATFORM STREQUAL "linux")
list(APPEND scripts beerocks_utils.sh
    beerocks_endurance_test.sh beerocks_watchdog.sh
    udhcpc.script.alt)
endif()

install(PROGRAMS ${scripts} DESTINATION scripts)
file(COPY ${scripts} DESTINATION ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/scripts)
