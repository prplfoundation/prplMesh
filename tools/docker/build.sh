#!/bin/sh
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

scriptdir="$(cd "${0%/*}"; pwd)"
rootdir="${scriptdir%/*/*}"

. ${rootdir}/tools/docker/functions.sh

main() {
    docker image inspect prplmesh-builder >/dev/null 2>&1 || {
        echo "Image prplmesh-build does not exist, creating..."
        run ${scriptdir}/image-build.sh
    }

    # Default docker arguments
    docker_args="\
    --workdir=$rootdir --user=${SUDO_UID:-$(id -u)}:${SUDO_GID:-$(id -g)} \
    -e USER=${SUDO_USER:-${USER}} -v ${rootdir}:${rootdir} \
    --entrypoint=./tools/maptools.py \
    "

    # Add platform base directory mapping into the container
    if [ ! -z "${PRPLMESH_PLATFORM_BASE_DIR}" ]; then
        docker_args="${docker_args} \
        -v ${PRPLMESH_PLATFORM_BASE_DIR}:${PRPLMESH_PLATFORM_BASE_DIR} \
        "
    fi

    run docker container run ${docker_args} --rm prplmesh-builder build $@
}

main $@
