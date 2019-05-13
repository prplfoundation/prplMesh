cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME beerocks_agent)
project (${PROJECT_NAME})
message("${BoldWhite}Preparing ${BoldGreen}${PROJECT_NAME}${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Use BPL package from mAP Framework
find_package(bcl REQUIRED)
find_package(bwl REQUIRED)
find_package(btl REQUIRED)

##############################################
# There is an unresolved depenency of BPL in MapfCommon
# The dependency was not resolved due to BPL is to be replaced by Platform Service
# Both those find_package should be deleted as one.
find_package(MapfCommon REQUIRED)
find_package(bpl REQUIRED)
#############################################

find_package(Tlvf REQUIRED)
find_package(btlvf REQUIRED)
find_package(elpp REQUIRED)

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

file(GLOB beerocks_agent_sources 
    ${MODULE_PATH}/backhaul_manager/*.c*
    ${MODULE_PATH}/platform_manager/*.c*
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

add_executable(${PROJECT_NAME} ${beerocks_agent_sources})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-z,defs")
target_link_libraries(${PROJECT_NAME} rt dl beerocks::bcl beerocks::btl tlvf elpp beerocks::btlvf ${LIBS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${MODULE_PATH}/../../../external
)

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
