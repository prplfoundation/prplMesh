add_executable(mapfcli cli.cpp)
find_package(elpp REQUIRED)
target_link_libraries(mapfcli common mapf::elpp)
install(TARGETS mapfcli DESTINATION bin)
