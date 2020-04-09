# SPDX-License-Identifier: BSD-2-Clause-Patent
#
# Copyright (c) 2019 the prplMesh contributors (see AUTHORS.md)
#
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

find_library(SLIBC_LIBRARY "libslibc.so")
find_path(SLIBC_INCLUDE_DIRS slibc/string.h)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(slibc DEFAULT_MSG
    SLIBC_LIBRARY
    SLIBC_INCLUDE_DIRS
)

if (slibc_FOUND)
    add_library(slibc UNKNOWN IMPORTED)

    # Includes
    set_target_properties(slibc PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${SLIBC_INCLUDE_DIRS}"
    )

    # Library
    set_target_properties(slibc PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${SLIBC_LIBRARY}"
    )

endif()
