cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME btl)
project(${PROJECT_NAME})
message("${BoldWhite}Preparing ${BoldGreen}${PROJECT_NAME}${BoldWhite} for the ${BoldGreen}${BEEROCKS_TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Dependecies
if(MSGLIB)
message(STATUS "${BoldGreen}Using MultiAP Framework for BTL transport${ColourReset}")
set(btl_sources btl.cpp btl_local_bus.cpp)
else()
message(STATUS "${BoldYellow}Failed to find MultiAP Framework transport, fall back to BTL over UDS - supporting single node only!${ColourReset}")
add_definitions(-DUDS_BUS)
set(btl_sources btl.cpp btl_uds.cpp)
endif()

find_package(elpp REQUIRED)

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})


# Build the library
add_library(${PROJECT_NAME} SHARED ${btl_sources})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${${PROJECT}_VERSION_STRING} SOVERSION ${${PROJECT}_VERSION_MAJOR})
if (MSGLIB)
     target_link_libraries(${PROJECT_NAME} PUBLIC bcl tlvf btlvf
         PRIVATE mapf::common mapf::ieee1905_transport_messages elpp)
else() # UDS Bus
    target_link_libraries(${PROJECT_NAME} PUBLIC bcl tlvf elpp btlvf)
endif()

# Include paths
target_include_directories(${PROJECT_NAME}
    PUBLIC ${MODULE_PATH}
    PRIVATE
    ${MODULE_PATH}/../../../external
    $<TARGET_PROPERTY:bcl,INTERFACE_INCLUDE_DIRECTORIES>
)

# Install
install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
