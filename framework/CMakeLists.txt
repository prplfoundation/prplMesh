project(multiap_framework C CXX)

set(MSGLIB "zmq" CACHE STRING "Which messaging library backend to use")
set_property(CACHE MSGLIB PROPERTY STRINGS "zmq" "nng" "None")

find_package(json-c REQUIRED)

add_subdirectory(platform)
add_subdirectory(external)
add_subdirectory(tlvf)
add_subdirectory(common)
if (NOT MSGLIB STREQUAL "None")
add_subdirectory(transport)
endif()

add_dependencies(mapfcommon elpp)
add_dependencies(tlvf elpp)
