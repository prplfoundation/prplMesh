#!/bin/bash
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################
scriptdir="$(cd "${0%/*}" && pwd)"
rootdir=$(realpath "$scriptdir/../")

# shellcheck source=functions.sh
. "$rootdir/tools/functions.sh"
# shellcheck source=../ci/owncloud/owncloud_definitions.sh
. "$rootdir/ci/owncloud/owncloud_definitions.sh"

usage() {
    echo "usage: $(basename "$0") [-hv]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -v|--verbose - add verbosity"
    echo "      --branch  - branch to use (default master)"
    echo "      --device  - device to use (default netgear-rax40)"
    echo "      --remote  - remote to use - owncloud or gitlab (default owncloud)"
    echo "      --path    - path to copy ipk to (default .)"
    
}

curl_download() {
    # curl options - fail on error (-f), follow redirect (-L)
    curl ${QUIET:+ -s -S} -f -L "$1" --output "$2"
}

main() {
    if ! OPTS=$(getopt -o 'hv' --long help,verbose,branch:,device:,remote:,path: -n 'parse-options' -- "$@"); then
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
            --remote)
                REMOTE="$2"
                shift 2
                ;;
            --path)
                DOWNLOAD_PATH="$2"
                shift 2
                ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    if [ "$REMOTE" = "owncloud" ]; then
        info "Download from owncloud"
        download_artifact latest/build/"$DEVICE"/prplmesh.ipk
        download_artifact latest/build/"$DEVICE"/prplmesh.buildinfo
    elif [ "$REMOTE" = "gitlab" ]; then
        GITLAB_BASE_URL="https://gitlab.com/prpl-foundation/prplmesh/-/jobs/artifacts/"
        # URL for downloading the prplmesh.ipk according to
        # https://docs.gitlab.com/ee/ci/pipelines/job_artifacts.html#downloading-the-latest-artifacts
        IPK_URL="$GITLAB_BASE_URL/$BRANCH/raw/build/$DEVICE/prplmesh.ipk?job=build-for-$DEVICE"
        BUILDINFO_URL="$GITLAB_BASE_URL/$BRANCH/raw/build/$DEVICE/prplmesh.buildinfo?job=build-for-$DEVICE"

        info "Download from gitlab"
        curl_download "$IPK_URL" "$DOWNLOAD_PATH"/prplmesh.ipk
        curl_download "$BUILDINFO_URL" "$DOWNLOAD_PATH"/prplmesh.buildinfo
    else
        err "Invalid remote $REMOTE"
        usage
        exit 1
    fi
}

DEVICE=netgear-rax40
BRANCH=master
DOWNLOAD_PATH=.
QUIET=true
REMOTE=owncloud

main "$@"
