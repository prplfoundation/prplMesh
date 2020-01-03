cmake_minimum_required(VERSION 3.0)
cmake_policy(VERSION 3.0)
project(prplmesh
        LANGUAGES C CXX
        VERSION 1.4.0)

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

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  # When compiling with openWrt, FORTIFY_SOURCE=1 always gets added.
  # Since we anyway use FORTIFY_SOURCE=2 and it cannot be set two times
  # with a different value, we remove the one set externally, and add our own
  string(REGEX REPLACE "-D_FORTIFY_SOURCE=[0-9]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  string(REGEX REPLACE "-D_FORTIFY_SOURCE=[0-9]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  string(REGEX REPLACE "-D_FORTIFY_SOURCE=[0-9]" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
  string(REGEX REPLACE "-D_FORTIFY_SOURCE=[0-9]" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
  add_definitions("-D_FORTIFY_SOURCE=2")
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
if (NOT DEFINED BEEROCKS_BH_WIRE_IFACE)
    set(BEEROCKS_BH_WIRE_IFACE "eth1")
endif()
set(BEEROCKS_NOTIFY 0)
set(BEEROCKS_REPEATER_MODE 0)
set(BEEROCKS_ENABLE_ARP_MONITOR 1)

# Logging configuration
set(BEEROCKS_LOG_FILES_ENABLED      "true")
set(BEEROCKS_LOG_FILES_PATH         "${TMP_PATH}/logs")
set(BEEROCKS_LOG_FILES_AUTO_ROLL    "true")
set(BEEROCKS_LOG_STDOUT_ENABLED     "false")
set(BEEROCKS_LOG_SYSLOG_ENABLED     "false")

# Platform specific flags
if (TARGET_PLATFORM STREQUAL "openwrt")
    if (TARGET_PLATFORM_TYPE STREQUAL "ugw")
        add_definitions(-DBEEROCKS_UGW)
        set(BEEROCKS_NOTIFY 1)
        set(BEEROCKS_REPEATER_MODE 1)
    else()
        set(BEEROCKS_WLAN2_IFACE "wlan1")
    endif()
elseif (TARGET_PLATFORM STREQUAL "rdkb")
    add_definitions(-DBEEROCKS_RDKB)
    set(BEEROCKS_BRIDGE_IFACE "brlan0")
    set(BEEROCKS_BH_WIRE_IFACE "nsgmii0")
    set(BEEROCKS_ENABLE_ARP_MONITOR 0)
    # Disable file and enable syslog logging
    set(BEEROCKS_LOG_FILES_ENABLED "false")
    set(BEEROCKS_LOG_SYSLOG_ENABLED "true")
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
