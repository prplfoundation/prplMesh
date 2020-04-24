#!/bin/sh
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

scriptdir="$(cd "${0%/*}"; pwd)"
rootdir="${scriptdir%/*/*}"

. ${rootdir}/tools/functions.sh

usage() {
    echo "usage: $(basename $0) [-hvbt]"
    echo "  mandatory:"
    echo "      type - image type <runner/builder>"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -v|--verbose - verbosity on"
    echo "      -b|--base-image - Base OS image to use (Dockerfile 'FROM')"
    echo "      -t|--tag - tag to add to prplmesh-builder and prplmesh-runner images"
}

main() {
    OPTS=`getopt -o 'hvb:t:' --long verbose,help,base-image,tag -n 'parse-options' -- "$@"`

    if [ $? != 0 ] ; then err "Failed parsing options." >&2 ; usage; exit 1 ; fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -v | --verbose)         VERBOSE=true; shift ;;
            -h | --help)            usage; exit 0; shift ;;
            -b | --base-image)      IMAGE="$2"; shift ; shift ;;
            -t | --tag)             TAG=":$2"; shift ; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    dbg IMAGE=$IMAGE
    dbg TAG=$TAG
    dbg rootdir=$rootdir

    info "Base docker image $IMAGE"
    info "Generating builder docker image (prplmesh-builder$TAG)"
    run docker image build \
        --build-arg image=$IMAGE \
        --tag prplmesh-builder$TAG \
        ${scriptdir}/builder

    info "Generating runner docker image (prplmesh-runner$TAG)"
    run docker image build \
        --build-arg image=$IMAGE \
        --tag prplmesh-runner$TAG \
        ${scriptdir}/runner
}

VERBOSE=false
IMAGE="ubuntu:18.04"

main $@
