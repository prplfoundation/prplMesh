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

usage() {
    echo "usage: $(basename "$0") [-hvbt]"
    echo "  mandatory:"
    echo "      -i|--image - specify the image(s) to build (can be specified multiple times)"
    echo "                   supported values: builder/runner"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -b|--base-image - Base OS image to use (Dockerfile 'FROM')"
    echo "      -t|--tag - tag to add to the built images"
    echo "      -v|--verbose - verbose output"
}

main() {
    if ! OPTS=$(getopt -o 'hb:t:i:v' --long help,base-image:,tag:,image:,verbose -n 'parse-options' -- "$@"); then
        err "Failed parsing options." >&2
        usage
        exit 1
    fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -h | --help)            usage; exit 0; shift ;;
            -b | --base-image)      BASE_IMAGE="$2"; shift ; shift ;;
            -t | --tag)             TAG=":$2"; shift ; shift ;;
            -i | --image)           BUILD_IMAGES="${BUILD_IMAGES}$2;"; shift ; shift ;;
            -v | --verbose)         export VERBOSE=true; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    dbg "BASE_IMAGE=${BASE_IMAGE}:${TAG}"
    dbg "TAG=${TAG}"
    dbg "BUILD_IMAGES=${BUILD_IMAGES}"
    dbg "rootdir=$rootdir"

    # Make sure that a build image was specified
    if [[ "${BUILD_IMAGES}" == ";" ]]; then
        err "build image not specified!"
        usage; exit 1
    fi

    if [[ "${BUILD_IMAGES}" == *";builder;"* ]]; then
        info "Generating builder docker image (prplmesh-builder$TAG)"
        info "Base docker image $BASE_IMAGE"
        run docker image build \
            --build-arg image="$BASE_IMAGE" \
            --tag "prplmesh-builder$TAG" \
            "${scriptdir}/builder"
    fi

    if [[ "${BUILD_IMAGES}" == *";runner;"* ]]; then
        info "Generating runner docker image (prplmesh-runner$TAG)"
        info "Base docker image $BASE_IMAGE"
        run docker image build \
            --build-arg image="$BASE_IMAGE" \
            --tag "prplmesh-runner$TAG" \
            "${scriptdir}/runner"
    fi
}

BASE_IMAGE="ubuntu:18.04"
BUILD_IMAGES=";"

main "$@"
