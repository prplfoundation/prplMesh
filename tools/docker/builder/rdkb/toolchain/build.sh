#!/bin/sh
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2020 Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

scriptdir="$(cd "${0%/*}"; pwd)"
rootdir="${scriptdir%/*/*/*/*/*}"

. ${rootdir}/tools/docker/functions.sh

main() {

    if [ -z "$1" ]; then
        err "Usage: $0 <rdkb_dir> <command>"
        exit 1
    fi   
    RDKB_ROOT=$(realpath $1)
    shift

    # Default docker arguments
    docker_args="\
     --workdir=$PWD --user=${SUDO_UID:-$(id -u)}:${SUDO_GID:-$(id -g)} \
     -e USER=${SUDO_USER:-${USER}} -e HOME=${HOME} -v ${HOME}:${HOME} \
     -v ${RDKB_ROOT}:${RDKB_ROOT} -e RDKB_ROOT=${RDKB_ROOT} \
     "

    run docker container run -it --rm ${docker_args} rdkb-builder "$@"
}

main $@
