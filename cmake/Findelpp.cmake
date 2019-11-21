set(ELPP_LIB_NAME "mapf::elpp")
set(ELPP_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/framework/external/easylogging)
set(ELPP_LIBRARY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libelpp.so.1.4.0)

add_definitions(-DELPP_THREAD_SAFE)
add_definitions(-DELPP_FORCE_USE_STD_THREAD)
add_definitions(-DELPP_NO_DEFAULT_LOG_FILE)
add_library(${ELPP_LIB_NAME} UNKNOWN IMPORTED)

# Include directory
set_target_properties(${ELPP_LIB_NAME} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ELPP_INCLUDE_DIRS}"
)

# Library
set_target_properties(${ELPP_LIB_NAME} PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${ELPP_LIBRARY}"
)
