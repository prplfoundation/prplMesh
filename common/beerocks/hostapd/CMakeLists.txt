
project(prplmesh_hostapd VERSION ${prplmesh_VERSION})

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Build the library
set(hostapd_sources source/configuration.cpp)
add_library(${PROJECT_NAME} ${hostapd_sources})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${prplmesh_VERSION} SOVERSION ${prplmesh_VERSION_MAJOR})
target_link_libraries(${PROJECT_NAME} elpp ${PRPLMESH_HOSTAPD_LIBS})
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PLATFORM_INCLUDE_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(
    TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (BUILD_TESTS)
    set(TEST_PROJECT_NAME ${PROJECT_NAME}_unit_tests)
    set(unit_tests_sources
        ${MODULE_PATH}/unit_tests/configuration_test.cpp
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
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
    target_link_libraries(${TEST_PROJECT_NAME} ${PROJECT_NAME} ${PRPLMESH_HOSTAPD_LIBS})
    target_link_libraries(${TEST_PROJECT_NAME} gtest_main)
    install(TARGETS ${TEST_PROJECT_NAME} DESTINATION bin/tests)
    add_test(NAME ${TEST_PROJECT_NAME} COMMAND $<TARGET_FILE:${TEST_PROJECT_NAME}>)
endif()
