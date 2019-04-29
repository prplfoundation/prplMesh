find_package(PkgConfig REQUIRED)

pkg_check_modules(JSONC json-c REQUIRED)

find_library(JSONC_LIBRARY "${JSONC_LIBRARIES}")
if(NOT JSONC_LIBRARY)
	message(FATAL_ERROR "libjson-c not found!")
endif()

add_library(json-c UNKNOWN IMPORTED)

# Workaround in case pkg-config finds json-c in the build server instead of the cross compiled toolchain - 
# For cross compile, we include all the staging_dir/usr/include so we don't need to explicitely include json-c dirs.
if (TARGET_PLATFORM STREQUAL "linux")
# Include directory
set_target_properties(json-c PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${JSONC_INCLUDE_DIRS}"
)
endif()

# Library
set_target_properties(json-c PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${JSONC_LIBRARY}"
)