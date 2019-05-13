cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME beerocks_controller)
project (${PROJECT_NAME})
message("${BoldWhite}Preparing ${BoldGreen}${PROJECT_NAME}${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Dependecies
find_package(bcl REQUIRED)
find_package(btl REQUIRED)

find_package(Tlvf REQUIRED)
find_package(btlvf REQUIRED)
find_package(elpp REQUIRED)

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

add_executable(${PROJECT_NAME} ${controller_sources} ${controller_tasks_sources} ${controller_db_sources})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-z,defs")

target_link_libraries(${PROJECT_NAME} beerocks::bcl beerocks::btl tlvf elpp beerocks::btlvf)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${MODULE_PATH}/../bml
    ${MODULE_PATH}/../../../external
    $<TARGET_PROPERTY:beerocks::bcl,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:beerocks::btlvf,INTERFACE_INCLUDE_DIRECTORIES>
)

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
