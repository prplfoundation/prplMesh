# Include common cmake definitions
include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

## General configuration
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR ARMV7 CACHE STRING "System Processor")
set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)

# Workaround for https://www.cmake.org/Bug/view.php?id=14075
set(CMAKE_CROSS_COMPILING ON)

# Platform specific
set(TARGET_PLATFORM             "rdkb" CACHE STRING "Target Platform")
set(PLATFORM_BASE_DIR           /local/vitaly/rdkb/atom_rdkbos/build/tmp/sysroots CACHE STRING "Platform Base Directory")
set(PLATFORM_BUILD_NAME         puma7-atom CACHE STRING "Platform Build Name")
set(PLATFORM_TOOLCHAIN_PREFIX   i686-rdk-linux- CACHE STRING "Platform Toolchain Prefix")
set(PLATFORM_BUILD_DIR          ${PLATFORM_BASE_DIR} CACHE STRING "Platform Build Directory")
set(PLATFORM_STAGING_DIR        ${PLATFORM_BUILD_DIR}/${PLATFORM_BUILD_NAME} CACHE STRING "Platform Staging Directory")
set(PLATFORM_TOOLCHAIN_DIR      ${PLATFORM_BASE_DIR}/x86_64-linux/usr/bin/i686-rdk-linux CACHE STRING "Platform Toolchain Directory")
set(PLATFORM_INCLUDE_DIR        ${PLATFORM_STAGING_DIR}/usr/include CACHE STRING "Platform Include Directory")
set(ENV{PKG_CONFIG_PATH}        "${PLATFORM_STAGING_DIR}/usr/lib/pkgconfig")

# Compiler Definitions
add_definitions(
    -DBEEROCKS_RDKB
    -DYOCTO
)

## Library config
set(BUILD_SHARED_LIBS ON CACHE BOOL "BUILD_SHARED_LIBS" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "BUILD_STATIC_LIBS" FORCE)
set(BUILD_SHARED ON CACHE BOOL "BUILD_SHARED" FORCE)
set(BUILD_STATIC ON CACHE BOOL "BUILD_STATIC" FORCE)

## Paths etc.
set(CMAKE_FIND_ROOT_PATH ${PLATFORM_STAGING_DIR} CACHE STRING "")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER CACHE STRING "")
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY CACHE STRING "")
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY CACHE STRING "")
set(CMAKE_PREFIX_PATH ${PLATFORM_STAGING_DIR} CACHE STRING "")
# set(CMAKE_INSTALL_PREFIX /usr/src/mxe/usr/x86_64-w64-mingw32.static.posix CACHE PATH "Installation Prefix")

# projects (mis)use `-isystem` to silence warnings from 3rd-party
# source (among other things). gcc6 introduces changes to search
# order which breaks this usage.
#   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70129
#   https://gitlab.kitware.com/cmake/cmake/issues/16291
#   https://gitlab.kitware.com/cmake/cmake/issues/16919
set(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES ${PLATFORM_TOOLCHAIN_DIR} CACHE STRING "")
set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES ${PLATFORM_TOOLCHAIN_DIR} CACHE STRING "")

## Toolchain Programs
set(CMAKE_C_COMPILER ${PLATFORM_TOOLCHAIN_DIR}/${PLATFORM_TOOLCHAIN_PREFIX}gcc CACHE STRING "C Compiler")
set(CMAKE_CXX_COMPILER ${PLATFORM_TOOLCHAIN_DIR}/${PLATFORM_TOOLCHAIN_PREFIX}g++ CACHE STRING "C++ Compiler")

# Do not use files from system paths
set(CMAKE_NO_SYSTEM_FROM_IMPORTED 1 CACHE INTERNAL "")
set(NO_CMAKE_SYSTEM_PATH 1 CACHE INTERNAL "")

# Default Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=${PLATFORM_STAGING_DIR}" CACHE STRING "Default C Flags" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=${PLATFORM_STAGING_DIR}" CACHE STRING "Default CXX Flags" FORCE)
