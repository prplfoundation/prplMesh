cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME bcl)
project(${PROJECT_NAME})

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

find_package(PythonInterp REQUIRED)
find_package(elpp REQUIRED)

message(STATUS ${CMAKE_CURRENT_BINARY_DIR})

message("${BoldWhite}Preparing ${BoldGreen}${PROJECT_NAME}${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Common Sources
file(GLOB_RECURSE bcl_sources ${MODULE_PATH}/source/*.c*)

# Build the library
add_library(${PROJECT_NAME} ${bcl_sources})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${${PROJECT}_VERSION_STRING} SOVERSION ${${PROJECT}_VERSION_MAJOR})

# Includes
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME} PUBLIC tlvf elpp)

# ELPP Definitions - Added as public since all modules that use
# the easylogging++ library should define these
target_compile_definitions(${PROJECT_NAME} 
    PUBLIC 
        ELPP_FORCE_USE_STD_THREAD
        ELPP_NO_DEFAULT_LOG_FILE
        ELPP_THREAD_SAFE
)

install(TARGETS ${PROJECT_NAME} EXPORT bclConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install external header files
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

install(EXPORT bclConfig NAMESPACE beerocks:: DESTINATION lib/cmake/beerocks/${PROJECT_NAME})

add_definitions(-DBEEROCKS_VERSION="${${PROJECT}_VERSION_STRING}")
