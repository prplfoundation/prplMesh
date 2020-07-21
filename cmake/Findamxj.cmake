# SPDX-License-Identifier: BSD-2-Clause-Patent
#
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
#
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

find_library(AMXJ_LIBRARY "libamxj.so")
find_path(AMXJ_INCLUDE_DIRS
    NAMES amxj/amxj_variant.h
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(amxj DEFAULT_MSG
    AMXJ_LIBRARY
    AMXJ_INCLUDE_DIRS
)

if (amxj_FOUND)
    add_library(amxj UNKNOWN IMPORTED)

    # Includes
    set_target_properties(amxj PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${AMXJ_INCLUDE_DIRS}/"
    )

    # Library
    set_target_properties(amxj PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${AMXJ_LIBRARY}"
    )

endif()
