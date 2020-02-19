set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)


#set(CMAKE_AR ${PLATFORM_TOOLCHAIN_PREFIX}ar)
set(CMAKE_C_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}gcc-5)
#set(CMAKE_C_ARCHIVE_CREATE ${PLATFORM_TOOLCHAIN_PREFIX}ar)
set(CMAKE_CXX_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}g++-5)
#set(CMAKE_CXX_ARCHIVE_CREATE ${PLATFORM_TOOLCHAIN_PREFIX}ar)


# Workaround for https://www.cmake.org/Bug/view.php?id=14075
set(CMAKE_CROSS_COMPILING ON)

# Platform specific
set(TARGET_PLATFORM "econet" CACHE STRING "EcoNet")

# 
# #TARGET_PLATFORM=mips
# TARGET_PLATFORM=econet
# #TARGET_PLATFORM=linux
# 
# PLATFORM_BUILD_NAME=target-mips-trenchip-linux-uclibc_rjo_en7528_le_7592_7613
# 
# 
# ##a) ECONET TOOLCHAIN
# ##PLATFORM_TOOLCHAIN_PREFIX=/opt/ugw/toolchain/64bit/toolchain-mips_mips32_gcc-4.8-linaro_uClibc-0.9.33.2_linux_3_10/bin/mips-openwrt-linux-uclibc-
# ##PLATFORM_TOOLCHAIN_PREFIX=/home/juan/Development/customers/EcoNet/sw/mips-x86_64.linux-xgcc/mips/bin/mips-
# ##PLATFORM_BASE_DIR=/home/vbukhovsky/work/GRX/UGW-7.2-Repo/beerocks_1_2_wcci_master_1.2_latest/ugw_sw/ugw/openwrt/core
# ##PLATFORM_BASE_DIR=/home/juan/Development/customers/EcoNet/prpl_build3/prplmesh_root_econet/econet
# ##PLATFORM_BASE_DIR=/home/juan/Development/customers/EcoNet/src/sdk/
# #PLATFORM_BASE_DIR=/home/juan/Development/customers/EcoNet/releasebsp_testprof
# #PLATFORM_TOOLCHAIN_PREFIX=/opt/trendchip/mipsel-linux-uclibc-4.6.3-kernel3.18/usr/bin/mipsel-buildroot-linux-uclibc-
# 
# 
# 
# ## b) UBUNTU MIPSEL TOOLCHAIN
# PLATFORM_TOOLCHAIN_PREFIX=/usr/bin/mipsel-linux-gnu-
# PLATFORM_BASE_DIR=/home/juan/Development/customers/EcoNet/releasebsp_testprof
# PLATFORM_BUILD_NAME=target-mips-trenchip-linux-uclibc_rjo_en7528_le_7592_7613

set(PLATFORM_BUILD_NAME "target-mips-trenchip-linux-uclibc_rjo_en7528_le_7592_7613")
set(PLATFORM_TOOLCHAIN_PREFIX "/usr/bin/mipsel-linux-gnu-")
set(PLATFORM_BASE_DIR "/home/juan/Development/customers/EcoNet/releasebsp_testprof")
set(PLATFORM_BUILD_NAME "target-mips-trenchip-linux-uclibc_rjo_en7528_le_7592_76132")



