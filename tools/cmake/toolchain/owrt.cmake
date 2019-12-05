# Include common cmake definitions
include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

## General configuration
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR $ENV{CMAKE_SYSTEM_PROCESSOR} CACHE STRING "System Processor")
set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)

# Workaround for https://www.cmake.org/Bug/view.php?id=14075
set(CMAKE_CROSS_COMPILING ON)

# Platform specific
set(TARGET_PLATFORM             "owrt" CACHE STRING "Target Platform")
set(PLATFORM_BASE_DIR           $ENV{TOPDIR} CACHE STRING "Platform Base Directory")
set(PLATFORM_TOOLCHAIN_PREFIX   $ENV{TARGET_CROSS} CACHE STRING "Platform Toolchain Prefix")

set(PLATFORM_BUILD_DIR          $ENV{BUILD_DIR} CACHE STRING "Platform Build Directory")
set(PLATFORM_STAGING_DIR        $ENV{STAGING_DIR} CACHE STRING "Platform Staging Directory")
set(PLATFORM_TOOLCHAIN_DIR      $ENV{TOOLCHAIN_DIR} CACHE STRING "Platform Toolchain Directory")
set(PLATFORM_INCLUDE_DIR        ${PLATFORM_STAGING_DIR}/usr/include CACHE STRING "Platform Include Directory")
set(ENV{PKG_CONFIG_PATH}        "${PLATFORM_STAGING_DIR}/usr/lib/pkgconfig")

set(PKG_PRPL_BIN_PATH           $ENV{PKG_PRPL_BIN_PATH} CACHE STRING "Package Install Bins")
set(PKG_PRPL_LIB_PATH           $ENV{PKG_PRPL_LIB_PATH} CACHE STRING "Package Install Libs")
set(PKG_PRPL_CFG_PATH           $ENV{PKG_PRPL_CFG_PATH} CACHE STRING "Package Install Config")
set(PKG_PRPL_SHARE_PATH         $ENV{PKG_PRPL_SHARE_PATH} CACHE STRING "Package Install Database")

set(TARGET_TMP_PATH             $ENV{TARGET_TMP_PATH}  CACHE STRING "Temporary Files Path on Target")
set(TARGET_BWL_TYPE             $ENV{TARGET_BWL_TYPE}  CACHE STRING "Target BWL Type")

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
