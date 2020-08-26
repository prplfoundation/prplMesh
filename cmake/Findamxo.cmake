# SPDX-License-Identifier: BSD-2-Clause-Patent
#
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
#
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

find_library(AMXO_LIBRARY "libamxo.so")
find_path(AMXO_INCLUDE_DIRS
    NAMES amxo/amxo.h
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(amxo DEFAULT_MSG
    AMXO_LIBRARY
    AMXO_INCLUDE_DIRS
)

if (amxo_FOUND)
    add_library(amxo UNKNOWN IMPORTED)

    # Includes
    set_target_properties(amxo PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${AMXO_INCLUDE_DIRS}/"
    )

    # Library
    set_target_properties(amxo PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${AMXO_LIBRARY}"
    )

endif()
