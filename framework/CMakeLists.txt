project(multiap_framework C CXX)

enable_testing()

option(BUILD_EXAMPLES "build examples" OFF)
option(BUILD_TESTS "build multiap unit tests" OFF)

set(MSGLIB "zmq" CACHE STRING "Which messaging library backend to use")
set_property(CACHE MSGLIB PROPERTY STRINGS "zmq" "nng" "None")

find_package(json-c REQUIRED)

add_subdirectory(platform)
add_subdirectory(external)
add_subdirectory(tlvf)
add_subdirectory(common)
if (NOT MSGLIB STREQUAL "None")
add_subdirectory(transport)
#add_subdirectory(discovery_agent)
add_subdirectory(examples)
endif()


add_dependencies(common elpp)
add_dependencies(tlvf elpp)
