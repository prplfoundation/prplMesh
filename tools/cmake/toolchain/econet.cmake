set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)


# Skip this file for internal CMAKE tests...
if (CMAKE_PROJECT_NAME STREQUAL "CMAKE_TRY_COMPILE")
    return()
endif()

message(" *** econet.cmake ***")

# Workaround for https://www.cmake.org/Bug/view.php?id=14075
set(CMAKE_CROSS_COMPILING ON)

# Platform specific
set(TARGET_PLATFORM "econet" CACHE STRING "EcoNet")

set(PLATFORM_BUILD_NAME "target-mips-trenchip-linux-uclibc_rjo_en7528_le_7592_7613")
message("Config: PRPLMESH_PLATFORM_BASE_DIR: ${PRPLMESH_PLATFORM_BASE_DIR}")
message("Config: PRPLMESH_PLATFORM_TOOLCHAIN_PREFIX: ${PRPLMESH_PLATFORM_TOOLCHAIN_PREFIX}")
message("Config: PRPLMESH_PLATFORM_TOOLCHAIN_C_COMPILER: ${PRPLMESH_PLATFORM_TOOLCHAIN_C_COMPILER}")
message("Config: PRPLMESH_PLATFORM_TOOLCHAIN_CXX_COMPILER: ${PRPLMESH_PLATFORM_TOOLCHAIN_CXX_COMPILER}")

#Platform base dir (SDK path)
if (NOT DEFINED PRPLMESH_PLATFORM_BASE_DIR)
    message(FATAL_ERROR "PRPLMESH_PLATFORM_BASE_DIR environment variable not defined!")
endif()
set(PLATFORM_BASE_DIR "$ENV{PRPLMESH_PLATFORM_BASE_DIR}" CACHE STRING "Platform Base Directory")

# Platform toolchain prefix
# PLATFORM_TOOLCHAIN_PREFIX example: "/usr/bin/mipsel-linux-gnu-")
if (NOT DEFINED PRPLMESH_PLATFORM_TOOLCHAIN_PREFIX)
    message(FATAL_ERROR "PLATFORM_TOOLCHAIN_PREFIX environment variable not defined!")
endif()
set(PLATFORM_TOOLCHAIN_PREFIX "${PRPLMESH_PLATFORM_TOOLCHAIN_PREFIX}" CACHE STRING "Platform Toolchain Prefix")

set(PLATFORM_BASE_DIR "${PRPLMESH_PLATFORM_BASE_DIR}" CACHE STRING "Platform Base Dir")
message(" PLATFORM_BASE_DIR: ${PLATFORM_BASE_DIR}")
message(" PLATFORM_TOOLCHAIN_PREFIX: ${PLATFORM_TOOLCHAIN_PREFIX}" )

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

message("${BoldWhite}Setting TOOLCHAIN for ${BoldGreen}'${TARGET_PLATFORM}'${BoldWhite} platform...${ColourReset}")


## IS THIS NECESSARY HERE??
# projects (mis)use `-isystem` to silence warnings from 3rd-party
# source (among other things). gcc6 introduces changes to search
# order which breaks this usage.
#   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70129
#   https://gitlab.kitware.com/cmake/cmake/issues/16291
#   https://gitlab.kitware.com/cmake/cmake/issues/16919
set(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES ${PLATFORM_TOOLCHAIN_DIR} CACHE STRING "")
set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES ${PLATFORM_TOOLCHAIN_DIR} CACHE STRING "")


message("Config: PKG_CONFIG_PATH: ${PKG_CONFIG_PATH} ")
message("Config: CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH} ")
message("Config: CMAKE_FIND_ROOT_PATH: ${CMAKE_FIND_ROOT_PATH} ")

## Toolchain Programs
if (NOT DEFINED PRPLMESH_PLATFORM_TOOLCHAIN_C_COMPILER)
    message(" PLATFORM_TOOLCHAIN_C_COMPILER not defined. Using default value: gcc-5 ")
    set(CMAKE_C_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}gcc-5)
else()
    set(CMAKE_C_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}${PRPLMESH_PLATFORM_TOOLCHAIN_C_COMPILER})
endif()
if (NOT DEFINED PRPLMESH_PLATFORM_TOOLCHAIN_CXX_COMPILER)
    message(" PLATFORM_TOOLCHAIN_CXX_COMPILER not defined. Using default value: g++-5 ")
    set(CMAKE_CXX_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}g++-5)
else()
    set(CMAKE_CXX_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}${PRPLMESH_PLATFORM_TOOLCHAIN_CXX_COMPILER})
endif()
set( CMAKE_AR "${PLATFORM_TOOLCHAIN_PREFIX}ar" CACHE FILEPATH "Archiver" )
set( CMAKE_RANLIB "${PLATFORM_TOOLCHAIN_PREFIX}ranlib" CACHE FILEPATH "Ranlib" )


# Target Environment (if not used it should be removed)
set(PLATFORM_BUILD_DIR      ${PLATFORM_BASE_DIR}/build_dir/${PLATFORM_BUILD_NAME})
set(PLATFORM_STAGING_DIR    ${PLATFORM_BASE_DIR})
#set(PLATFORM_INCLUDE_DIR    ${PLATFORM_STAGING_DIR}/usr/include)

add_definitions(-DBEEROCKS_ECONET)
# Default Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=/  " CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=/ " CACHE STRING "" FORCE)


#Linker options (static libc)
##set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
if(NOT BUILD_SHARED_LIBS)
	set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} -static-libgcc -static-libstdc++ -ldl ")
	#set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} -ldl ")
endif()

## COPIED FROM DEFAULT CMAKELISTS.TXT FOR LINUX TARGET ##
# Linker Options
# - Setting the "rpath-link" linker variable to help the linker resolve secondery dependecies
#   (Libraries that are not directly referenced by the executable, but a linked library)
# - Setting "rpath" linker to add the cmake installed library folder to the linker search path
#   (Removes the need to set LD_LIBRARY_PATH manually when installing to a none standard location)
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath=${CMAKE_INSTALL_FULL_LIBDIR},-rpath-link=${PLATFORM_STAGING_DIR}/usr/lib")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath=/tmp/prpl/lib/,-rpath-link=/tmp/prpl/lib")

# Platform link directories (remove if later not used!)
#link_directories(${PLATFORM_STAGING_DIR}/usr/lib)
#link_directories(${PKG_CONFIG_PATH})
#link_directories(${PLATFORM_STAGING_DIR}/lib_install)

# Set the TARGET_PLATFORM build definition
add_definitions(-DTARGET_PLATFORM=${TARGET_PLATFORM})
add_definitions(-DBEEROCKS_BRIDGE_IFACE=${BEEROCKS_BRIDGE_IFACE})
add_definitions(-DBEEROCKS_BH_WIRE_IFACE=${BEEROCKS_BH_WIRE_IFACE})
add_definitions(-DBEEROCKS_WLAN1_IFACE=${BEEROCKS_WLAN1_IFACE})
add_definitions(-DBEEROCKS_WLAN2_IFACE=${BEEROCKS_WLAN2_IFACE})

