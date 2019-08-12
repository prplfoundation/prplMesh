#!/bin/sh
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

scriptdir="$(cd "${0%/*}"; pwd)"
topdir="${scriptdir%/*/*/*}"

. ${topdir}/prplMesh/tools/docker/functions.sh

usage() {
    echo "usage: $(basename $0) [-hv] [-n name]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -v|--verbose - verbosity on"
    echo "      -n|--name - container name for which to attach"
}

main() {
    OPTS=`getopt -o 'hvn:' --long verbose,help,name -n 'parse-options' -- "$@"`

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -v | --verbose) VERBOSE=true; OPT="-v"; shift ;;
            -h | --help)    usage; exit 0; shift ;;
            -n | --name)    NAME="$2"; shift; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    run docker container exec -it ${NAME} ${topdir}/build/install/scripts/prplmesh_utils.sh status $OPT
}

VERBOSE=false
NAME=prplMesh
main $@
