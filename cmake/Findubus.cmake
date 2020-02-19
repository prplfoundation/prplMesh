find_library(UBUS_LIBRARY "ubus")
find_path(UBUS_INCLUDE_DIRS libubus.h)

if(NOT UBUS_LIBRARY OR NOT UBUS_INCLUDE_DIRS)
    message("UBUS_LIBRARY: ${UBUS_LIBRARY}")
    message("UBUS_INCLUDE_DIRS: ${UBUS_INCLUDE_DIRS}")
	message(FATAL_ERROR "libubus not found!")
endif()

add_library(ubus SHARED IMPORTED)

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
# Include directories
set_target_properties(ubus PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${UBUS_INCLUDE_DIRS}"
)
endif()

if (TARGET_PLATFORM STREQUAL "econet")
# Include directories
set_target_properties(ubus PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${UBUS_INCLUDE_DIRS}"
)
endif()




# Library
set_target_properties(ubus PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${UBUS_LIBRARY}"
)
