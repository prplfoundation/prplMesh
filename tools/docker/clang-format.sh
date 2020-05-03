#!/bin/sh
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

scriptdir="$(cd "${0%/*}" && pwd)"
rootdir="${scriptdir%/*/*}"

# shellcheck source=functions.sh
. "${rootdir}/tools/functions.sh"

usage() {
    echo "usage: $(basename "$0") [-hvd] [-i ip] [-n name] [-N network]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -v|--verbose - verbosity on"
    echo "      -t|--tag - prplmesh-builder tag to use (default is none)"
}

main() {
    if ! OPTS="$(getopt -o 'hvt:' --long verbose,help,tag:,options: -n 'parse-options' -- "$@")"; then err "Failed parsing options." >&2 ; usage; exit 1 ; fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -v | --verbose)     VERBOSE=true; shift ;;
            -h | --help)        usage; exit 0; shift ;;
            -t | --tag)         TAG=":$2"; shift; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    image="prplmesh-builder$TAG"
    dbg "VERBOSE=${VERBOSE}"
    dbg "IMAGE=${image}"

    docker image inspect "$image" >/dev/null 2>&1 || {
        [ -n "$TAG" ] && { err "image $image doesn't exist, aborting"; exit 1; }
        dbg "Image $image does not exist, creating..."
        run "${scriptdir}"/image-build.sh --image builder
    }

    DOCKEROPTS="-e USER=${SUDO_USER:-${USER}}
                -e SOURCES_DIR=${rootdir}
                -v ${rootdir}:${rootdir}
                --user=${SUDO_UID:-$(id -u)}:${SUDO_GID:-$(id -g)}
                --name prplMesh-clang-format"

    DOCKEROPTS="$DOCKEROPTS --rm"

    # TODO: replace DOCKEROPTS with a bash array (and require bash)
    # We disable SC2086 because of the DOCKEROPTS variable
    # shellcheck disable=SC2086
    run docker container run --entrypoint "${rootdir}/clang-format.sh" ${DOCKEROPTS} "${image}" "$@"
}

VERBOSE=false

main "$@"
