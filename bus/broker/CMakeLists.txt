add_library(broker broker.cpp broker_config.cpp broker_interface.cpp)

find_package(elpp REQUIRED)
set_target_properties(broker PROPERTIES PREFIX "mapf")
target_link_libraries(broker PUBLIC common PRIVATE mapf::elpp ${JSONC_LIBRARY})

target_include_directories(broker PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
	)

install(TARGETS broker EXPORT MapfBrokerConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT MapfBrokerConfig NAMESPACE mapf:: DESTINATION lib/cmake/MapfBroker)
