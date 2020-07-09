if(BUILD_TESTS)
    # Broker Tests
    add_executable(ieee1905_transport_broker_tests
        ieee1905_transport_broker_tests.cpp
    )

    target_link_libraries(ieee1905_transport_broker_tests ieee1905_transport_lib gtest_main)

    install(TARGETS ieee1905_transport_broker_tests DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}/tests)
    add_test(NAME ieee1905_transport_broker_tests COMMAND $<TARGET_FILE:ieee1905_transport_broker_tests>)
endif()
