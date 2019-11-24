# Include common cmake definitions
include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

## General configuration
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)

# Workaround for https://www.cmake.org/Bug/view.php?id=14075
set(CMAKE_CROSS_COMPILING ON)

# Platform specific
set(TARGET_PLATFORM             "openwrt" CACHE STRING "Target Platform")
set(TARGET_PLATFORM_TYPE        "$ENV{PRPLMESH_PLATFORM_TYPE}" CACHE STRING "Target Platform Type")
set(PLATFORM_BASE_DIR           "$ENV{PRPLMESH_PLATFORM_BASE_DIR}" CACHE STRING "Platform Base Directory")
set(PLATFORM_BUILD_NAME         "$ENV{PRPLMESH_PLATFORM_BUILD_NAME}" CACHE STRING "Platform Build Name")
set(PLATFORM_TOOLCHAIN          "$ENV{PRPLMESH_PLATFORM_TOOLCHAIN}" CACHE STRING "Platform Toolchain")
set(PLATFORM_TOOLCHAIN_PREFIX   "$ENV{PRPLMESH_PLATFORM_TOOLCHAIN_PREFIX}" CACHE STRING "Platform Toolchain Prefix")
set(PLATFORM_BUILD_DIR          ${PLATFORM_BASE_DIR}/build_dir/${PLATFORM_BUILD_NAME} CACHE STRING "Platform Build Directory")
set(PLATFORM_STAGING_DIR        ${PLATFORM_BASE_DIR}/staging_dir/${PLATFORM_BUILD_NAME} CACHE STRING "Platform Staging Directory")
set(PLATFORM_TOOLCHAIN_DIR      ${PLATFORM_BASE_DIR}/staging_dir/${PLATFORM_TOOLCHAIN} CACHE STRING "Platform Toolchain Directory")
set(PLATFORM_INCLUDE_DIR        ${PLATFORM_STAGING_DIR}/usr/include CACHE STRING "Platform Include Directory")
set(ENV{PKG_CONFIG_PATH}        "${PLATFORM_STAGING_DIR}/usr/lib/pkgconfig")

# Compiler Definitions
add_definitions(
    -DBEEROCKS_OPENWRT
)

# Compiler Flags and Search Paths
set(CMAKE_C_FLAGS "--sysroot=${PLATFORM_STAGING_DIR}")
set(CMAKE_CXX_FLAGS "--sysroot=${PLATFORM_STAGING_DIR}")
set(CMAKE_LIBRARY_PATH /usr/lib /lib)

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
set(CMAKE_C_COMPILER ${PLATFORM_TOOLCHAIN_DIR}/bin/${PLATFORM_TOOLCHAIN_PREFIX}gcc CACHE STRING "C Compiler")
set(CMAKE_CXX_COMPILER ${PLATFORM_TOOLCHAIN_DIR}/bin/${PLATFORM_TOOLCHAIN_PREFIX}g++ CACHE STRING "C++ Compiler")