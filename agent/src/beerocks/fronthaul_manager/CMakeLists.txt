project(beerocks_fronthaul VERSION ${prplmesh_VERSION})
message("${BoldWhite}Preparing ${BoldGreen}${PROJECT_NAME}${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

file(GLOB fronthaul_manager_sources 
    ${MODULE_PATH}/monitor/*.c*
    ${MODULE_PATH}/ap_manager/*.c*
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

#RDKB
if (TARGET_PLATFORM STREQUAL "rdkb")
    file(GLOB RDKB_SOURCES ${MODULE_PATH}/monitor/rdkb/*.c*)
    list(APPEND fronthaul_manager_sources  ${RDKB_SOURCES})
endif()

add_executable(${PROJECT_NAME} ${fronthaul_manager_sources})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-z,defs")
target_link_libraries(${PROJECT_NAME} bcl btlvf rt dl tlvf elpp bwl ${LIBS})

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
