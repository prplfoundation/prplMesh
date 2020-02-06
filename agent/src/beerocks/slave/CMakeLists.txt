project(beerocks_agent VERSION ${prplmesh_VERSION})
message("${BoldWhite}Preparing ${BoldGreen}${PROJECT_NAME}${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

file(GLOB beerocks_agent_sources 
    ${MODULE_PATH}/backhaul_manager/*.c*
    # This code should be moved to BPL 
    # [TASK] Move link metric related classes to BPL #910
    ${MODULE_PATH}/link_metrics/*.c*
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
target_link_libraries(${PROJECT_NAME} rt dl bcl btl tlvf elpp btlvf bwl bpl ${LIBS})

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
