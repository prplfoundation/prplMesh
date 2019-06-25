configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/prplmesh_utils.sh.in"
        "${CMAKE_CURRENT_BINARY_DIR}/prplmesh_utils.sh"
        @ONLY
        )

set(scripts
    ${CMAKE_CURRENT_BINARY_DIR}/prplmesh_utils.sh
    beerocks_utils.sh
    beerocks_endurance_test.sh
    beerocks_watchdog.sh
    udhcpc.script.alt)

install(PROGRAMS ${scripts} DESTINATION scripts)
file(COPY ${scripts} DESTINATION ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/scripts)
