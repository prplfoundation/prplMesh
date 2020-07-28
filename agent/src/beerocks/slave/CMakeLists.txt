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
    ${MODULE_PATH}/helpers/*.c*
    ${MODULE_PATH}/tasks/*.c*
    ${MODULE_PATH}/gate/*.c*
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

# Tests
if (BUILD_TESTS)
    set(TEST_PROJECT_NAME ${PROJECT_NAME}_unit_tests)
    set(unit_tests_sources
        gate/unit_tests/gate_test.cpp
        ${MODULE_PATH}/gate/1905_beacon_query_to_vs.cpp 
        ${MODULE_PATH}/gate/vs_beacon_response_to_1905.cpp
    )
    add_executable(${TEST_PROJECT_NAME}
        ${unit_tests_sources}
    )
    if (COVERAGE)
        set_target_properties(${TEST_PROJECT_NAME} PROPERTIES COMPILE_FLAGS "--coverage -fPIC -O0")
        set_target_properties(${TEST_PROJECT_NAME} PROPERTIES LINK_FLAGS "--coverage")
    endif()
    target_include_directories(${TEST_PROJECT_NAME}
        PRIVATE
            ${PLATFORM_INCLUDE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/gate
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
    target_link_libraries(${TEST_PROJECT_NAME} btlvf tlvf mapfcommon)
    target_link_libraries(${TEST_PROJECT_NAME} gtest_main)
    install(TARGETS ${TEST_PROJECT_NAME} DESTINATION bin/tests)
    add_test(NAME ${TEST_PROJECT_NAME} COMMAND $<TARGET_FILE:${TEST_PROJECT_NAME}>)
endif()



