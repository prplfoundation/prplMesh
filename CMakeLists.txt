cmake_minimum_required(VERSION 3.0)
project (prplmesh C CXX)

if(NOT DEFINED TARGET_PLATFORM)
set(TARGET_PLATFORM "linux")
endif()

add_subdirectory(framework)
add_subdirectory(common)
add_subdirectory(agent)
add_subdirectory(controller)
