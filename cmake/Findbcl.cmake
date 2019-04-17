set(BCL_LIB_NAME "beerocks::bcl")
set(BCL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/beerocks/bcl/include)
set(BCL_LIBRARY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libbcl.so)
###################################################################################################################################
# FIXME
# probably due to all the include_directories and link_directories in BWL, when the tree does not have the built framework,
# BWL build fails on missing <mapf/external/easylogging++.h>. So the fix is only in standalone mode to update the include dirs.
###################################################################################################################################
if(STANDALONE)
list(INSERT BCL_INCLUDE_DIRS 0 ${CMAKE_INSTALL_PREFIX}/include)
endif()

list(APPEND BCL_DEFINITIONS
    ELPP_FORCE_USE_STD_THREAD
    ELPP_NO_DEFAULT_LOG_FILE
    ELPP_THREAD_SAFE
)

add_library(${BCL_LIB_NAME} UNKNOWN IMPORTED)

# Include directory
set_target_properties(${BCL_LIB_NAME} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${BCL_INCLUDE_DIRS}"
)

# Global compilation definitions
set_target_properties(${BCL_LIB_NAME} PROPERTIES 
    INTERFACE_COMPILE_DEFINITIONS "${BCL_DEFINITIONS}"
)

# Library
set_target_properties(${BCL_LIB_NAME} PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${BCL_LIBRARY}"
)