# # Base RDKB build dir (e.g. /path/to/atom_rdkbos)
# if (NOT DEFINED ENV{PRPLMESH_PLATFORM_BASE_DIR})
#     message(FATAL_ERROR "PLATFORM_BASE_DIR environment variable not defined!")
# endif()
# set(PLATFORM_BASE_DIR "$ENV{PRPLMESH_PLATFORM_BASE_DIR}" CACHE STRING "Platform Base Directory")
# 
# # RDKB build name (e.g. puma7-atom)
# if (NOT DEFINED ENV{PRPLMESH_PLATFORM_BUILD_NAME})
#     message(FATAL_ERROR "PRPLMESH_PLATFORM_BUILD_NAME environment variable not defined!")
# endif()
# set(PLATFORM_BUILD_NAME "$ENV{PRPLMESH_PLATFORM_BUILD_NAME}" CACHE STRING "Platform Build Name")
# 
# # Platform toolchain name (e.g. x86_64-linux)
# if (NOT DEFINED ENV{PRPLMESH_PLATFORM_TOOLCHAIN})
#     message(FATAL_ERROR "PRPLMESH_PLATFORM_TOOLCHAIN environment variable not defined!")
# endif()
# set(PLATFORM_TOOLCHAIN "$ENV{PRPLMESH_PLATFORM_TOOLCHAIN}" CACHE STRING "Platform Toolchain")
# 
# # Platform toolchain prefix (e.g. i686-rdk-linux)
# if (NOT DEFINED ENV{PRPLMESH_PLATFORM_TOOLCHAIN_PREFIX})
#     message(FATAL_ERROR "PLATFORM_TOOLCHAIN_PREFIX environment variable not defined!")
# endif()
# set(PLATFORM_TOOLCHAIN_PREFIX "$ENV{PRPLMESH_PLATFORM_TOOLCHAIN_PREFIX}" CACHE STRING "Platform Toolchain Prefix")


# Skip this file for internal CMAKE tests...
if (CMAKE_PROJECT_NAME STREQUAL "CMAKE_TRY_COMPILE")
    return()
endif()

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

# # Check if "external_toolchain.cfg" file exists
# if (EXISTS "${CMAKE_SOURCE_DIR}/external_toolchain.cfg" AND IS_SYMLINK "${CMAKE_SOURCE_DIR}/external_toolchain.cfg")
# 
#     file(STRINGS "external_toolchain.cfg" ConfigContents)
#     foreach(NameAndValue ${ConfigContents})
#         # Strip leading spaces
#         string(REGEX REPLACE "^[ ]+" "" NameAndValue ${NameAndValue})
# 
#         # Find variable name
#         string(REGEX MATCH "^[^=]+" Name ${NameAndValue})
# 
#         # Find the value
#         string(REPLACE "${Name}=" "" Value ${NameAndValue})
# 
#         # Set the variable
#         set(${Name} "${Value}")
#     endforeach()
# 
#     if (NOT TARGET_PLATFORM)
#         message(FATAL_ERROR "${BoldRed}TARGET_PLATFORM NOT DEFINED!${ColourReset}")
#     endif()

    message("${BoldWhite}Setting TOOLCHAIN for ${BoldGreen}'${TARGET_PLATFORM}'${BoldWhite} platform...${ColourReset}")

