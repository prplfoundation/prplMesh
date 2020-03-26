#!/bin/sh -e
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2020 Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

usage() {
    echo "usage: $(basename "$0") [-hbd]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -v|--verbose - add verbosity"
    echo "      --branch  - branch to use (default master)"
    echo "      --device  - device to use (default netgear-rax40)"
    echo "      --path    - path to copy ipk to (default .)"
    
}

download() {
    # URL for downloading the prplmesh.ipk according to
    # https://docs.gitlab.com/ee/ci/pipelines/job_artifacts.html#downloading-the-latest-artifacts
    IPK_URL="https://gitlab.com/prpl-foundation/prplmesh/-/jobs/artifacts/$BRANCH/raw/build/$DEVICE/prplmesh.ipk?job=build-for-$DEVICE"
    BUILDINFO_URL="https://gitlab.com/prpl-foundation/prplmesh/-/jobs/artifacts/$BRANCH/raw/build/$DEVICE/prplmesh.buildinfo?job=build-for-$DEVICE"
    # curl options - fail on error (-f), follow redirect (-L)
    curl ${QUIET:+ -s -S} -f -L "$IPK_URL" --output "$IPK_PATH"/prplmesh.ipk
    curl ${QUIET:+ -s -S} -f -L "$BUILDINFO_URL" --output "$IPK_PATH"/prplmesh_buildinfo.txt
}

main() {
    if ! OPTS=$(getopt -o 'hv' --long help,verbose,branch:,device:,path: -n 'parse-options' -- "$@"); then
        echo "Failed parsing options." >&2
        usage
        exit 1
    fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -h|--help)
                usage
                exit 0
                ;;
            -v|--verbose)
                QUIET=
                VERBOSE=true
                shift
                ;;
            --branch)
                BRANCH="$2"
                shift 2
                ;;
            --device)
                DEVICE="$2"
                shift 2
                ;;
            --path)
                IPK_PATH="$2"
                shift 2
                ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    download "$@"
}

DEVICE=netgear-rax40
BRANCH=master
IPK_PATH=.
VERBOSE=false
QUIET=true

main "$@"
