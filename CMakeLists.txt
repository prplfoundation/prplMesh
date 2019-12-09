cmake_minimum_required(VERSION 3.0)
project (prplmesh C CXX)

include(cmake/multiap-helpers.cmake)

if(NOT DEFINED TARGET_PLATFORM)
set(TARGET_PLATFORM "linux")
endif()

option (BUILD_AGENT "Build EasyMesh agent" ON)
option (BUILD_CONTROLLER "Build EasyMesh controller" ON)

## Generic checks and defaults

include(FindPkgConfig)
include(GNUInstallDirs)

# Add global project cmake modules path
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.8.0")
      message(FATAL_ERROR "gcc version (${CMAKE_CXX_COMPILER_VERSION} not supported, minumum is 4.8.0")
    endif()
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE)
endif()
message("-- prefix=${CMAKE_INSTALL_PREFIX}")

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "Release")
endif()

list (FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" isSystemDir)
if ("${isSystemDir}" STREQUAL "-1")
    set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
endif ("${isSystemDir}" STREQUAL "-1")
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(INSTALL_PATH "/opt/prplmesh")
set(TMP_PATH "/tmp/beerocks")

if(TARGET_PLATFORM STREQUAL "linux")
    set(INSTALL_PATH "${CMAKE_INSTALL_PREFIX}")
    option(USE_USER_TMP_PATH "use /tmp/$USER instead of /tmp path" ON)
    if(USE_USER_TMP_PATH)
        set(USER "$ENV{USER}")
        set(TMP_PATH "/tmp/${USER}/beerocks")
    endif()
endif()

# Default values
set(BEEROCKS_WLAN1_IFACE "wlan0")
set(BEEROCKS_WLAN2_IFACE "wlan2")
set(BEEROCKS_WLAN3_IFACE "wlan4")
set(BEEROCKS_BRIDGE_IFACE "br-lan")
set(BEEROCKS_BH_WIRE_IFACE "eth1")
set(BEEROCKS_STOP_ON_FAILURE_ATTEMPTS 1)
set(BEEROCKS_NOTIFY 0)
set(BEEROCKS_REPEATER_MODE 0)
set(BEEROCKS_ENABLE_ARP_MONITOR 1)
set(BEEROCKS_LOG_PATH "${TMP_PATH}/logs")

# Platform specific flags
if (TARGET_PLATFORM STREQUAL "openwrt")
    if (TARGET_PLATFORM_TYPE STREQUAL "ugw")
        add_definitions(-DBEEROCKS_UGW)
        set(BEEROCKS_NOTIFY 1)
        set(BEEROCKS_REPEATER_MODE 1)
    elseif (TARGET_PLATFORM_TYPE STREQUAL "turris-omnia")
        set(BEEROCKS_WLAN2_IFACE "wlan1")
    endif()
elseif (TARGET_PLATFORM STREQUAL "rdkb")
    add_definitions(-DBEEROCKS_RDKB)
    set(BEEROCKS_BRIDGE_IFACE "brlan0")
    set(BEEROCKS_BH_WIRE_IFACE "nsgmii0")
    set(BEEROCKS_ENABLE_ARP_MONITOR 0)
    set(BEEROCKS_LOG_PATH "/rdklogs/logs/")
    set(CMAKE_SKIP_RPATH TRUE)
elseif (TARGET_PLATFORM STREQUAL "linux")
    add_definitions(-DBEEROCKS_LINUX)
    set(BEEROCKS_BH_WIRE_IFACE "sim-eth0")
endif()

## Components

add_subdirectory(framework)
add_subdirectory(common)
if (BUILD_AGENT)
add_subdirectory(agent)
endif()
if (BUILD_CONTROLLER)
add_subdirectory(controller)
endif()
