configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/beerocks_controller.conf.in"
        "${CMAKE_CURRENT_BINARY_DIR}/beerocks_controller.conf"
        )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/beerocks_controller.conf DESTINATION config)
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/beerocks_controller.conf DESTINATION "${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/config")