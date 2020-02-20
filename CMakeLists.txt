cmake_minimum_required(VERSION 3.1)
cmake_policy(VERSION 3.1)
project(prplmesh
        LANGUAGES C CXX
        VERSION 1.4.0)

include(cmake/multiap-helpers.cmake)

if(NOT DEFINED TARGET_PLATFORM)
set(TARGET_PLATFORM "linux")
endif()

option (BUILD_AGENT "Build EasyMesh agent" ON)
option (BUILD_CONTROLLER "Build EasyMesh controller" ON)

option(BUILD_SHARED_LIBS "Build shared libraries (.so) instead of static ones (.a)" ON)
option(BUILD_FULL_STATIC "Build full static executable (links library dependencies as static)" OFF)

if (BUILD_FULL_STATIC)
     if (BUILD_SHARED_LIBS)
        message("Turning off BUILD_SHARED_LIBS!")
        set (BUILD_SHARED_LIBS "OFF")
     endif()
endif()

## Generic checks and defaults

include(FindPkgConfig)
include(GNUInstallDirs)

# Add global project cmake modules path
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE)
endif()

# Update output directories for easy deployment
set(CMAKE_MULTIAP_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/bin)

message(STATUS "CMAKE_FIND_ROOT_PATH - ${CMAKE_FIND_ROOT_PATH}")
message(STATUS "CMAKE_INSTALL_PREFIX - ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_MODULE_PATH - ${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_PREFIX_PATH - ${CMAKE_PREFIX_PATH}")
message(STATUS "CMAKE_MULTIAP_OUTPUT_DIRECTORY - ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}")

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Do not fall back to C++98

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

# Default Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -pthread -Wno-comment -Wno-unused-parameter -Wno-missing-field-initializers")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s -O2 -Wformat -Wformat-security -Wl,-S -fPIC -fPIE")

# Compiler specific flags
if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-address-of-packed-member")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-protector-strong")
else()
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-protector")
endif()

if(NOT BUILD_SHARED_LIBS)
    #strip for Release
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
    #libdl needed by libcrypto
    set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} -ldl")
    if (BUILD_FULL_STATIC)
        #Force to use all static libraries. Can this be done in a different way?
        set(CMAKE_LINK_SEARCH_START_STATIC 1)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    endif()
else()
    add_definitions(-DBUILD_SHARED_LIBS)
endif()

# Release flags
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_C_FLAGS_RELEASE}")

# Linker Options
# - Setting the "rpath-link" linker variable to help the linker resolve secondery dependecies
#   (Libraries that are not directly referenced by the executable, but a linked library)
# - Setting "rpath" linker to add the cmake installed library folder to the linker search path
#   (Removes the need to set LD_LIBRARY_PATH manually when installing to a none standard location)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath=${CMAKE_INSTALL_FULL_LIBDIR},-rpath-link=${PLATFORM_STAGING_DIR}/usr/lib")
# Workaround for linking error with pthreads when building statically
if(BUILD_FULL_STATIC)
    set(CMAKE_EXE_LINKER_FLAGS " -Wl,-u,pthread_mutex_lock -Wl,-u,pthread_mutex_unlock  -static  ${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath=${CMAKE_INSTALL_FULL_LIBDIR},-rpath-link=${PLATFORM_STAGING_DIR}/usr/lib")
    set(CMAKE_SKIP_RPATH  TRUE)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z noexecstack -z relro -z now -pie")
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
set(BEEROCKS_REPEATER_MODE 0)
set(BEEROCKS_ENABLE_ARP_MONITOR 1)

# Logging configuration
set(BEEROCKS_LOG_FILES_ENABLED      "true")
set(BEEROCKS_LOG_FILES_PATH         "${TMP_PATH}/logs")
set(BEEROCKS_LOG_FILES_SUFFIX       ".log")
set(BEEROCKS_LOG_FILES_AUTO_ROLL    "true")
set(BEEROCKS_LOG_STDOUT_ENABLED     "false")
set(BEEROCKS_LOG_SYSLOG_ENABLED     "false")

# Platform specific flags
if (TARGET_PLATFORM STREQUAL "openwrt")
    if (TARGET_PLATFORM_TYPE STREQUAL "ugw")
        add_definitions(-DBEEROCKS_UGW)
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
    set(BEEROCKS_LOG_FILES_AUTO_ROLL "false")
    set(BEEROCKS_LOG_FILES_PATH "/rdklogs/logs")
    set(BEEROCKS_LOG_FILES_SUFFIX ".txt.0")
    set(CMAKE_SKIP_RPATH TRUE)
elseif (TARGET_PLATFORM STREQUAL "linux")
    add_definitions(-DBEEROCKS_LINUX)
    set(BEEROCKS_BH_WIRE_IFACE "eth0")
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

## Version
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/version.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version"
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/version DESTINATION config)
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/version DESTINATION "${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/config")
