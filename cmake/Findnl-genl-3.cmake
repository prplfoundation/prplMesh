# SPDX-License-Identifier: BSD-2-Clause-Patent
#
# Copyright (c) 2020 Tomer Eliyahu (Intel Corporation)
#
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

find_library(NL3_GENL_LIBRARY "libnl-genl-3.so")
find_path(NL3_INCLUDE_DIRS netlink/genl/genl.h PATH_SUFFIXES libnl3)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(nl-genl-3 DEFAULT_MSG
    NL3_GENL_LIBRARY
    NL3_INCLUDE_DIRS
)

if (nl-genl-3_FOUND)
    add_library(nl-genl-3 UNKNOWN IMPORTED)

    # Includes
    set_target_properties(nl-genl-3 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${NL3_INCLUDE_DIRS}/"
    )

    # Library
    set_target_properties(nl-genl-3 PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION "${NL3_GENL_LIBRARY}"
    )

endif()
