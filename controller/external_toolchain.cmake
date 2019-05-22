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
if (EXISTS "${CMAKE_SOURCE_DIR}/external_toolchain.cfg")
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

        # Do not use files from system paths
        set(NO_CMAKE_SYSTEM_PATH)

        # Default Compiler flags
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=${PLATFORM_STAGING_DIR}" CACHE STRING "" FORCE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=${PLATFORM_STAGING_DIR}" CACHE STRING "" FORCE)

        # Yocto Build
        add_definitions(-DYOCTO)

    else()

        message(FATAL_ERROR "${BoldRed}Unsupported platform type '${TARGET_PLATFORM}'!${ColourReset}")  

    endif()

else()
    message("${BoldWhite}Using default TOOLCHAIN...${ColourReset}")
    set(TARGET_PLATFORM "linux")
endif()
