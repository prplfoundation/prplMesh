find_package(elpp REQUIRED)

add_library(ieee1905_transport_lib SHARED ieee1905_transport.cpp ieee1905_transport_network.cpp ieee1905_transport_netlink.cpp ieee1905_transport_local_bus.cpp ieee1905_transport_packet_processing.cpp)
set_target_properties(ieee1905_transport_lib PROPERTIES VERSION ${prplmesh_VERSION} SOVERSION ${${PROJECT}_VERSION_MAJOR})
target_link_libraries(ieee1905_transport_lib PUBLIC ieee1905_transport_messages common PRIVATE mapf::elpp)
target_include_directories(ieee1905_transport_lib PUBLIC include/)

add_library(ieee1905_transport_messages SHARED ieee1905_transport_messages.cpp)
set_target_properties(ieee1905_transport_messages PROPERTIES VERSION ${prplmesh_VERSION} SOVERSION ${${PROJECT}_VERSION_MAJOR})
target_link_libraries(ieee1905_transport_messages PUBLIC common PRIVATE mapf::elpp)
target_include_directories(ieee1905_transport_messages PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

add_executable(ieee1905_transport main.cpp)
target_link_libraries(ieee1905_transport PUBLIC ieee1905_transport_lib PRIVATE mapf::elpp)

install(TARGETS ieee1905_transport_messages ieee1905_transport_lib ieee1905_transport
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

add_subdirectory(test)
