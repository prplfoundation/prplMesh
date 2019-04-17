# Add RPATH for mapf libraries for all executables in this directory and below
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

find_package(elpp REQUIRED)

add_library(ieee1905_transport_lib SHARED ieee1905_transport.cpp ieee1905_transport_network.cpp ieee1905_transport_netlink.cpp ieee1905_transport_local_bus.cpp ieee1905_transport_packet_processing.cpp)
set_target_properties(ieee1905_transport_lib PROPERTIES VERSION ${${PROJECT}_VERSION_STRING} SOVERSION ${${PROJECT}_VERSION_MAJOR})
target_link_libraries(ieee1905_transport_lib PUBLIC ieee1905_transport_messages localbus PRIVATE mapf::elpp)
target_include_directories(ieee1905_transport_lib PUBLIC include/)
install(TARGETS ieee1905_transport_lib DESTINATION lib)

add_library(ieee1905_transport_messages SHARED ieee1905_transport_messages.cpp)
set_target_properties(ieee1905_transport_messages PROPERTIES VERSION ${${PROJECT}_VERSION_STRING} SOVERSION ${${PROJECT}_VERSION_MAJOR})
target_link_libraries(ieee1905_transport_messages PUBLIC common PRIVATE mapf::elpp)
target_include_directories(ieee1905_transport_messages PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )
install(TARGETS ieee1905_transport_messages
        EXPORT MapfTransportConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT MapfTransportConfig NAMESPACE mapf:: DESTINATION lib/cmake/MapfTransportConfig)

add_executable(ieee1905_transport main.cpp)
target_link_libraries(ieee1905_transport PUBLIC ieee1905_transport_lib PRIVATE mapf::elpp)
install(TARGETS ieee1905_transport DESTINATION bin)

add_subdirectory(test)
