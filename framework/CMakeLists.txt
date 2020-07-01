project(multiap_framework C CXX)

find_package(json-c REQUIRED)

add_subdirectory(platform)
add_subdirectory(external)
add_subdirectory(tlvf)
add_subdirectory(common)
add_subdirectory(transport)

add_dependencies(mapfcommon elpp)
add_dependencies(tlvf elpp)
