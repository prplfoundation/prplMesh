project(bcl VERSION ${prplmesh_VERSION})

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

find_package(PythonInterp REQUIRED)

message(STATUS ${CMAKE_CURRENT_BINARY_DIR})

message("${BoldWhite}Preparing ${BoldGreen}${PROJECT_NAME}${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Common Sources
file(GLOB_RECURSE bcl_sources ${MODULE_PATH}/source/*.c*)

# Build the library
add_library(${PROJECT_NAME} ${bcl_sources})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${prplmesh_VERSION} SOVERSION ${prplmesh_VERSION_MAJOR})

# Includes
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if (TARGET_PLATFORM STREQUAL "econet")
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../econet/include
    )
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC tlvf elpp)

install(TARGETS ${PROJECT_NAME} EXPORT bclConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install external header files
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

install(EXPORT bclConfig NAMESPACE beerocks:: DESTINATION lib/cmake/beerocks/${PROJECT_NAME})

add_definitions(-DBEEROCKS_VERSION="${prplmesh_VERSION}")
