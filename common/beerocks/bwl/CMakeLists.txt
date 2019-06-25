cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME bwl)
project(${PROJECT_NAME})

# Find required packages
find_package(bcl REQUIRED)

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Common Sources
file(GLOB_RECURSE bwl_common_sources ${MODULE_PATH}/common/*.c*)

# Support more permissive c++ language
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

# UGW
if (TARGET_PLATFORM STREQUAL "ugw")

    set(BWL_TYPE_DEFAULT "DWPAL")
    # Extra libraries
    list(APPEND BWL_LIBS rt fapiwlancommon dl helper ugwhelper blobmsg_json)
    # UGW platform libraries
    link_directories(${PLATFORM_STAGING_DIR}/usr/lib)
    # Extra libraries (for dwpal)
    link_directories(${PLATFORM_STAGING_DIR}/opt/lantiq/lib)
    # hostap dir
    set(HOSTAPD_DIR "${PLATFORM_BUILD_DIR}/iwlwav-hostap-2.6")
# RDKB
elseif(TARGET_PLATFORM STREQUAL "rdkb")

    set(BWL_TYPE_DEFAULT "DWPAL")
    # Extra libraries
    list(APPEND BWL_LIBS rt dl)
    # Extre directories
    include_directories(${PLATFORM_INCLUDE_DIR}/wav-dpal)

# Linux
elseif(TARGET_PLATFORM STREQUAL "linux")
    set(BWL_TYPE_DEFAULT "DUMMY")
endif()

set(BWL_TYPE ${BWL_TYPE_DEFAULT} CACHE STRING "Which BWL backend to use")

message("${BoldWhite}Preparing ${BoldGreen}${PROJECT_NAME}${BoldWhite} ${BWL_TYPE} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

##########################################################################
##########################################################################
##########################################################################

if (BWL_TYPE STREQUAL "WAV_FAPI")

    add_definitions(-DUSE_FAPI_DAEMON)

    file(GLOB bwl_platform_sources
        ${bwl_platform_sources}
        ${MODULE_PATH}/wav_fapi/*.c*
    )

elseif(BWL_TYPE STREQUAL "WAV")

    include_directories(
        ${HOSTAPD_DIR}/src/utils
        ${HOSTAPD_DIR}/src/common
        ${HOSTAPD_DIR}/src/drivers
    )

    add_definitions(
        -DCONFIG_CTRL_IFACE
        -DCONFIG_CTRL_IFACE_UNIX
    )

    # WPA Control Interface
    file(GLOB bwl_platform_sources
        ${HOSTAPD_DIR}/src/common/wpa_ctrl.c
        ${HOSTAPD_DIR}/src/utils/os_unix.c
    )

    file(GLOB bwl_platform_sources
        ${bwl_platform_sources}
        ${MODULE_PATH}/wav/*.c*
    )

elseif(BWL_TYPE STREQUAL "DWPAL")
    
    file(GLOB bwl_platform_sources
        ${bwl_platform_sources}
        ${MODULE_PATH}/dwpal/*.c*
    )

    find_package(dwpal REQUIRED)
    list(APPEND BWL_LIBS dwpal::dwpal)

elseif(BWL_TYPE STREQUAL "DUMMY")

    file(GLOB bwl_platform_sources
        ${bwl_platform_sources}
        ${MODULE_PATH}/dummy/mon_wlan_hal_dummy.cpp
        ${MODULE_PATH}/dummy/ap_wlan_hal_dummy.cpp
        ${MODULE_PATH}/dummy/base_wlan_hal_dummy.cpp
    )

endif()

# Build the library
add_library(${PROJECT_NAME} SHARED ${bwl_common_sources} ${bwl_platform_sources})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${${PROJECT}_VERSION_STRING} SOVERSION ${${PROJECT}_VERSION_MAJOR})
target_link_libraries(${PROJECT_NAME} beerocks::bcl ${BWL_LIBS})
target_include_directories(${PROJECT_NAME}
    PRIVATE
        $<TARGET_PROPERTY:beerocks::bcl,INTERFACE_INCLUDE_DIRECTORIES>
        ${PLATFORM_INCLUDE_DIR}
    PUBLIC
        $<INSTALL_INTERFACE:include>
)

# Install
install(
    TARGETS ${PROJECT_NAME} EXPORT bwlConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

file(GLOB bwl_install_files ${MODULE_PATH}/common/*.h)
install(FILES ${bwl_install_files} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/beerocks/${PROJECT_NAME})
install(EXPORT bwlConfig NAMESPACE beerocks:: DESTINATION lib/cmake/beerocks/${PROJECT_NAME})
