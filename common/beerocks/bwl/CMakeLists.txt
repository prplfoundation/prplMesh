project(bwl VERSION ${prplmesh_VERSION})

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Common Sources
file(GLOB_RECURSE bwl_common_sources ${MODULE_PATH}/common/*.c*)

# Support more permissive c++ language
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

# Default defaults
set(BWL_TYPE "DUMMY")
add_definitions(-DBEEROCKS_TMP_PATH="${TMP_PATH}")

if (TARGET_PLATFORM STREQUAL "rdkb")

    set(BWL_TYPE "DWPAL")
    # Extra libraries
    list(APPEND BWL_LIBS rt dl)

elseif(TARGET_PLATFORM STREQUAL "openwrt")

    if (TARGET_PLATFORM_TYPE STREQUAL "ugw")
        set(BWL_TYPE "DWPAL")
    else()
        set(BWL_TYPE "NL80211")
    endif()

    # hostapd directory
    file(GLOB HOSTAPD_SEARCH_PATHS "${PLATFORM_BUILD_DIR}/hostapd*/hostapd-*")
    find_path(HOSTAPD_INCLUDE_DIR NAMES "src/common/wpa_ctrl.h" PATHS ${HOSTAPD_SEARCH_PATHS} NO_CMAKE_FIND_ROOT_PATH)
    set(HOSTAPD_DIR "${HOSTAPD_INCLUDE_DIR}")

endif()

set(BWL_TYPE ${BWL_TYPE} CACHE STRING "Which BWL backend to use")

##########################################################################
##########################################################################
##########################################################################

if(BWL_TYPE STREQUAL "DWPAL")
    
    include_directories(
        ${HOSTAPD_DIR}/src/drivers
        ${PLATFORM_BUILD_DIR}/iwlwav-iw-4.14
    )

    file(GLOB bwl_platform_sources
        ${bwl_platform_sources}
        ${MODULE_PATH}/shared/*.c*
        ${MODULE_PATH}/dwpal/*.c*
    )

    find_package(dwpal REQUIRED)
    find_package(nl-3 REQUIRED)
    find_package(nl-genl-3 REQUIRED)
    list(APPEND BWL_LIBS dwpal nl-3 nl-genl-3)

    # safec library can come from either libsafec or slibc
    find_package(safec QUIET)
    if (safec_FOUND)
        list(APPEND BWL_LIBS safec)
        add_definitions(-DUSE_LIBSAFEC)

        # Signal libsafec that we support the C99 standard
        add_definitions(-DHAVE_C99)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
    else()
        find_package(slibc REQUIRED)
        list(APPEND BWL_LIBS slibc)
        add_definitions(-DUSE_SLIBC)
    endif()

elseif(BWL_TYPE STREQUAL "NL80211")

    file(GLOB bwl_platform_sources
        ${MODULE_PATH}/shared/*.c*
        ${MODULE_PATH}/nl80211/*.c*

        # WPA Control Interface
        ${HOSTAPD_DIR}/src/common/wpa_ctrl.c
        ${HOSTAPD_DIR}/src/utils/os_unix.c
    )

    # Enable WPA control interface
    add_definitions(
        -DCONFIG_CTRL_IFACE
        -DCONFIG_CTRL_IFACE_UNIX
    )

    # Hostapd/NL80211 include directories
    include_directories(
        ${HOSTAPD_DIR}/src/utils
        ${HOSTAPD_DIR}/src/common
        ${HOSTAPD_DIR}/src/drivers
        ${PLATFORM_INCLUDE_DIR}/libnl-tiny
    )

    # Platform libraries
    link_directories(
        ${PLATFORM_STAGING_DIR}/usr/lib
    )

    list(APPEND BWL_LIBS nl-tiny)

elseif(BWL_TYPE STREQUAL "DUMMY")

    set(bwl_platform_sources
        ${bwl_platform_sources}
        ${MODULE_PATH}/dummy/nl80211_client_dummy.cpp
        ${MODULE_PATH}/dummy/nl80211_client_factory_dummy.cpp
        ${MODULE_PATH}/dummy/mon_wlan_hal_dummy.cpp
        ${MODULE_PATH}/dummy/ap_wlan_hal_dummy.cpp
        ${MODULE_PATH}/dummy/base_wlan_hal_dummy.cpp
        ${MODULE_PATH}/dummy/sta_wlan_hal_dummy.cpp
    )
    
else()

    string(TOLOWER ${BWL_TYPE} BWL_PATH)

    if(EXISTS ${MODULE_PATH}/${BWL_PATH})
        set(bwl_platform_sources
            ${bwl_platform_sources}
	    ${MODULE_PATH}/${BWL_PATH}/mon_wlan_hal_${BWL_PATH}.cpp
	    ${MODULE_PATH}/${BWL_PATH}/ap_wlan_hal_${BWL_PATH}.cpp
	    ${MODULE_PATH}/${BWL_PATH}/base_wlan_hal_${BWL_PATH}.cpp
	    ${MODULE_PATH}/${BWL_PATH}/sta_wlan_hal_${BWL_PATH}.cpp
        )
    else()
        message( FATAL_ERROR "BWL_TYPE not found" )
    endif()

endif()

# Build the library
add_library(${PROJECT_NAME} ${bwl_common_sources} ${bwl_platform_sources})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-z,defs")
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${prplmesh_VERSION} SOVERSION ${prplmesh_VERSION_MAJOR})
target_link_libraries(${PROJECT_NAME} bcl elpp ${BWL_LIBS})
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PLATFORM_INCLUDE_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(
    TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
