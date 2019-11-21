set(PROJECT CONTROLLER)
cmake_minimum_required(VERSION 2.8)
include(GNUInstallDirs)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()
message(STATUS "Beerocks Build Type: ${CMAKE_BUILD_TYPE}")

# Default values
set(BEEROCKS_TMP_PATH "/tmp/beerocks")
set(BEEROCKS_WLAN1_IFACE "wlan0")
set(BEEROCKS_WLAN2_IFACE "wlan2")

# Platform specific flags
if (TARGET_PLATFORM STREQUAL "openwrt")
    # UGW
    if (TARGET_PLATFORM_TYPE STREQUAL "ugw")
        add_definitions(-DBEEROCKS_UGW)
    # Turris Omnia
    elseif (TARGET_PLATFORM_TYPE STREQUAL "turris-omnia")
        set(BEEROCKS_WLAN2_IFACE "wlan1")
    endif()    
elseif(TARGET_PLATFORM STREQUAL "rdkb")
    add_definitions(-DBEEROCKS_RDKB)
    set(CMAKE_SKIP_RPATH TRUE)
elseif (TARGET_PLATFORM STREQUAL "linux")
    set(BEEROCKS_TMP_PATH "/tmp/$ENV{USER}/beerocks")
endif()

list (FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" isSystemDir)
if ("${isSystemDir}" STREQUAL "-1")
    set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
endif ("${isSystemDir}" STREQUAL "-1")
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Use beerocks intel ip patent
option (BUILD_PATENTS "build with patents enabled" ON)
if (BUILD_PATENTS)
    add_definitions(-DBEEROCKS_PATENTS)
endif()

option(STANDALONE "Standalone build" OFF)

# Search local cmake folder first
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)
# Add beerocks cmake modules to search paths
list(APPEND CMAKE_PREFIX_PATH ${PLATFORM_STAGING_DIR}/usr/lib/cmake/beerocks)
if (NOT STANDALONE)
    message(STATUS "${BoldBlue}Building 'In-Tree'${ColourReset}")
else()
    message(STATUS "${BoldBlue}Building 'Standalone'${ColourReset}")
    # Convert the install prefix into an absolute path
    get_filename_component(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}"
        REALPATH BASE_DIR "${CMAKE_SOURCE_DIR}")
    list(INSERT CMAKE_FIND_ROOT_PATH 0 ${CMAKE_INSTALL_PREFIX})
    list(APPEND CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX}/lib/cmake/beerocks)
endif()

# Update output directories for easy deployment
set(CMAKE_MULTIAP_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/bin)

message(STATUS "CMAKE_FIND_ROOT_PATH - ${CMAKE_FIND_ROOT_PATH}")
message(STATUS "CMAKE_INSTALL_PREFIX - ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_MODULE_PATH - ${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_PREFIX_PATH - ${CMAKE_PREFIX_PATH}")
message(STATUS "CMAKE_MULTIAP_OUTPUT_DIRECTORY - ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}")

# Add "strip" flags

# Default Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -pthread -Wno-comment  -Wno-unused-parameter -Wno-shift-negative-value -Wno-missing-field-initializers -fPIC")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s -O2 -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -Wl,-S -fPIE")

# Compiler specific flags
if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-address-of-packed-member")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-protector-strong")
else()
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-protector")
endif()

# Release flags
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS} -std=c++11 -fpermissive")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_C_FLAGS_RELEASE}")

# Linker Options
# - Setting the "rpath-link" linker variable to help the linker resolve secondery dependecies
#   (Libraries that are not directly referenced by the executable, but a linked library)
# - Setting "rpath" linker to add the cmake installed library folder to the linker search path
#   (Removes the need to set LD_LIBRARY_PATH manually when installing to a none standard location)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath=${CMAKE_INSTALL_FULL_LIBDIR},-rpath-link=${PLATFORM_STAGING_DIR}/usr/lib")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z noexecstack -z relro -z now -pie")
endif()

include(multiap-helpers)
add_definitions(-DBEEROCKS_VERSION="${${PROJECT}_VERSION_STRING}")
add_definitions(-DBEEROCKS_BUILD_DATE="${${PROJECT}_BUILD_DATE}")
add_definitions(-DBEEROCKS_REVISION="${${PROJECT}_REVISION}")
if(TARGET_PLATFORM STREQUAL "linux")
add_definitions(-DBEEROCKS_BIN_PATH="${CMAKE_INSTALL_PREFIX}/bin/")
add_definitions(-DBEEROCKS_CONF_PATH="${CMAKE_INSTALL_PREFIX}/config/")
elseif(TARGET_PLATFORM STREQUAL "openwrt")
add_definitions(-DBEEROCKS_BIN_PATH="/opt/prplMesh/bin/")
add_definitions(-DBEEROCKS_CONF_PATH="/opt/prplMesh/config/")
elseif(TARGET_PLATFORM STREQUAL "rdkb")
add_definitions(-DBEEROCKS_BIN_PATH="/opt/prplMesh/bin/")
add_definitions(-DBEEROCKS_CONF_PATH="/opt/prplMesh/config/")
else()
add_definitions(-DBEEROCKS_BIN_PATH="/opt/beerocks/")
add_definitions(-DBEEROCKS_CONF_PATH="/opt/beerocks/")
endif()

add_subdirectory("src/beerocks")
add_subdirectory("config")
