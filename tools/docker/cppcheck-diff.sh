#!/bin/bash
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

scriptdir="$(cd "${0%/*}" && pwd)"
rootdir="$(realpath "$scriptdir/../..")"

docker run --rm -it -w "$rootdir" -v "$rootdir:$rootdir" --user="${SUDO_UID:-$UID}" prplfoundationinc/prplmesh-builder:alpine-3.11.3 "$rootdir"/ci/cppcheck/cppcheck-new-issues.sh .
