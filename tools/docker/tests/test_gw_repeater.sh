#!/bin/sh
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

scriptdir="$(cd "${0%/*}"; pwd)"
topdir="${scriptdir%/*/*/*/*}"

. ${topdir}/prplMesh/tools/docker/functions.sh

usage() {
    echo "usage: $(basename $0) [-hv] [-d delay]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -v|--verbose - verbosity on"
    echo "      -d|--delay - delay between starting the containers
    and testing their status (default %DELAY seconds"
    echo "      -f|--force - kill any running containers before starting"
    echo "      -g|--gateway - gateway container name"
    echo "      -r|--repeater - repeater container name"
    echo "      --rm - remove containers after test completes"
    echo "      --gateway-only - start gateway only"
    echo "      --repeater-only - start repeater only"
}

main() {
    OPTS=`getopt -o 'hvd:fg:r:' --long help,verbose,rm,gateway-only,repeater-only,delay:,force,gateway:,repeater: -n 'parse-options' -- "$@"`

    if [ $? != 0 ] ; then err "Failed parsing options." >&2 ; usage; exit 1 ; fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -v | --verbose)       VERBOSE=true; VERBOSE_OPT="-v"; shift ;;
            -h | --help)          usage; exit 0; shift ;;
            -d | --delay)         DELAY="$2"; shift; shift ;;
            -f | --force)         FORCE_OPT="-f"; shift ;;
            -g | --gateway)       GW_NAME="$2"; shift; shift ;;
            -r | --repeater)      REPEATER_NAME="$2"; shift; shift ;;
            --rm)                 REMOVE=true; shift ;;
            --gateway-only)       START_REPEATER=false; shift ;;
            --repeater-only)      START_GATEWAY=false; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    status "Starting GW+Repeater test"

    dbg REMOVE=$REMOVE
    dbg GW_NAME=$GW_NAME
    dbg REPEATER_NAME=$REPEATER_NAME
    dbg START_GATEWAY=$START_GATEWAY
    dbg START_REPEATER=$START_REPEATER
    dbg DELAY=$DELAY

    [ "$START_GATEWAY" = "true" ] && {
        status "Start GW (Controller + local Agent)"
        ${scriptdir}/../run.sh ${VERBOSE_OPT} ${FORCE_OPT} start-controller-agent -d -n ${GW_NAME} -m 00:11:22:33 "$@"
    }

    [ "$START_GATEWAY" = "true" -a "$START_REPEATER" = "true" ] && {
        status "Delay ${DELAY} seconds..."
        sleep ${DELAY}
    }

    [ "$START_REPEATER" = "true" ] && {
        status "Start Repeater (Remote Agent)"
        ${scriptdir}/../run.sh ${VERBOSE_OPT} ${FORCE_OPT} start-agent -d -n ${REPEATER_NAME} -m aa:bb:cc:dd "$@"
    }

    status "Delay ${DELAY} seconds..."
    sleep ${DELAY}

    error=0
    [ "$START_GATEWAY" = "true" ] && report "GW operational" \
        ${scriptdir}/../test.sh ${VERBOSE_OPT} -n ${GW_NAME}
    
    [ "$START_REPEATER" = "true" ] && report "Repeater operational" \
        ${scriptdir}/../test.sh ${VERBOSE_OPT} -n ${REPEATER_NAME}

    [ "$REMOVE" = "true" ] && {
        status "Deleting containers ${GW_NAME}, ${REPEATER_NAME}"
        docker rm -f ${GW_NAME} ${REPEATER_NAME} >/dev/null 2>&1
    }

    return $error
}

VERBOSE=false
REMOVE=false
GW_NAME=gateway
REPEATER_NAME=repeater
START_GATEWAY=true
START_REPEATER=true
DELAY=5

main $@
