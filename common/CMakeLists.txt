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
set(BEEROCKS_PLATFORM_INIT "true")
set(BEEROCKS_INSTALL_PATH "/opt/beerocks")
set(BEEROCKS_TMP_PATH "/tmp/beerocks")

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
elseif (TARGET_PLATFORM STREQUAL "turris-omnia")
    set(BEEROCKS_BH_WIRE_IFACE "eth1")
    set(BEEROCKS_PLATFORM_INIT "false")
    set(BEEROCKS_WLAN1_IFACE "wlan0")
    set(BEEROCKS_WLAN2_IFACE "wlan1")
elseif (TARGET_PLATFORM STREQUAL "glinet-b1300")
    set(BEEROCKS_BH_WIRE_IFACE "eth0")
    set(BEEROCKS_PLATFORM_INIT "false")
    set(BEEROCKS_WLAN1_IFACE "wlan0")
    set(BEEROCKS_WLAN2_IFACE "wlan1")
    # Add library search path explicitly through options
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${PLATFORM_STAGING_DIR}/usr/lib")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${PLATFORM_STAGING_DIR}/usr/lib")
elseif (TARGET_PLATFORM STREQUAL "owrt")
    set(BEEROCKS_BRIDGE_IFACE "dummy_br-lan")
    set(BEEROCKS_BH_WIRE_IFACE "dummy_eth")
    set(BEEROCKS_WLAN1_IFACE "dummy_wlan0")
    set(BEEROCKS_WLAN2_IFACE "dummy_wlan1")
    set(BEEROCKS_PLATFORM_INIT "false")
    set(BEEROCKS_TMP_PATH "${TARGET_TMP_PATH}")
    # BEEROCKS_INSTALL_PATH is used in the prplmesh_utils.sh to build path to
    # various binaries, so set it basing on the bin path passed to us.
    get_filename_component(BEEROCKS_INSTALL_PATH ${PKG_PRPL_BIN_PATH} DIRECTORY)
endif()

list (FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" isSystemDir)
if ("${isSystemDir}" STREQUAL "-1")
    set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
endif ("${isSystemDir}" STREQUAL "-1")
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

option(STANDALONE "Standalone build" OFF)

# Set the CMAKE modules path
if (NOT STANDALONE)
    message(STATUS "${BoldBlue}Building 'In-Tree'${ColourReset}")
else() # Standalone build
    message(STATUS "${BoldBlue}Building 'Standalone'${ColourReset}")

    # Convert the install prefix into an absolute path
    get_filename_component(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" 
        REALPATH BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")

    # CMake search paths
    list(INSERT CMAKE_FIND_ROOT_PATH 0 ${CMAKE_INSTALL_PREFIX})
    list(APPEND CMAKE_PROGRAM_PATH ${CMAKE_INSTALL_PREFIX}/host/bin)
    list(APPEND CMAKE_PROGRAM_PATH ${PLATFORM_STAGING_DIR}/host/bin)
endif()

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
