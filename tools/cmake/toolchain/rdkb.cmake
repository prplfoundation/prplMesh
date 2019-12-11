## General configuration
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)

# Workaround for https://www.cmake.org/Bug/view.php?id=14075
set(CMAKE_CROSS_COMPILING ON)

# Platform specific
set(TARGET_PLATFORM "rdkb" CACHE STRING "Target Platform")

# Base RDKB build dir (e.g. /path/to/atom_rdkbos)
if (NOT DEFINED ENV{PRPLMESH_PLATFORM_BASE_DIR})
    message(FATAL_ERROR "PLATFORM_BASE_DIR environment variable not defined!")
endif()
set(PLATFORM_BASE_DIR "$ENV{PRPLMESH_PLATFORM_BASE_DIR}" CACHE STRING "Platform Base Directory")

# RDKB build name (e.g. puma7-atom)
if (NOT DEFINED ENV{PRPLMESH_PLATFORM_BUILD_NAME})
    message(FATAL_ERROR "PRPLMESH_PLATFORM_BUILD_NAME environment variable not defined!")
endif()
set(PLATFORM_BUILD_NAME "$ENV{PRPLMESH_PLATFORM_BUILD_NAME}" CACHE STRING "Platform Build Name")

# Platform toolchain name (e.g. x86_64-linux)
if (NOT DEFINED ENV{PRPLMESH_PLATFORM_TOOLCHAIN})
    message(FATAL_ERROR "PRPLMESH_PLATFORM_TOOLCHAIN environment variable not defined!")
endif()
set(PLATFORM_TOOLCHAIN "$ENV{PRPLMESH_PLATFORM_TOOLCHAIN}" CACHE STRING "Platform Toolchain")

# Platform toolchain prefix (e.g. i686-rdk-linux)
if (NOT DEFINED ENV{PRPLMESH_PLATFORM_TOOLCHAIN_PREFIX})
    message(FATAL_ERROR "PLATFORM_TOOLCHAIN_PREFIX environment variable not defined!")
endif()
set(PLATFORM_TOOLCHAIN_PREFIX "$ENV{PRPLMESH_PLATFORM_TOOLCHAIN_PREFIX}" CACHE STRING "Platform Toolchain Prefix")

# Platform directories
set(PLATFORM_BUILD_DIR          ${PLATFORM_BASE_DIR}/build/tmp/sysroots CACHE STRING "Platform Build Directory")
set(PLATFORM_STAGING_DIR        ${PLATFORM_BUILD_DIR}/${PLATFORM_BUILD_NAME} CACHE STRING "Platform Staging Directory")
set(PLATFORM_TOOLCHAIN_DIR      ${PLATFORM_BUILD_DIR}/${PLATFORM_TOOLCHAIN}/usr/bin/${PLATFORM_TOOLCHAIN_PREFIX} CACHE STRING "Platform Toolchain Directory")
set(PLATFORM_INCLUDE_DIR        ${PLATFORM_STAGING_DIR}/usr/include CACHE STRING "Platform Include Directory")
set(ENV{PKG_CONFIG_PATH}        "${PLATFORM_STAGING_DIR}/usr/lib/pkgconfig")

# Compiler Definitions
add_definitions(
    -DBEEROCKS_RDKB
)

## Paths etc.
set(CMAKE_FIND_ROOT_PATH ${PLATFORM_STAGING_DIR} CACHE STRING "")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER CACHE STRING "")
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY CACHE STRING "")
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY CACHE STRING "")
set(CMAKE_PREFIX_PATH ${PLATFORM_STAGING_DIR} CACHE STRING "")

# projects (mis)use `-isystem` to silence warnings from 3rd-party
# source (among other things). gcc6 introduces changes to search
# order which breaks this usage.
#   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70129
#   https://gitlab.kitware.com/cmake/cmake/issues/16291
#   https://gitlab.kitware.com/cmake/cmake/issues/16919
set(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES ${PLATFORM_TOOLCHAIN_DIR} CACHE STRING "")
set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES ${PLATFORM_TOOLCHAIN_DIR} CACHE STRING "")

## Toolchain Programs
set(CMAKE_C_COMPILER ${PLATFORM_TOOLCHAIN_DIR}/${PLATFORM_TOOLCHAIN_PREFIX}-gcc CACHE STRING "C Compiler")
set(CMAKE_CXX_COMPILER ${PLATFORM_TOOLCHAIN_DIR}/${PLATFORM_TOOLCHAIN_PREFIX}-g++ CACHE STRING "C++ Compiler")

# Do not use files from system paths
set(CMAKE_NO_SYSTEM_FROM_IMPORTED 1 CACHE INTERNAL "")
set(NO_CMAKE_SYSTEM_PATH 1 CACHE INTERNAL "")

# Default Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=${PLATFORM_STAGING_DIR}" CACHE STRING "Default C Flags" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=${PLATFORM_STAGING_DIR}" CACHE STRING "Default CXX Flags" FORCE)
