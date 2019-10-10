# Include common cmake definitions
include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

## General configuration
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR ARMV7 CACHE STRING "System Processor")
set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)

# Workaround for https://www.cmake.org/Bug/view.php?id=14075
set(CMAKE_CROSS_COMPILING ON)

# Check if openwrt dir path var is set
if(NOT DEFINED ENV{OPENWRT_DIR})
    message(FATAL_ERROR "You must set OPENWRT_DIR environment variable")
endif()

# Platform specific
set(TARGET_PLATFORM             "glinet-b1300" CACHE STRING "Target Platform")
set(PLATFORM_BASE_DIR           $ENV{OPENWRT_DIR} CACHE STRING "Platform Base Directory")
set(PLATFORM_BUILD_NAME         target-arm_cortex-a7+neon-vfpv4_musl_eabi CACHE STRING "Platform Build Name")
set(PLATFORM_TOOLCHAIN          toolchain-arm_cortex-a7+neon-vfpv4_gcc-7.4.0_musl_eabi CACHE STRING "Platform Toolchain")
set(PLATFORM_TOOLCHAIN_PREFIX   arm-openwrt-linux-muslgnueabi- CACHE STRING "Platform Toolchain Prefix")

set(PLATFORM_BUILD_DIR          ${PLATFORM_BASE_DIR}/build_dir/${PLATFORM_BUILD_NAME} CACHE STRING "Platform Build Directory")
set(PLATFORM_STAGING_DIR        ${PLATFORM_BASE_DIR}/staging_dir/${PLATFORM_BUILD_NAME} CACHE STRING "Platform Staging Directory")
set(PLATFORM_TOOLCHAIN_DIR      ${PLATFORM_BASE_DIR}/staging_dir/${PLATFORM_TOOLCHAIN} CACHE STRING "Platform Toolchain Directory")
set(PLATFORM_INCLUDE_DIR        ${PLATFORM_STAGING_DIR}/usr/include CACHE STRING "Platform Include Directory")
set(ENV{PKG_CONFIG_PATH}        "${PLATFORM_STAGING_DIR}/usr/lib/pkgconfig")

# Check if openwrt staging dir path environment var is set
if(NOT DEFINED ENV{STAGING_DIR})
    message(FATAL_ERROR "You must set STAGING_DIR environment variable to ${PLATFORM_STAGING_DIR}")
endif()

# Compiler Definitions
add_definitions(-DOPENWRT_BASED_PLATFORM)

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

# projects (mis)use `-isystem` to silence warnings from 3rd-party
# source (among other things). gcc6 introduces changes to search
# order which breaks this usage.
#   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70129
#   https://gitlab.kitware.com/cmake/cmake/issues/16291
#   https://gitlab.kitware.com/cmake/cmake/issues/16919
set(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES ${PLATFORM_TOOLCHAIN_DIR} CACHE STRING "")
set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES ${PLATFORM_TOOLCHAIN_DIR} CACHE STRING "")

## Toolchain Programs
set(CMAKE_C_COMPILER ${PLATFORM_TOOLCHAIN_DIR}/bin/${PLATFORM_TOOLCHAIN_PREFIX}gcc CACHE STRING "C Compiler")
set(CMAKE_CXX_COMPILER ${PLATFORM_TOOLCHAIN_DIR}/bin/${PLATFORM_TOOLCHAIN_PREFIX}g++ CACHE STRING "C++ Compiler")
