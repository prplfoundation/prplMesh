project(beerocks_cli VERSION ${prplmesh_VERSION})
message("${BoldWhite}Preparing ${BoldGreen}${PROJECT_NAME}${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Dependecies
find_package(readline)

# Set the base path for the current module
set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

file(GLOB cli_sources ${MODULE_PATH}/*.cpp)

# Module LIBS
set(cli_libraries bcl btlvf bml)

# In some compiler versions, the static casts to pFunction that are done in the various cli
# implementations are interpreted as a negative shift. So disable the corresponding warning.
add_compile_options(-Wno-shift-negative-value)

add_executable(${PROJECT_NAME} ${cli_sources} ${cli_extra_sources})
target_link_libraries(${PROJECT_NAME} ${cli_libraries} tlvf elpp)
target_include_directories(${PROJECT_NAME} 
    PRIVATE
        ${MODULE_PATH}/../bml
        ${MODULE_PATH}/../../../external
        $<TARGET_PROPERTY:bcl,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:btlvf,INTERFACE_INCLUDE_DIRECTORIES>
)

if (readline_FOUND)
    target_link_libraries(${PROJECT_NAME} readline)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_READLINE)
endif()

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
