project(btl VERSION ${prplmesh_VERSION})
message("${BoldWhite}Preparing ${BoldGreen}${PROJECT_NAME}${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

if (NOT MSGLIB STREQUAL "None")
message(STATUS "${BoldGreen}Using MultiAP Framework for BTL transport${ColourReset}")
set(BTL_TYPE "LOCAL_BUS" PARENT_SCOPE)
set(btl_sources btl.cpp btl_local_bus.cpp)
else()
message(STATUS "${BoldYellow}Failed to find MultiAP Framework transport, fall back to BTL over UDS - supporting single node only!${ColourReset}")
set(BTL_TYPE "UDS" PARENT_SCOPE)
add_definitions(-DUDS_BUS)
set(btl_sources btl.cpp btl_uds.cpp)
endif()

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Build the library
add_library(${PROJECT_NAME} ${btl_sources})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${prplmesh_VERSION} SOVERSION ${prplmesh_VERSION_MAJOR})
target_link_libraries(${PROJECT_NAME} PUBLIC bcl tlvf elpp btlvf common)
if (NOT MSGLIB STREQUAL "None")
     target_link_libraries(${PROJECT_NAME} PRIVATE ieee1905_transport_messages)
endif()

# Include paths
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

# Install
install(
    TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
