#!/bin/sh
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
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -v|--verbose - verbosity on"
    echo "      -b|--base-image - Base OS image to use (Dockerfile 'FROM')"
    echo "      -t|--tag - tag to add to prplmesh-builder and prplmesh-runner images"
}

pull_and_tag() {
    name="$1"
    base_image="$2"
    local_tag="$3"

    repo="registry.gitlab.com/prpl-foundation/prplmesh/$name-$base_image"

    info "Pulling $name from $repo"
    run docker pull "$repo"
    run docker tag "$repo" "prplmesh-$name$local_tag"
}

main() {
    if ! OPTS=$(getopt -o 'hb:t:' --long help,base-image,tag -n 'parse-options' -- "$@"); then
        err "Failed parsing options." >&2
        usage
        exit 1
    fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -h | --help)            usage; exit 0; shift ;;
            -b | --base-image)      IMAGE="$2"; shift ; shift ;;
            -t | --tag)             TAG=":$2"; shift ; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    dbg "TAG=$TAG"
    dbg "rootdir=$rootdir"

    # The registry used in Gitlab has the base image name appended with : converted to -
    base_image="$(echo "$IMAGE" | tr : -)"
    info "Base docker image $IMAGE"

    pull_and_tag builder "$base_image" "$TAG"
    pull_and_tag runner "$base_image" "$TAG"
}

IMAGE="ubuntu:18.04"

main "$@"
