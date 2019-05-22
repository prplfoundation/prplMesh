configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/beerocks_agent_version.in"
        "${CMAKE_CURRENT_BINARY_DIR}/beerocks_agent_version"
        )
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/beerocks_agent.conf.in"
        "${CMAKE_CURRENT_BINARY_DIR}/beerocks_agent.conf"
        )

set(confs 
    ${CMAKE_CURRENT_BINARY_DIR}/beerocks_agent_version
    ${CMAKE_CURRENT_BINARY_DIR}/beerocks_agent.conf)

install(FILES ${confs} DESTINATION config)
file(COPY ${confs} DESTINATION "${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/config")