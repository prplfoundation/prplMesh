set(PROJECT FRAMEWORK)
project (multiap_framework C CXX)

enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic -pthread -Wall -Wextra -Werror -Wno-comment -Wno-unused-parameter -Wno-missing-field-initializers -Wno-address-of-packed-member -fpermissive" CACHE STRING "" FORCE)
if(CMAKE_VERSION VERSION_LESS "3.1")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
	set(CMAKE_CXX_STANDARD 11)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z noexecstack -z relro -z now -pie")
endif()
message("-- Build Type=${CMAKE_BUILD_TYPE}")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -Wl,-S -fPIE -fPIC")
if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-protector-strong")
else()
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-protector")
endif()
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_C_FLAGS_RELEASE}")

if (TARGET_PLATFORM STREQUAL "rdkb")
    set(CMAKE_SKIP_RPATH TRUE)
endif()

# Update output directories for easy deployment
set(CMAKE_MULTIAP_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/bin)

message(STATUS "CMAKE_INSTALL_PREFIX - ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_PREFIX_PATH - ${CMAKE_PREFIX_PATH}")
message(STATUS "CMAKE_MODULE_PATH - ${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_PROGRAM_PATH - ${CMAKE_PROGRAM_PATH}")
message(STATUS "CMAKE_MULTIAP_OUTPUT_DIRECTORY - ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}")

include(multiap-helpers)

option(BUILD_EXAMPLES "build examples" OFF)
option(BUILD_TESTS "build multiap unit tests" OFF)
option(PASSIVE_MODE "build framework passive mode" OFF)

set(MSGLIB "zmq" CACHE STRING "Which messaging library backend to use")
set_property(CACHE MSGLIB PROPERTY STRINGS "zmq" "nng" "None")
if(PASSIVE_MODE)
message(STATUS "Force MSGLIB=None in passive mode")
set(MSGLIB "None")
endif()

find_package(json-c REQUIRED)

add_subdirectory(platform)
add_subdirectory(external)
add_subdirectory(tlvf)
add_subdirectory(common)
if (NOT MSGLIB STREQUAL "None")
add_subdirectory(cli)
add_subdirectory(transport)
#add_subdirectory(discovery_agent)
add_subdirectory(examples)
endif()

add_dependencies(common elpp)
add_dependencies(tlvf elpp)
