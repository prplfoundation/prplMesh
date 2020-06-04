message("${BoldWhite}Preparing ${BoldGreen}common${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Default values
set(BEEROCKS_PLATFORM_INIT "false" CACHE STRING "Configure minimal set of dummy WLAN ifaces and bridge")

# Modules
add_subdirectory(beerocks)
