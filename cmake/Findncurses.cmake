find_library(NCURSES_LIBRARY "ncurses")
find_path(NCURSES_INCLUDE_DIRS ncurses.h)
message("NCURSES_LIBRARY: ${NCURSES_LIBRARY}")
message("NCURSES_INCLUDE_DIRS: ${NCURSES_INCLUDE_DIRS}")
if(NOT NCURSES_LIBRARY OR NOT NCURSES_INCLUDE_DIRS)
    message("NCURSES_LIBRARY: ${NCURSES_LIBRARY}")
    message("NCURSES_INCLUDE_DIRS: ${NCURSES_INCLUDE_DIRS}")
	message(FATAL_ERROR "libncurses not found!")
endif()

add_library(ncurses UNKNOWN IMPORTED)

#############################################################################################################################
# RDKB workaround - for some reason, if exposing INTERFACE_INCLUDE_DIRECTORIES for <prefix>/usr/include,
# we get a compiler error:
# <...>/usr/include/c++/6.4.0/cstdlib:75:25: fatal error: stdlib.h: No such file or directory
# This happens since cmake is generating the include flags with -isystem instead of -I. Similar issue 
# observed and discussed inin https://github.com/OpenLightingProject/ola/pull/1126 but they fix it in their build system
# which is not CMAKE.
# For now - simply do not set this property for none linux platforms - in UGW and RDKB we already have usr/include included
# TODO: Find a real solution...
#############################################################################################################################
if (TARGET_PLATFORM STREQUAL "linux")
# Include directory
set_target_properties(ncurses PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${NCURSES_INCLUDE_DIRS}"
)
endif()

# Library
set_target_properties(ncurses PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${NCURSES_LIBRARY}"
)