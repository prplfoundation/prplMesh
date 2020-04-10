# SPDX-License-Identifier: BSD-2-Clause-Patent
#
# SPDX-FileCopyrightText: 2019 the prplMesh contributors (see AUTHORS.md)
#
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

find_library(SAFEC_LIBRARY "libsafec-3.3.so")
find_path(SAFEC_INCLUDE_DIRS libsafec/safe_str_lib.h)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(safec DEFAULT_MSG
    SAFEC_LIBRARY
    SAFEC_INCLUDE_DIRS
)

if (safec_FOUND)
    add_library(safec UNKNOWN IMPORTED)

    # Includes
    set_target_properties(safec PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${SAFEC_INCLUDE_DIRS}"
    )

    # Library
    set_target_properties(safec PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${SAFEC_LIBRARY}"
    )

endif()
