#!/bin/bash
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

scriptdir="$(cd "${0%/*}"; pwd)"
rootdir="${scriptdir%/*}"

. ${rootdir}/tools/functions.sh

usage() {
    echo "usage: $(basename $0) [-hv] [-d delay]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -v|--verbose - verbosity on"
    echo "      -d|--delay - delay between starting the containers
    and testing their status (default %DELAY seconds"
    echo "      -f|--force - kill any running containers before starting"
    echo "      -u|--unique-id - unique id to add as suffix to container and network names"
    echo "      -g|--gateway - gateway container name"
    echo "      -r|--repeater - repeater container name"
    echo "      --rm - remove containers after test completes"
    echo "      --gateway-only - start gateway only"
    echo "      --repeater-only - start repeater only"
}

# This function checks if this script is executed in Microsoft's WSL
# Docker for Windows currently is unable to forward traffic from the
# host to the containers. Since we communicate with the UCC listeners
# on the gateway and the repeaters, expose the UCC listening ports
# from the containers to the host
check_wsl() {
    # Do nothing for non-WSL environments
    if ! grep -q Microsoft /proc/version; then return; fi

    status "Running in WSL"
    
    # Read GW & Repeater UCC ports
    local GW_UCC_PORT
    GW_UCC_PORT=$(grep ucc_listener_port \
    "${rootdir}/build/install/config/beerocks_controller.conf" | \
    awk -F'[= ]' '{ print $2 }')

    local RP_UCC_PORT
    RP_UCC_PORT=$(grep ucc_listener_port \
    "${rootdir}/build/install/config/beerocks_agent.conf" | \
    awk -F'[= ]' '{ print $2 }')

    # In addition to exporting the UCC port on the host, we also
    # need to make sure that the container's IP doesn't change.
    # Docker fails to route traffic from the host to the containers
    # if the IP is anything but what was allocated by the daemon
    # when the container was created
    GW_EXTRA_OPT="--expose ${GW_UCC_PORT} --publish 127.0.0.1::${GW_UCC_PORT} --ipaddr 0.0.0.0"
    RP_EXTRA_OPT="--expose ${RP_UCC_PORT} --publish 127.0.0.1::${RP_UCC_PORT} --ipaddr 0.0.0.0"
}

main() {
    OPTS=`getopt -o 'hvd:fg:r:u:' --long help,verbose,rm,gateway-only,repeater-only,delay:,force,gateway:,repeater:,unique-id: -n 'parse-options' -- "$@"`

    if [ $? != 0 ] ; then err "Failed parsing options." >&2 ; usage; exit 1 ; fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -v | --verbose)       VERBOSE=true; VERBOSE_OPT="-v"; shift ;;
            -h | --help)          usage; exit 0; shift ;;
            -d | --delay)         DELAY="$2"; shift; shift ;;
            -f | --force)         FORCE_OPT="-f"; shift ;;
            -u | --unique-id)     UNIQUE_ID="$2"; shift; shift ;;
            -g | --gateway)       GW_NAME="$2"; shift; shift ;;
            -r | --repeater)      REPEATER_NAMES="$REPEATER_NAMES $2"; shift; shift ;;
            --rm)                 REMOVE=true; shift ;;
            --gateway-only)       START_REPEATER=false; shift ;;
            --repeater-only)      START_GATEWAY=false; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    check_wsl

    status "Starting GW+Repeater test"

    # default values for gateway and repeater[s] names
    REPEATER_NAMES=${REPEATER_NAMES-repeater-${UNIQUE_ID}}
    GW_NAME=${GW_NAME-gateway-${UNIQUE_ID}}

    dbg REMOVE=$REMOVE
    dbg GW_NAME=$GW_NAME
    dbg REPEATER_NAMES=$REPEATER_NAMES
    dbg START_GATEWAY=$START_GATEWAY
    dbg START_REPEATER=$START_REPEATER
    dbg DELAY=$DELAY
    dbg UNIQUE_ID=$UNIQUE_ID

    [ "$START_GATEWAY" = "true" ] && {
        status "Start GW (Controller + local Agent)"
        ${rootdir}/tools/docker/run.sh -u ${UNIQUE_ID} ${VERBOSE_OPT} ${FORCE_OPT} ${GW_EXTRA_OPT} \
        start-controller-agent -d -n ${GW_NAME} -m 00:11:22:33 -- "$@"
    }

    [ "$START_GATEWAY" = "true" -a "$START_REPEATER" = "true" ] && {
        status "Delay ${DELAY} seconds..."
        sleep ${DELAY}
    }

    [ "$START_REPEATER" = "true" ] && {
        index=0
        for repeater in $REPEATER_NAMES; do
            status "Start Repeater (Remote Agent): $repeater"
            ${rootdir}/tools/docker/run.sh -u ${UNIQUE_ID} ${VERBOSE_OPT} ${FORCE_OPT} ${RP_EXTRA_OPT} \
            start-agent -d -n ${repeater} -m aa:bb:cc:$index$index -- "$@"
            index=$((index+1))
        done
    }

    status "Delay ${DELAY} seconds..."
    sleep ${DELAY}

    error=0
    [ "$START_GATEWAY" = "true" ] && report "GW operational" \
        ${rootdir}/tools/docker/test.sh ${VERBOSE_OPT} -n ${GW_NAME}


    [ "$START_REPEATER" = "true" ] && {
        for repeater in $REPEATER_NAMES
        do
            report "Repeater $repeater operational" \
            ${rootdir}/tools/docker/test.sh ${VERBOSE_OPT} -n ${repeater}
        done
    }

    [ "$REMOVE" = "true" ] && {
        status "Deleting containers ${GW_NAME} ${REPEATER_NAMES}"
        docker rm -f ${GW_NAME} ${REPEATER_NAMES} >/dev/null 2>&1
    }

    return $error
}

VERBOSE=false
REMOVE=false
START_GATEWAY=true
START_REPEATER=true
UNIQUE_ID=${SUDO_USER:-${USER}}
DELAY=5

main $@
