if(MSGLIB STREQUAL "ZMQ")
message(STATUS "${BoldGreen}Messaging library - ZeroMQ${ColourReset}")
find_package(ZMQ REQUIRED)
set(msglib ${ZMQ_LIBRARY})
set(sources poller.cpp socket.cpp message_factory.cpp logger.cpp)
else()
message(STATUS "${BoldYellow}Messaging library - None${ColourReset}")
set(sources logger.cpp)
endif()

find_package(elpp REQUIRED)

set(compile_flags)

	configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/logging.conf.in"
		"${CMAKE_CURRENT_BINARY_DIR}/logging.conf"
	)
		install(FILES "${CMAKE_CURRENT_BINARY_DIR}/logging.conf" DESTINATION share)
		file(COPY "${CMAKE_CURRENT_BINARY_DIR}/logging.conf" DESTINATION "${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/share/")

add_library(common SHARED ${sources})
set_target_properties(common PROPERTIES PREFIX "mapf" VERSION ${${PROJECT}_VERSION_STRING} SOVERSION ${${PROJECT}_VERSION_MAJOR})
target_link_libraries(common PUBLIC ${msglib} PRIVATE mapf::elpp ${JSONC_LIBRARY})
target_include_directories(common PUBLIC ${PLATFORM_INCLUDE_DIR})
target_compile_definitions(common PUBLIC ${compile_flags})

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/mapf_config.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/include/mapf/common/config.h"
	)

target_include_directories(common
		PUBLIC
				$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
				$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
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

if(MSGLIB STREQUAL "ZMQ")
add_executable(version version.cpp)
target_link_libraries(version common mapf::elpp)
install(TARGETS version DESTINATION bin)
endif()

add_subdirectory(test)
add_subdirectory(external/easylogging)
