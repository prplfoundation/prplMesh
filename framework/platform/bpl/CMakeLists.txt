cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME bpl)
project(${PROJECT_NAME})

message("${BoldWhite}Preparing ${BoldGreen}${PROJECT_NAME}${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Common Sources
file(GLOB_RECURSE bpl_common_sources ${MODULE_PATH}/common/*.c*)

# Common libraries
find_package(elpp REQUIRED)

# OpenWRT
if (TARGET_PLATFORM STREQUAL "openwrt")

    # UGW
    if (TARGET_PLATFORM_TYPE STREQUAL "ugw")

        # Platform definitions
        add_definitions(-DBEEROCKS_UGW)

        # Platform specific includes
        include_directories(${PLATFORM_INCLUDE_DIR}/libnl3)
        include_directories(${PLATFORM_INCLUDE_DIR})

        # libbridge
        file(GLOB LIBBRIDGE_SEARCH_PATHS "${PLATFORM_BUILD_DIR}/linux-intel_mips*/bridge-utils*/libbridge")
        find_path(LIBBRIDGE_INCLUDE_DIR NAMES "libbridge.h" PATHS "${LIBBRIDGE_SEARCH_PATHS}" NO_CMAKE_FIND_ROOT_PATH)
        include_directories(${LIBBRIDGE_INCLUDE_DIR})
        link_directories(${LIBBRIDGE_INCLUDE_DIR})

        # UCI BPL
        file(GLOB_RECURSE bpl_platform_sources ${MODULE_PATH}/uci/*.c*)

        # UGW specific libraries
        find_package(ubus REQUIRED)
        find_package(safec REQUIRED)

        link_directories(${PLATFORM_STAGING_DIR}/usr/lib)
        list(APPEND BPL_LIBS ubus nl-3 nl-route-3 bridge dl uci ubox safec)

        # Signal libsafec that we support the C99 standard
        add_definitions(-DHAVE_C99)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

    else()

        file(GLOB_RECURSE bpl_platform_sources ${MODULE_PATH}/linux/*.c*)

        add_definitions(-DPLATFORM_DB_PATH="${INSTALL_PATH}/share/prplmesh_platform_db")
        add_definitions(-DPLATFORM_DB_PATH_TEMP="${TMP_PATH}/prplmesh_platform_db")
        install(FILES "${MODULE_PATH}/linux/prplmesh_platform_db" DESTINATION ${CMAKE_INSTALL_PREFIX}/share)

    endif()

elseif (TARGET_PLATFORM STREQUAL "rdkb")

    # Platform definitions
    add_definitions(-DBEEROCKS_RDKB)

    # Platform specific includes
    include_directories(${PLATFORM_INCLUDE_DIR})
    include_directories(${PLATFORM_INCLUDE_DIR}/libnl3)
    include_directories(${PLATFORM_INCLUDE_DIR}/bridge-utils)

    # UCI BPL
    file(GLOB_RECURSE bpl_platform_sources ${MODULE_PATH}/uci/*.c*)

    # RDKB specific libraries
    find_package(ubus REQUIRED)
    find_package(slibc REQUIRED)

    list(APPEND BPL_LIBS nl-3 nl-route-3 bridge dl swpal uci ubox ubus slibc)

elseif (TARGET_PLATFORM STREQUAL "linux")

    # Platform definitions
    add_definitions(-DBEEROCKS_LINUX)

    file(GLOB_RECURSE bpl_platform_sources ${MODULE_PATH}/linux/*.c*)

    add_definitions(-DPLATFORM_DB_PATH="${INSTALL_PATH}/share/prplmesh_platform_db")
    add_definitions(-DPLATFORM_DB_PATH_TEMP="${TMP_PATH}/prplmesh_platform_db")
    install(FILES "${MODULE_PATH}/linux/prplmesh_platform_db" DESTINATION ${CMAKE_INSTALL_PREFIX}/share)

else()

    message(WARNING "${BoldWhite}Skipping ${BoldGreen}${PROJECT_NAME}${BoldWhite} build for unsupported platform...${ColourReset}")
    return()

endif()

# Build the library
add_library(${PROJECT_NAME} SHARED ${bpl_common_sources} ${bpl_platform_sources})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${prplmesh_VERSION} SOVERSION ${${PROJECT}_VERSION_MAJOR})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-z,defs")
target_link_libraries(${PROJECT_NAME} PRIVATE mapf::elpp common ${BPL_LIBS})

# Install
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    )

install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

if(BUILD_TESTS)
  add_subdirectory(test)
endif()
