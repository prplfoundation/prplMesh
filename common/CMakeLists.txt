set(PROJECT COMMON)
message("${BoldWhite}Preparing ${BoldGreen}MULTIAP_${PROJECT}${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

include(FindPkgConfig)
cmake_minimum_required (VERSION 2.8.12)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.8.0")
      message(FATAL_ERROR "gcc version (${CMAKE_CXX_COMPILER_VERSION} not supported, minumum is 4.8.0")
    endif()
endif()

include(GNUInstallDirs)

# Default values
set(BEEROCKS_BRIDGE_IFACE "br-lan")
set(BEEROCKS_PLATFORM_INIT "false")
set(BEEROCKS_INSTALL_PATH "/opt/prplmesh")
set(BEEROCKS_TMP_PATH "/tmp/beerocks")
set(BEEROCKS_WLAN1_IFACE "wlan0")
set(BEEROCKS_WLAN2_IFACE "wlan2")

# Platform specific overrides
if (TARGET_PLATFORM STREQUAL "ugw")
    add_definitions(-DBEEROCKS_UGW)
    set(BEEROCKS_BH_WIRE_IFACE "eth1")
elseif (TARGET_PLATFORM STREQUAL "rdkb")
    add_definitions(-DBEEROCKS_RDKB)
    set(BEEROCKS_BRIDGE_IFACE "brlan0")
    set(BEEROCKS_BH_WIRE_IFACE "nsgmii0")
    set(CMAKE_SKIP_RPATH TRUE)
elseif (TARGET_PLATFORM STREQUAL "linux")
    add_definitions(-DBEEROCKS_LINUX)
    set(BEEROCKS_BH_WIRE_IFACE "sim-eth0")
    set(BEEROCKS_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}")
    set(BEEROCKS_TMP_PATH "/tmp/$ENV{USER}/beerocks")
    set(BEEROCKS_PLATFORM_INIT "true")
elseif (TARGET_PLATFORM STREQUAL "openwrt")
    set(BEEROCKS_BH_WIRE_IFACE "eth1")
endif()

list (FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" isSystemDir)
if ("${isSystemDir}" STREQUAL "-1")
    set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
endif ("${isSystemDir}" STREQUAL "-1")
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
 
# Update output directories for easy deployment
set(CMAKE_MULTIAP_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/bin)

message(STATUS "CMAKE_INSTALL_PREFIX - ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_PREFIX_PATH - ${CMAKE_PREFIX_PATH}")
message(STATUS "CMAKE_MODULE_PATH - ${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_PROGRAM_PATH - ${CMAKE_PROGRAM_PATH}")
message(STATUS "CMAKE_MULTIAP_OUTPUT_DIRECTORY - ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}")

# Default Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -pthread -Wno-comment -Wno-unused-parameter -Wno-missing-field-initializers -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS} -std=c++11 -fpermissive")

# Compiler specific flags
if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-address-of-packed-member")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-protector-strong")
else()
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-protector")
endif()

# Release flags
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s -O2 -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -Wl,-S -fPIE")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_C_FLAGS_RELEASE}")

message(STATUS "Prefix=${CMAKE_INSTALL_PREFIX}")

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "Release")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z noexecstack -z relro -z now -pie")
else()
    add_definitions(-DDEBUG)
endif()

message(STATUS "Build Type=${CMAKE_BUILD_TYPE}")

include(multiap-helpers)

# Modules
add_subdirectory(beerocks)
