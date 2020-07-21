# SPDX-License-Identifier: BSD-2-Clause-Patent
#
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
#
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

find_library(AMXD_LIBRARY "libamxd.so")
find_path(AMXD_INCLUDE_DIRS
    NAMES amxd_object_function.h amxd_object.h amxd_object_expression.h amxd_parameter_action.h amxd_object_parameter.h amxd_object_hierarchy.h amxd_action.h amxd_transaction.h amxd_function.h amxd_parameter.h amxd_object_event.h amxd_common.h amxd_object_action.h
    PATH_SUFFIXES amxd
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(amxd DEFAULT_MSG
    AMXD_LIBRARY
    AMXD_INCLUDE_DIRS
)

if (amxd_FOUND)
    add_library(amxd UNKNOWN IMPORTED)

    # Includes
    set_target_properties(amxd PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${AMXD_INCLUDE_DIRS}/"
    )

    # Library
    set_target_properties(amxd PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${AMXD_LIBRARY}"
    )

endif()
