#!/bin/bash
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################


scriptdir="$(cd "${0%/*}" && pwd)"
rootdir="$(realpath "$scriptdir/../..")"
# shellcheck source=functions.sh
. "${rootdir}/tools/functions.sh"

image="${DOCKER_REGISTRY}"prplmesh-builder-alpine
docker run --rm -it -w "$rootdir" -v "$rootdir:$rootdir" --user="${SUDO_UID:-$UID}"  "$image" "$rootdir"/tools/docker/static-analysis/cppcheck.sh -j"$(nproc)" .

# Also check for new issues:
docker run --rm -it -w "$rootdir" -v "$rootdir:$rootdir" --user="${SUDO_UID:-$UID}" "$image" "$rootdir"/ci/cppcheck/cppcheck-new-issues.sh .
