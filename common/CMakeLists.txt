message("${BoldWhite}Preparing ${BoldGreen}common${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Default values
set(BEEROCKS_PLATFORM_INIT "false")

# Modules
add_subdirectory(tlvf)
add_subdirectory(bpl)
add_subdirectory(beerocks)

if (NOT MSGLIB STREQUAL "None")
add_subdirectory(ieee1905_transport)
endif()
