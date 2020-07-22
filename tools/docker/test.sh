#!/bin/bash
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
    echo "usage: $(basename "$0") [-hv] [-n name [-b bridge_mac]]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -v|--verbose - verbosity on"
    echo "      -n|--name - container name for which to attach"
    echo "      -b|--bridge_mac - bridge mac to get operational status on"
}

main() {
    if ! OPTS=$(getopt -o 'hvn:b:' --long verbose,help,name,bridge_mac -n 'parse-options' -- "$@"); then
        err "Failed parsing options." >&2
        usage
        exit 1
    fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -v | --verbose)    VERBOSE=true; OPT="-v"; shift ;;
            -h | --help)       usage; exit 0; shift ;;
            -n | --name)       NAME="$2"; shift; shift ;;
            -b | --bridge_mac) BRIDGE_MAC="$2"; shift; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    if [ -z "$BRIDGE_MAC" ]; then 
        run docker container exec "${NAME}" "${rootdir}/build/install/scripts/prplmesh_utils.sh" status $OPT
    else
        run docker container exec "${NAME}" "${rootdir}/build/install/scripts/prplmesh_utils.sh" status "$BRIDGE_MAC" $OPT
    fi

    
}

VERBOSE=false
NAME=prplMesh
declare BRIDGE_MAC

main "$@"
