cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME beerocks_cli)
project (${PROJECT_NAME})
message("${BoldWhite}Preparing ${BoldGreen}${PROJECT_NAME}${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Dependecies
find_package(bcl REQUIRED)
find_package(btlvf REQUIRED)
find_package(Tlvf REQUIRED)
find_package(elpp REQUIRED)
find_package(readline REQUIRED)
find_package(ncurses REQUIRED)

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

file(GLOB cli_sources ${MODULE_PATH}/*.cpp)

# Module LIBS
set(cli_libraries readline ncurses beerocks::bcl beerocks::btlvf bml)

add_executable(${PROJECT_NAME} ${cli_sources} ${cli_extra_sources})
target_link_libraries(${PROJECT_NAME} ${cli_libraries} tlvf elpp)
target_include_directories(${PROJECT_NAME} 
    PRIVATE
        ${MODULE_PATH}/../bml
        ${MODULE_PATH}/../../../external
        $<TARGET_PROPERTY:beerocks::bcl,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:beerocks::btlvf,INTERFACE_INCLUDE_DIRECTORIES>
)

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})