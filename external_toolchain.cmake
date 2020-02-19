set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

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

# Check if "external_toolchain.cfg" file exists
if (EXISTS "${CMAKE_SOURCE_DIR}/external_toolchain.cfg" AND IS_SYMLINK "${CMAKE_SOURCE_DIR}/external_toolchain.cfg")

    file(STRINGS "external_toolchain.cfg" ConfigContents)
    foreach(NameAndValue ${ConfigContents})
        # Strip leading spaces
        string(REGEX REPLACE "^[ ]+" "" NameAndValue ${NameAndValue})

        # Find variable name
        string(REGEX MATCH "^[^=]+" Name ${NameAndValue})

        # Find the value
        string(REPLACE "${Name}=" "" Value ${NameAndValue})

        # Set the variable
        set(${Name} "${Value}")
    endforeach()

    if (NOT TARGET_PLATFORM)
        message(FATAL_ERROR "${BoldRed}TARGET_PLATFORM NOT DEFINED!${ColourReset}")
    endif()

    message("${BoldWhite}Setting TOOLCHAIN for ${BoldGreen}'${TARGET_PLATFORM}'${BoldWhite} platform...${ColourReset}")

    # UGW
    if (TARGET_PLATFORM STREQUAL "ugw")
        set(CMAKE_C_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}gcc)
        set(CMAKE_CXX_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}g++)

        # Target Environment
        set(PLATFORM_BUILD_DIR      ${PLATFORM_BASE_DIR}/build_dir/${PLATFORM_BUILD_NAME})
        set(PLATFORM_STAGING_DIR    ${PLATFORM_BASE_DIR}/staging_dir/${PLATFORM_BUILD_NAME})
        set(PLATFORM_INCLUDE_DIR    ${PLATFORM_STAGING_DIR}/usr/include)
        set(CMAKE_FIND_ROOT_PATH    ${PLATFORM_STAGING_DIR})
        set(ENV{PKG_CONFIG_PATH}  "${PLATFORM_STAGING_DIR}/usr/lib/pkgconfig")
        set(ENV{DEPENDENCIES_PATH}  "${PLATFORM_STAGING_DIR}/usr/lib/pkgconfig")

        # Platform link directories
        link_directories(${PLATFORM_STAGING_DIR}/usr/lib)
    
    elseif (TARGET_PLATFORM STREQUAL "rdkb")
        set(CMAKE_C_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}gcc)
        set(CMAKE_CXX_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}g++)

        # Target Environment
        set(PLATFORM_BUILD_DIR      ${PLATFORM_BASE_DIR})
        set(PLATFORM_STAGING_DIR    ${PLATFORM_BUILD_DIR}/${PLATFORM_BUILD_NAME})
        set(PLATFORM_INCLUDE_DIR    ${PLATFORM_STAGING_DIR}/usr/include)
        set(CMAKE_FIND_ROOT_PATH    ${PLATFORM_STAGING_DIR})
        set(ENV{PKG_CONFIG_PATH}  "${PLATFORM_STAGING_DIR}/usr/lib/pkgconfig")
        # Do not use files from system paths
        set(NO_CMAKE_SYSTEM_PATH)

        # Default Compiler flags
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=${PLATFORM_STAGING_DIR}" CACHE STRING "" FORCE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=${PLATFORM_STAGING_DIR}" CACHE STRING "" FORCE)

        # RDKB Build
        add_definitions(-DBEEROCKS_RDKB -DYOCTO)

    elseif (TARGET_PLATFORM STREQUAL "econet")
    
    
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
        set(CMAKE_C_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}gcc-5)
        set(CMAKE_CXX_COMPILER ${PLATFORM_TOOLCHAIN_PREFIX}g++-5)




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
	# a) ECONET TOOLCHAIN: no sysroot?
        #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=${PLATFORM_STAGING_DIR}" CACHE STRING "" FORCE)
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=${PLATFORM_STAGING_DIR}" CACHE STRING "" FORCE)
        #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=/usr/mipsel-linux-gnu/" CACHE STRING "" FORCE)
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=/usr/mipsel-linux-gnu/" CACHE STRING "" FORCE)
	# b) NEW TOOLCHAIN: sysroot
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=/" CACHE STRING "" FORCE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=/" CACHE STRING "" FORCE)

        # a) Add experimental features C++ (ECONET TOOLCHAIN)
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x " CACHE STRING "" FORCE)

	#Linker options (static libc)
	#NEW TOOLCHAIN
	set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++  ${CMAKE_CSS_STANDARD_LIBRARIES}")
	##set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")


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
        
        # Platform link directories
        #link_directories(${PLATFORM_STAGING_DIR}/usr/lib)
        #link_directories(${PKG_CONFIG_PATH})
        #link_directories(${PLATFORM_STAGING_DIR}/lib_install)
        set(TARGET_PLATFORM  "linux")

    else()

        message(FATAL_ERROR "${BoldRed}Unsupported platform type '${TARGET_PLATFORM}'!${ColourReset}")  

    endif()

    # Set the TARGET_PLATFORM build definition
    add_definitions(-DTARGET_PLATFORM=${TARGET_PLATFORM})

else()
    message("${BoldWhite}Using default TOOLCHAIN...${ColourReset}")
endif()
