configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/beerocks_controller_version.in"
        "${CMAKE_CURRENT_BINARY_DIR}/beerocks_controller_version"
        )
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/beerocks_controller.conf.in"
        "${CMAKE_CURRENT_BINARY_DIR}/beerocks_controller.conf"
        )

set(confs 
    ${CMAKE_CURRENT_BINARY_DIR}/beerocks_controller.conf
    ${CMAKE_CURRENT_BINARY_DIR}/beerocks_controller_version)

install(FILES ${confs} DESTINATION config)
file(COPY ${confs} DESTINATION "${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/config")