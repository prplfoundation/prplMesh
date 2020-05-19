# SPDX-License-Identifier: BSD-2-Clause-Patent
#
# SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
#
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

set(CMAKE_FIND_ROOT_PATH ${CMAKE_PREFIX_PATH}/opt/intel;${CMAKE_FIND_ROOT_PATH})
find_library(DWPAL_LIBRARY "libdwpal.so")
if(NOT DWPAL_LIBRARY)
	message(FATAL_ERROR "libdwpal.so not found!")
endif()
find_path(DWPAL_INCLUDE_DIRS
    NAMES dwpal.h
    PATH_SUFFIXES wav-dpal
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dwpal DEFAULT_MSG
    DWPAL_LIBRARY
    DWPAL_INCLUDE_DIRS
)

if (dwpal_FOUND)
    add_library(dwpal UNKNOWN IMPORTED)

    # Includes
    set_target_properties(dwpal PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${DWPAL_INCLUDE_DIRS}/"
    )

    # Library
    set_target_properties(dwpal PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${DWPAL_LIBRARY}"
    )

endif()
