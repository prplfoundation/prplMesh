project (prplMesh LANGUAGES CXX C)
cmake_minimum_required (VERSION 3.3)

# Some CMakeLists.txt files still use the PROJECT variable TODO get rid of it.
set (PROJECT ${PROJECT_NAME})
include(FindPkgConfig)

# TODO turn this into an option
if(NOT DEFINED TARGET_PLATFORM)
set(TARGET_PLATFORM "linux")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.8.0")
        message(FATAL_ERROR "gcc version (${CMAKE_CXX_COMPILER_VERSION} not supported, minumum is 4.8.0")
    endif()
endif()

include(GNUInstallDirs)
enable_testing()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE)
endif()
message("-- prefix=${CMAKE_INSTALL_PREFIX}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic -pthread -Wall -Wextra -Werror -Wno-comment -Wno-unused-parameter -Wno-missing-field-initializers -fpermissive" CACHE STRING "" FORCE)
if(CMAKE_VERSION VERSION_LESS "3.1")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
	set(CMAKE_CXX_STANDARD 11)
endif()

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "Release")
endif()

# TODO use CMAKE_EXE_LINKER_FLAGS_RELEASE resp. COMPILE_DEFINITIONS_DEBUG
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z noexecstack -z relro -z now -pie")
else()
    add_definitions(-DDEBUG)
endif()
message("-- Build Type=${CMAKE_BUILD_TYPE}")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -Wl,-S -fPIE -fPIC")
if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-protector-strong")
else()
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-protector")
endif()
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_C_FLAGS_RELEASE}")

# Add "strip" flags TODO use CMAKE_EXE_LINKER_FLAGS_RELEASE
if (CMAKE_BUILD_TYPE STREQUAL "Release")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s")
endif()

# TODO is this needed?
if (TARGET_PLATFORM STREQUAL "rdkb")
    set(CMAKE_SKIP_RPATH TRUE)
endif()

# Linker Options
# - Setting the "rpath-link" linker variable to help the linker resolve secondery dependecies
#   (Libraries that are not directly referenced by the executable, but a linked library)
# - Setting "rpath" linker to add the cmake installed library folder to the linker search path
#   (Removes the need to set LD_LIBRARY_PATH manually when installing to a none standard location)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath=${CMAKE_INSTALL_FULL_LIBDIR},-rpath-link=${PLATFORM_STAGING_DIR}/usr/lib")

# Search local cmake folder first
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

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

# Platform specific flags
if (TARGET_PLATFORM STREQUAL "ugw")
    add_definitions(-DBEEROCKS_UGW)
elseif(TARGET_PLATFORM STREQUAL "rdkb")
    add_definitions(-DBEEROCKS_RDKB)
elseif (TARGET_PLATFORM STREQUAL "linux")
    add_definitions(-DBEEROCKS_LINUX)
endif()


add_definitions(-DBEEROCKS_VERSION="${${PROJECT}_VERSION_STRING}")
add_definitions(-DBEEROCKS_BUILD_DATE="${${PROJECT}_BUILD_DATE}")
add_definitions(-DBEEROCKS_REVISION="${${PROJECT}_REVISION}")
if(TARGET_PLATFORM STREQUAL "linux")
add_definitions(-DBEEROCKS_BIN_PATH="${CMAKE_INSTALL_PREFIX}/bin/")
add_definitions(-DBEEROCKS_CONF_PATH="${CMAKE_INSTALL_PREFIX}/config/")
else()
add_definitions(-DBEEROCKS_BIN_PATH="/opt/beerocks/")
add_definitions(-DBEEROCKS_CONF_PATH="/opt/beerocks/")
endif()

add_subdirectory(framework)
add_subdirectory(common)
add_subdirectory(agent)
add_subdirectory(controller)
