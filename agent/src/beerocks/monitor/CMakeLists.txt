cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME beerocks_monitor)
project (${PROJECT_NAME})
message("${BoldWhite}Preparing ${BoldGreen}${PROJECT_NAME}${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Dependecies
find_package(bcl REQUIRED)
find_package(bwl REQUIRED)

find_package(Tlvf REQUIRED)
find_package(btlvf REQUIRED)
find_package(elpp REQUIRED)

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

file(GLOB beerocks_monitor_sources 
    ${MODULE_PATH}/../../son/monitor/*.c*
    ${MODULE_PATH}/*.c*
)

# UGW
if (TARGET_PLATFORM STREQUAL "ugw")

    # UGW ugwhelper library
    add_library(ugwhelper SHARED IMPORTED)
    set_target_properties(ugwhelper PROPERTIES
        IMPORTED_LOCATION "${PLATFORM_STAGING_DIR}/usr/lib/libugwhelper.so"
    )

    # Platform specific libraries
    list(APPEND LIBS ugwhelper)

endif()

#RDKB
if (TARGET_PLATFORM STREQUAL "rdkb")
    file(GLOB RDKB_SOURCES ${MODULE_PATH}/rdkb/*.c*)
    list(APPEND beerocks_monitor_sources  ${RDKB_SOURCES})
endif()

add_executable(${PROJECT_NAME} ${beerocks_monitor_sources})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-z,defs")
target_link_libraries(${PROJECT_NAME} beerocks::bcl beerocks::btlvf rt dl tlvf elpp ${LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${MODULE_PATH}/../../son/monitor
    ${MODULE_PATH}/../../../external
    # $<TARGET_PROPERTY:beerocks::bwl,INTERFACE_INCLUDE_DIRECTORIES>
)

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})