#     # UGW
#     if (TARGET_PLATFORM STREQUAL "ugw")
#         set(CMAKE_C_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}gcc)
#         set(CMAKE_CXX_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}g++)
# 
#         # Target Environment
#         set(PLATFORM_BUILD_DIR      ${PLATFORM_BASE_DIR}/build_dir/${PLATFORM_BUILD_NAME})
#         set(PLATFORM_STAGING_DIR    ${PLATFORM_BASE_DIR}/staging_dir/${PLATFORM_BUILD_NAME})
#         set(PLATFORM_INCLUDE_DIR    ${PLATFORM_STAGING_DIR}/usr/include)
#         set(CMAKE_FIND_ROOT_PATH    ${PLATFORM_STAGING_DIR})
#         set(ENV{PKG_CONFIG_PATH}  "${PLATFORM_STAGING_DIR}/usr/lib/pkgconfig")
#         set(ENV{DEPENDENCIES_PATH}  "${PLATFORM_STAGING_DIR}/usr/lib/pkgconfig")
# 
#         # Platform link directories
#         link_directories(${PLATFORM_STAGING_DIR}/usr/lib)
#     
#     elseif (TARGET_PLATFORM STREQUAL "rdkb")
#         set(CMAKE_C_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}gcc)
#         set(CMAKE_CXX_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}g++)
# 
#         # Target Environment
#         set(PLATFORM_BUILD_DIR      ${PLATFORM_BASE_DIR})
#         set(PLATFORM_STAGING_DIR    ${PLATFORM_BUILD_DIR}/${PLATFORM_BUILD_NAME})
#         set(PLATFORM_INCLUDE_DIR    ${PLATFORM_STAGING_DIR}/usr/include)
#         set(CMAKE_FIND_ROOT_PATH    ${PLATFORM_STAGING_DIR})
#         set(ENV{PKG_CONFIG_PATH}  "${PLATFORM_STAGING_DIR}/usr/lib/pkgconfig")
#         # Do not use files from system paths
#         set(NO_CMAKE_SYSTEM_PATH)
# 
#         # Default Compiler flags
#         set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=${PLATFORM_STAGING_DIR}" CACHE STRING "" FORCE)
#         set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=${PLATFORM_STAGING_DIR}" CACHE STRING "" FORCE)
# 
#         # RDKB Build
#         add_definitions(-DBEEROCKS_RDKB -DYOCTO)
# 
#     elseif (TARGET_PLATFORM STREQUAL "econet")
    
    
        # Workaround for https://www.cmake.org/Bug/view.php?id=14075
        set(CMAKE_CROSS_COMPILING ON)

        ## IS THIS NECESSARY HERE??
        ## IS THIS NECESSARY HERE??
        ## IS THIS NECESSARY HERE??
        # projects (mis)use `-isystem` to silence warnings from 3rd-party
        # source (among other things). gcc6 introduces changes to search
        # order which breaks this usage.
        #   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70129
        #   https://gitlab.kitware.com/cmake/cmake/issues/16291
        #   https://gitlab.kitware.com/cmake/cmake/issues/16919
        set(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES ${PLATFORM_TOOLCHAIN_DIR} CACHE STRING "")
        set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES ${PLATFORM_TOOLCHAIN_DIR} CACHE STRING "")
        
    
        message(" ***  econet in external_toolchain.cmake ***")
        message(" # PKG_CONFIG_PATH default value: ${PKG_CONFIG_PATH} ")
        message(" # CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH} ")
        message(" # CMAKE_FIND_ROOT_PATH: ${CMAKE_FIND_ROOT_PATH} ")
        
	#a) ECONET TOOLCHAIN
        #set(CMAKE_C_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}gcc)
        #set(CMAKE_CXX_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}g++)
	#b) NEW TOOLCHAIN
        #set(CMAKE_AR ${PLATFORM_TOOLCHAIN_PREFIX}ar)
        set(CMAKE_C_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}gcc-5)
        #set(CMAKE_C_ARCHIVE_CREATE ${PLATFORM_TOOLCHAIN_PREFIX}ar)
        set(CMAKE_CXX_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}g++-5)
        #set(CMAKE_CXX_ARCHIVE_CREATE ${PLATFORM_TOOLCHAIN_PREFIX}ar)
        #set(CMAKE_AR ${PLATFORM_TOOLCHAIN_PREFIX}ar)
        set(CMAKE_C_COMPILER_AR ${PLATFORM_TOOLCHAIN_PREFIX}ar)
        message("###CMAKE_AR: ${CMAKE_AR}####")

        set(CMAKE_C_COMPILER_AR ${PLATFORM_TOOLCHAIN_PREFIX}ar)
        set(CMAKE_CXX_COMPILER_AR ${PLATFORM_TOOLCHAIN_PREFIX}ar)
        set(CMAKE_C_COMPILER_RANLIB ${PLATFORM_TOOLCHAIN_PREFIX}ranlib)
        set(CMAKE_CXX_COMPILER_RANLIB ${PLATFORM_TOOLCHAIN_PREFIX}ranlib)

        set( CMAKE_AR "${PLATFORM_TOOLCHAIN_PREFIX}ar" CACHE FILEPATH "Archiver" )
        set( CMAKE_RANLIB "${PLATFORM_TOOLCHAIN_PREFIX}ranlib" CACHE FILEPATH "Ranlib" )



        # Target Environment
        set(PLATFORM_BUILD_DIR      ${PLATFORM_BASE_DIR}/build_dir/${PLATFORM_BUILD_NAME})
        #set(PLATFORM_STAGING_DIR    ${PLATFORM_BASE_DIR}/staging_dir/${PLATFORM_BUILD_NAME})
        set(PLATFORM_STAGING_DIR    ${PLATFORM_BASE_DIR})
        ##set(PLATFORM_INCLUDE_DIR    ${PLATFORM_STAGING_DIR}/usr/include)
        #set(PLATFORM_INCLUDE_DIR    ${PLATFORM_STAGING_DIR}/lib_install)
        
        #set(CMAKE_FIND_ROOT_PATH    ${PLATFORM_STAGING_DIR})
        #set(ENV{PKG_CONFIG_PATH}  "${PLATFORM_STAGING_DIR}/usr/lib/pkgconfig")
        #set(OPENSSL_ROOT_DIR 

        
        add_definitions(-DBEEROCKS_ECONET)
        # Default Compiler flags
    if(ECONET_TOOLCHAIN)
	# a) ECONET TOOLCHAIN: no sysroot?
        #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=${PLATFORM_STAGING_DIR}" CACHE STRING "" FORCE)
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=${PLATFORM_STAGING_DIR}" CACHE STRING "" FORCE)
        
        # a) Add experimental features C++ (ECONET TOOLCHAIN)
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x " CACHE STRING "" FORCE)

	# b) NEW TOOLCHAIN: sysroot
    else()
        #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=/usr/mipsel-linux-gnu/" CACHE STRING "" FORCE)
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=/usr/mipsel-linux-gnu/" CACHE STRING "" FORCE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=/  " CACHE STRING "" FORCE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=/ " CACHE STRING "" FORCE)

    endif()


    #Linker options (static libc)
	#NEW TOOLCHAIN
    if(NOT ECONET_TOOLCHAIN)
        ##set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
	if(NOT BUILD_SHARED_LIBS)
        	set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} -static-libgcc -static-libstdc++ -ldl ")
        	#set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} -ldl ")
	endif()
    endif()

	## COPIED FROM DEFAULT CMAKELISTS.TXT FOR LINUX TARGET ##
	## COPIED FROM DEFAULT CMAKELISTS.TXT FOR LINUX TARGET ##
	## COPIED FROM DEFAULT CMAKELISTS.TXT FOR LINUX TARGET ##
	# Linker Options
	# - Setting the "rpath-link" linker variable to help the linker resolve secondery dependecies
	#   (Libraries that are not directly referenced by the executable, but a linked library)
	# - Setting "rpath" linker to add the cmake installed library folder to the linker search path
	#   (Removes the need to set LD_LIBRARY_PATH manually when installing to a none standard location)
	#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath=${CMAKE_INSTALL_FULL_LIBDIR},-rpath-link=${PLATFORM_STAGING_DIR}/usr/lib")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath=/tmp/prpl/lib/,-rpath-link=/tmp/prpl/lib")
	#if (CMAKE_BUILD_TYPE STREQUAL "Release")
	#    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z noexecstack -z relro -z now -pie")
	#endif()

        # RDKB Build
        #add_definitions(-DBEEROCKS_RDKB -DYOCTO)

	set(BEEROCKS_BRIDGE_IFACE "br0")
	set(BEEROCKS_BH_WIRE_IFACE "eth0")
	set(BEEROCKS_WLAN1_IFACE "ra0")
	set(BEEROCKS_WLAN2_IFACE "rai0")
	
        
        # Platform link directories
        #link_directories(${PLATFORM_STAGING_DIR}/usr/lib)
        #link_directories(${PKG_CONFIG_PATH})
        #link_directories(${PLATFORM_STAGING_DIR}/lib_install)
        
        
        #set(TARGET_PLATFORM  "linux")

#     else()
# 
#         message(FATAL_ERROR "${BoldRed}Unsupported platform type '${TARGET_PLATFORM}'!${ColourReset}")  
# 
#     endif()

    # Set the TARGET_PLATFORM build definition
    add_definitions(-DTARGET_PLATFORM=${TARGET_PLATFORM})
    add_definitions(-DBEEROCKS_BRIDGE_IFACE=${BEEROCKS_BRIDGE_IFACE})
    add_definitions(-DBEEROCKS_BH_WIRE_IFACE=${BEEROCKS_BH_WIRE_IFACE})
    add_definitions(-DBEEROCKS_WLAN1_IFACE=${BEEROCKS_WLAN1_IFACE})
    add_definitions(-DBEEROCKS_WLAN2_IFACE=${BEEROCKS_WLAN2_IFACE})

# else()
#     message("${BoldWhite}Using default TOOLCHAIN...${ColourReset}")
# endif()
