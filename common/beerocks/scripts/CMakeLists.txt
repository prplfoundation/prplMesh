set(scripts
    beerocks_utils.sh
    beerocks_endurance_test.sh
    beerocks_watchdog.sh
    udhcpc.script.alt)

install(PROGRAMS ${scripts} DESTINATION scripts)
file(COPY ${scripts} DESTINATION ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/scripts)