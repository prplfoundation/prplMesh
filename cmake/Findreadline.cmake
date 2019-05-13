find_library(READLINE_LIBRARY "readline")
find_path(READLINE_INCLUDE_DIRS readline/readline.h)

if(NOT READLINE_LIBRARY OR NOT READLINE_INCLUDE_DIRS)
    message("READLINE_LIBRARY: ${READLINE_LIBRARY}")
    message("READLINE_INCLUDE_DIRS: ${READLINE_INCLUDE_DIRS}")
	message(FATAL_ERROR "libreadline not found!")
endif()

add_library(readline UNKNOWN IMPORTED)

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
set_target_properties(readline PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${READLINE_INCLUDE_DIRS}"
)
endif()

# Library
set_target_properties(readline PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${READLINE_LIBRARY}"
)