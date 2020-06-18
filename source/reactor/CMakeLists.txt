project(reactor VERSION ${prplmesh_VERSION})

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Sources
file(GLOB_RECURSE reactor_sources ${MODULE_PATH}/src/*.c*)

# Support more permissive c++ language
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

# Build the library
add_library(${PROJECT_NAME} ${reactor_sources})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-z,defs")
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${prplmesh_VERSION} SOVERSION ${prplmesh_VERSION_MAJOR})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${MODULE_PATH}/inc>
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
        ${MODULE_PATH}/unit_tests/EventHandlersTest.cpp
        ${MODULE_PATH}/unit_tests/EventLoopImplTest.cpp
        ${MODULE_PATH}/unit_tests/Main.cpp
        ${MODULE_PATH}/unit_tests/TimeToolsTest.cpp
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
            $<BUILD_INTERFACE:${MODULE_PATH}/inc>
            ${MODULE_PATH}/src
    )
    target_link_libraries(${TEST_PROJECT_NAME} reactor)
    target_link_libraries(${TEST_PROJECT_NAME} gtest gmock)
    install(TARGETS ${TEST_PROJECT_NAME} DESTINATION bin/tests)
    add_test(NAME ${TEST_PROJECT_NAME} COMMAND $<TARGET_FILE:${TEST_PROJECT_NAME}>)
endif()
