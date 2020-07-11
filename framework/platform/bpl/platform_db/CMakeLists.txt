configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/prplmesh_platform_db.in"
        "${CMAKE_CURRENT_BINARY_DIR}/prplmesh_platform_db"
        )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/prplmesh_platform_db DESTINATION share)
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/prplmesh_platform_db DESTINATION "${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/share")
