#!/bin/bash
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

scriptdir="$(cd "${0%/*}" || exit 1; pwd)"
rootdir="${scriptdir%/*/*}"

# shellcheck source=functions.sh
. "${rootdir}/tools/functions.sh"

main() {
    # Default docker arguments
    docker_args=(
        --workdir "${rootdir}"
        --user "${SUDO_UID:-$(id -u)}:${SUDO_GID:-$(id -g)}"
        -e "USER=${SUDO_USER:-${USER}}"
        -v "${rootdir}:${rootdir}"
        --entrypoint "./tools/maptools.py"
    )

    # Add platform base directory mapping into the container
    if [ -n "${PRPLMESH_PLATFORM_BASE_DIR}" ]; then
        docker_args+=(-v "${PRPLMESH_PLATFORM_BASE_DIR}:${PRPLMESH_PLATFORM_BASE_DIR}")
    fi

    run docker container run "${docker_args[@]}" --rm "${DOCKER_REGISTRY}prplmesh-builder" build "$@"
}

main "$@"
