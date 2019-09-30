cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME beerocks_agent)
project (${PROJECT_NAME})
message("${BoldWhite}Preparing ${BoldGreen}${PROJECT_NAME}${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Use BPL package from mAP Framework
find_package(btl REQUIRED)
find_package(MapfTransport QUIET)

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

file(GLOB beerocks_agent_sources 
    ${MODULE_PATH}/backhaul_manager/*.c*
    ${MODULE_PATH}/platform_manager/*.c*
    ${MODULE_PATH}/*.c*
)

# UGW
if (TARGET_PLATFORM STREQUAL "ugw")

    # UGW ugwhelper library
    add_library(ugwhelper SHARED IMPORTED)
    set_target_properties(ugwhelper PROPERTIES
        IMPORTED_LOCATION "${PLATFORM_STAGING_DIR}/usr/lib/libugwhelper.so"
    )

    # Platform specific libraries
    list(APPEND LIBS ugwhelper)

endif()

add_executable(${PROJECT_NAME} ${beerocks_agent_sources})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-z,defs")
target_link_libraries(${PROJECT_NAME} rt dl beerocks::bcl beerocks::btl tlvf elpp beerocks::btlvf ${LIBS})

# For some reason, this isn't found automatically from btl...
if (MapfTransport_FOUND)
     target_link_libraries(${PROJECT_NAME} PRIVATE mapf::ieee1905_transport_messages)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${MODULE_PATH}/../../../external
)

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
