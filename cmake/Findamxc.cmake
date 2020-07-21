# SPDX-License-Identifier: BSD-2-Clause-Patent
#
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
#
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

find_library(AMXC_LIBRARY "libamxc.so")
find_path(AMXC_INCLUDE_DIRS
    NAMES amxc/amxc.h
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(amxc DEFAULT_MSG
    AMXC_LIBRARY
    AMXC_INCLUDE_DIRS
)

if (amxc_FOUND)
    add_library(amxc UNKNOWN IMPORTED)

    # Includes
    set_target_properties(amxc PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${AMXC_INCLUDE_DIRS}/"
    )

    # Library
    set_target_properties(amxc PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${AMXC_LIBRARY}"
    )

endif()
