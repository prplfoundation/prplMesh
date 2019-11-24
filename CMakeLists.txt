cmake_minimum_required(VERSION 3.0)
project (prplmesh C CXX)

if(NOT DEFINED TARGET_PLATFORM)
set(TARGET_PLATFORM "linux")
endif()

option (BUILD_AGENT "Build EasyMesh agent" ON)
option (BUILD_CONTROLLER "Build EasyMesh controller" ON)

# Add global project cmake modules path
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_subdirectory(framework)
add_subdirectory(common)
if (BUILD_AGENT)
add_subdirectory(agent)
endif()
if (BUILD_CONTROLLER)
add_subdirectory(controller)
endif()
