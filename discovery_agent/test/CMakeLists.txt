if(BUILD_TESTS)
		add_executable(discovery_agent_test discovery_agent_test.cpp)
		add_executable(discovery_agent_test1 multi_ap_agent1.cpp)
		add_executable(discovery_agent_test2 multi_ap_agent2.cpp)
		target_link_libraries(discovery_agent_test localbus ieee1905_transport_messages mapf::elpp)
		target_link_libraries(discovery_agent_test1 localbus ieee1905_transport_messages mapf::elpp)
		target_link_libraries(discovery_agent_test2 localbus ieee1905_transport_messages mapf::elpp)
		target_include_directories(discovery_agent_test1 PUBLIC
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
			$<INSTALL_INTERFACE:include>
			)
		target_include_directories(discovery_agent_test2 PUBLIC
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
			$<INSTALL_INTERFACE:include>
			)
		install(TARGETS discovery_agent_test DESTINATION bin/tests/discovery_agent_test)
		install(TARGETS discovery_agent_test1 DESTINATION bin/tests/discovery_agent_test1)
		install(TARGETS discovery_agent_test2 DESTINATION bin/tests/discovery_agent_test2)
		add_test(test_discovery_agent discovery_agent_test)
endif()
