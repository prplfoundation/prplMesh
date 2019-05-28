cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME bml)
project(${PROJECT_NAME})
message("${BoldWhite}Preparing ${BoldGreen}${PROJECT_NAME}${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Dependecies
find_package(elpp REQUIRED)

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Common Sources
file(GLOB bml_common_sources 
    ${MODULE_PATH}/*.c*
    ${MODULE_PATH}/internal/*.c*
)

#RDKB
if (TARGET_PLATFORM STREQUAL "rdkb")
    file(GLOB RDKB_SOURCES 
        ${MODULE_PATH}/rdkb/*.c* 
        ${MODULE_PATH}/rdkb/internal/*.c*
    )
    list(APPEND bml_common_sources  ${RDKB_SOURCES})
endif()

# Build the library
add_library(${PROJECT_NAME} SHARED ${bml_common_sources})
target_link_libraries(${PROJECT_NAME} PRIVATE bcl tlvf elpp btlvf)
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-z,defs" VERSION ${${PROJECT}_VERSION_STRING} SOVERSION ${${PROJECT}_VERSION_MAJOR})
target_include_directories(${PROJECT_NAME} PRIVATE
    ${MODULE_PATH}
    ${MODULE_PATH}/../../../external
    $<TARGET_PROPERTY:bcl,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:btlvf,INTERFACE_INCLUDE_DIRECTORIES>
)

# Install
install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
