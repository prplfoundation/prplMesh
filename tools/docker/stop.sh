#!/bin/sh
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 Raphaël Mélotte
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

#
# Stop, kill or remove the containers that have been started by the
# test scripts.
#

scriptdir="$(cd "${0%/*}"; pwd)"

usage() {
    echo "usage: $(basename "$0") [-hkr]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -k|--kill - kill the containers instead of stopping them"
    echo "      -r|--remove - remove the container after it has been stopped"
}

main() {
    local stop_cmd remove containers_file
    containers_file="${scriptdir}/.test_containers"
    stop_cmd=stop
    OPTS=$(getopt -o 'hkr' --long help,kill,remove -n 'parse-options' -- "$@")

    if [ ! -f "$containers_file" ] ; then
        echo "\"$containers_file\" does not exist, no container to stop."
        exit 0
    fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -h | --help)    usage; exit 0; shift ;;
            -k | --kill)    stop_cmd="kill"; shift;;
            -r | --remove)    remove=true; shift;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    while read -r container; do
        docker "$stop_cmd" "${container}"
        if [ "$remove" = true ] ; then
            docker rm "${container}"
        fi
    done <"$containers_file"

}

main "$@"
