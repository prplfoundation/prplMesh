project(beerocks_controller VERSION ${prplmesh_VERSION})
message("${BoldWhite}Preparing ${BoldGreen}${PROJECT_NAME}${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

file(GLOB controller_sources ${MODULE_PATH}/*.cpp)
file(GLOB controller_tasks_sources ${MODULE_PATH}/tasks/*.cpp)
file(GLOB controller_db_sources ${MODULE_PATH}/db/*.cpp)

#RDKB
if (TARGET_PLATFORM STREQUAL "rdkb")
    file(GLOB RDKB_SOURCES ${MODULE_PATH}/tasks/rdkb/*.cpp)
    list(APPEND controller_tasks_sources  ${RDKB_SOURCES})
endif()

if (TARGET_PLATFORM STREQUAL "econet")
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/econet/include
    )
endif()


add_executable(${PROJECT_NAME} ${controller_sources} ${controller_tasks_sources} ${controller_db_sources})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-z,defs")

target_link_libraries(${PROJECT_NAME} bpl bcl btl tlvf elpp btlvf)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${MODULE_PATH}/../bml
)

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
