# SPDX-License-Identifier: BSD-2-Clause-Patent
#
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
#
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

find_library(AMXB_LIBRARY "libamxb.so")
find_path(AMXB_INCLUDE_DIRS
    NAMES amxb/amxb.h
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(amxb DEFAULT_MSG
    AMXB_LIBRARY
    AMXB_INCLUDE_DIRS
)

if (amxb_FOUND)
    add_library(amxb UNKNOWN IMPORTED)

    # Includes
    set_target_properties(amxb PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${AMXB_INCLUDE_DIRS}/"
    )

    # Library
    set_target_properties(amxb PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${AMXB_LIBRARY}"
    )

endif()
