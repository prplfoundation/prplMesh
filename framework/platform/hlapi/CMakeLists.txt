project(hlapi VERSION ${prplmesh_VERSION})

message("${BoldWhite}Preparing ${BoldGreen}${PROJECT_NAME}${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Common Sources
set(hlapi_sources ${MODULE_PATH}/hlapi.cpp)

# Include AMBIORIX in the build
message(STATUS "ENABLE_HLAPI: ${ENABLE_HLAPI}")
find_package(amxb REQUIRED)
find_package(amxc REQUIRED)
find_package(amxd REQUIRED)
find_package(amxj REQUIRED)
find_package(amxo REQUIRED)
find_package(amxp REQUIRED)
list(APPEND HLAPI_LIBS amxb amxc amxd amxj amxo amxp)


# Build the library
add_library(${PROJECT_NAME} ${hlapi_sources})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${prplmesh_VERSION} SOVERSION ${prplmesh_VERSION_MAJOR})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-z,defs")
target_link_libraries(${PROJECT_NAME} PRIVATE ${HLAPI_LIBS})

# Install
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/lib>
    )

install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
