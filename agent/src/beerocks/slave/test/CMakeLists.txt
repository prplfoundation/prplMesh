
if(BUILD_TESTS)
    set(TEST_CASE 1905_beacon_query_to_vs_test)

    add_executable(
        ${TEST_CASE}
        1905_beacon_query_to_vs_test.cpp
        ../1905_beacon_query_to_vs.cpp # this should be in a lib
    )

    target_include_directories(${TEST_CASE} PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../build/install/include/>
        )

    target_link_libraries(${TEST_CASE} btlvf tlvf common)
    install(TARGETS ${TEST_CASE} DESTINATION bin/tests)
    add_test(NAME ${TEST_CASE} COMMAND $<TARGET_FILE:${TEST_CASE}>)
endif()
