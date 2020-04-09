#!/bin/sh
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

scriptdir="$(cd "${0%/*}" || exit; pwd)"

err() {
    printf '\033[1;31m%s\033[0m\n' "$*"
}

run() {
    echo "$*"
    "$@" || exit $?
}

usage() {
    echo "usage: $(basename $0) [-hrv]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -r|--rdkb-root - path to rdkb root (default is PWD)"
    echo "      -i|--build-image - Only build image"
    echo ""
}

build_image() {
    docker build --tag rdkb-builder "$scriptdir/"
}

main() {

    if ! OPTS=$(getopt -o 'hir:' --long help,image,rdkb-path: -n 'parse-options' -- "$@"); then
        err "Failed parsing options." >&2; usage; exit 1
    fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -h | --help)               usage; exit 0 ;;
            -i | --image)              BUILD_IMAGE=true; shift ;;
            -r | --rdkb-root)          RDKB_ROOT="$2"; shift 2 ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    [ $BUILD_IMAGE = true ] && build_image

    run docker container run -it --rm \
        --workdir="${RDKB_ROOT}" \
        -e HOME="${HOME}" \
        -e RDKB_ROOT="${RDKB_ROOT}" \
        -e USER="${USER}" \
        -e USER_ID="$(id -u)" \
        -e GROUP_ID="$(id -g)" \
        -v "${HOME}:${HOME}" \
        -v "${RDKB_ROOT}:${RDKB_ROOT}" \
        rdkb-builder "$@"
}

RDKB_ROOT="$PWD"
BUILD_IMAGE=false

main "$@"
