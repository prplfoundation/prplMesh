add_executable(controller multiap_controller.cpp)
target_link_libraries(controller localbus ieee1905_transport_messages mapf::elpp)
install(TARGETS controller DESTINATION bin/examples/transport)

add_executable(agent multiap_agent.cpp)
target_link_libraries(agent localbus ieee1905_transport_messages mapf::elpp)
install(TARGETS agent DESTINATION bin/examples/transport)
