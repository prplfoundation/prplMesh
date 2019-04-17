
add_executable(local_bus local_bus.cpp broker.cpp broker_config.cpp broker_interface.cpp)
target_include_directories(local_bus PUBLIC include)
target_link_libraries(local_bus PUBLIC common PRIVATE mapf::elpp ${JSONC_LIBRARY})
install(TARGETS local_bus DESTINATION bin)

find_package(elpp REQUIRED)
add_library(localbus SHARED broker.cpp broker_config.cpp broker_interface.cpp)
set_target_properties(localbus PROPERTIES PREFIX "mapf" VERSION ${${PROJECT}_VERSION_STRING} SOVERSION ${${PROJECT}_VERSION_MAJOR})
target_link_libraries(localbus PUBLIC common PRIVATE mapf::elpp ${JSONC_LIBRARY}) # TODO: add message libraries

target_include_directories(localbus PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
	)

install(TARGETS localbus EXPORT MapfLocalBusConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT MapfLocalBusConfig NAMESPACE mapf:: DESTINATION lib/cmake/MapfLocalBus)

configure_file(liblocalbus.pc.in liblocalbus.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/liblocalbus.pc DESTINATION lib/pkgconfig)

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/local_bus.conf.in"
	"${CMAKE_CURRENT_BINARY_DIR}/local_bus.conf"
	)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/local_bus.conf" DESTINATION share)
file(COPY "${CMAKE_CURRENT_BINARY_DIR}/local_bus.conf" DESTINATION "${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/share")
add_subdirectory(test)
