configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/beerocks_agent.conf.in"
        "${CMAKE_CURRENT_BINARY_DIR}/beerocks_agent.conf"
        )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/beerocks_agent.conf DESTINATION config)
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/beerocks_agent.conf DESTINATION "${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/config")