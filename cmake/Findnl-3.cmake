# SPDX-License-Identifier: BSD-2-Clause-Patent
#
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
#
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

find_library(NL3_LIBRARY "libnl-3.so")
find_path(NL3_INCLUDE_DIRS netlink/netlink.h PATH_SUFFIXES libnl3)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(nl-3 DEFAULT_MSG
    NL3_LIBRARY
    NL3_INCLUDE_DIRS
)

if (nl-3_FOUND)
    add_library(nl-3 UNKNOWN IMPORTED)

    # Includes
    set_target_properties(nl-3 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${NL3_INCLUDE_DIRS}/"
    )

    # Library
    set_target_properties(nl-3 PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${NL3_LIBRARY}"
    )

endif()
