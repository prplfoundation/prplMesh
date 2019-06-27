# Logging
find_package(elpp REQUIRED)
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/logging.conf.in"
	"${CMAKE_CURRENT_BINARY_DIR}/logging.conf"
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/logging.conf" DESTINATION share)
file(COPY "${CMAKE_CURRENT_BINARY_DIR}/logging.conf" DESTINATION "${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/share/")

# Messaging
if(MSGLIB STREQUAL "None")

message(STATUS "${BoldYellow}Messaging library - None${ColourReset}")
set(sources logger.cpp)

else()

message(STATUS "${BoldGreen}Messaging library - ${MSGLIB}${ColourReset}")
find_package(${MSGLIB} REQUIRED)
set(MSGLIB_TARGET ${MSGLIB}::${MSGLIB})
set(sources logger.cpp message_factory.cpp broker_config.cpp broker_interface.cpp ${MSGLIB}/broker.cpp ${MSGLIB}/socket.cpp ${MSGLIB}/context.cpp ${MSGLIB}/poller.cpp )

add_executable(version ${MSGLIB}/version.cpp)
target_link_libraries(version common mapf::elpp ${MSGLIB_TARGET})

add_executable(local_bus local_bus.cpp)
target_link_libraries(local_bus PRIVATE common mapf::elpp json-c)

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/local_bus.conf.in"
	"${CMAKE_CURRENT_BINARY_DIR}/local_bus.conf"
	)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/local_bus.conf" DESTINATION share)
file(COPY "${CMAKE_CURRENT_BINARY_DIR}/local_bus.conf" DESTINATION "${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/share")
install(TARGETS version DESTINATION bin)
install(TARGETS local_bus DESTINATION bin)

endif()

# Create a single common library
add_library(common SHARED ${sources})
set_target_properties(common PROPERTIES PREFIX "mapf" VERSION ${${PROJECT}_VERSION_STRING} SOVERSION ${${PROJECT}_VERSION_MAJOR})
target_link_libraries(common PRIVATE ${MSGLIB_TARGET} mapf::elpp json-c)
target_include_directories(common PUBLIC ${PLATFORM_INCLUDE_DIR})

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/mapf_config.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/include/mapf/common/config.h"
)

target_include_directories(common
		PUBLIC
				$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
				$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
				$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${MSGLIB}/include>
		PRIVATE
				$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/>
				$<INSTALL_INTERFACE:include>
		)

install(TARGETS common EXPORT MapfCommonConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT MapfCommonConfig NAMESPACE mapf:: DESTINATION lib/cmake/MapfCommon)

add_subdirectory(test)
