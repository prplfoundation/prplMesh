#!/bin/sh
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

scriptdir="$(cd "${0%/*}"; pwd)"

dbg() {
    [ "$VERBOSE" = "true" ] && echo "$*"
}

status() {
	printf '\033[1;35m'"$@\n"'\033[0m'
}

err() {
    printf '\033[1;31m'"$@\n"'\033[0m'
}

success() {
    printf '\033[1;36m'"$@\n"'\033[0m'
}

report() {
    msg="$1"; shift
    if "$@"; then
        success "OK $msg"
    else
        err "FAIL $msg"
        error=1
    fi
}

usage() {
    echo "usage: $(basename $0) [-hv] [-d delay]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -v|--verbose - verbosity on"
    echo "      -d|--delay - delay between starting the containers 
    and testing their status (default %DELAY seconds"
    echo "      -g|--gateway - gateway container name"
    echo "      -r|--repeater - repeater container name"
    echo "      --rm - remove containers after test completes" 
}

main() {
    OPTS=`getopt -o 'hvd:g:r:' --long help,verbose,rm,gateway-only,delay:,gateway:,repeater: -n 'parse-options' -- "$@"`

    if [ $? != 0 ] ; then err "Failed parsing options." >&2 ; usage; exit 1 ; fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -v | --verbose)       VERBOSE=true; OPT="-v"; shift ;;
            -h | --help)          usage; exit 0; shift ;;
            -d | --delay)         DELAY="$2"; shift; shift ;;
            -g | --gateway)       GW_NAME="$2"; shift; shift ;;
            -r | --repeater)      REPEATER_NAME="$2"; shift; shift ;;
            --rm)                 REMOVE=true; shift ;;
            --gateway-only)       GATEWAY_ONLY=true; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    [ -z $GATEWAY_ONLY ] && status "Starting GW+Repeater test" || status "Starting GW only test"

    dbg REMOVE=$REMOVE
    dbg GW_NAME=$GW_NAME
    dbg GATEWAY_ONLY=$GATEWAY_ONLY
    [ -z $GATEWAY_ONLY ] && dbg REPEATER_NAME=$REPEATER_NAME
    dbg DELAY=$DELAY

    status "Start GW (Controller + local Agent)"
    ${scriptdir}/../run.sh start-controller-agent -d -n ${GW_NAME} -m 00:11:22:33
    [ -z $GATEWAY_ONLY ] && {
        sleep ${DELAY}
        status "Start Repeater (Remote Agent)"
        ${scriptdir}/../run.sh start-agent -d -n ${REPEATER_NAME} -m aa:bb:cc:dd
    }
    
    status "Delay ${DELAY} seconds..."
    sleep ${DELAY}
    
    error=0
    report "GW operational" \
        ${scriptdir}/../test.sh ${OPT} -n ${GW_NAME}
    [ -z $GATEWAY_ONLY ] && {
        report "Repeater operational" \
            ${scriptdir}/../test.sh ${OPT} -n ${REPEATER_NAME}
    }
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
DELAY=5

main $@
