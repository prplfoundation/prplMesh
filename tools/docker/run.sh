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
    echo "usage: $(basename $0) [-hvd] [-i ip] [-n name] [-N network]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -v|--verbose - verbosity on"
    echo "      -d|--detach - run in background"
    echo "      -f|--force - if the container is already running, kill it and restart"
    echo "      -i|--ipaddr - ipaddr for container br-lan (should be in network subnet)"
    echo "      -m|--mac - base MAC address for interfaces in the container"
    echo "      -n|--name - container name (for later easy attach)"
    echo "      -p|--port - port to expose on the container"
    echo "      -I|--image - docker network to which to attach the container"
    echo "      -N|--network - docker network to which to attach the container"
    echo "      --entrypoint - use a different entrypoint for the container"
}

iprand(){
    ipaddr=$1
    rand=`shuf -i 2-254 -n 1`
    ip_hex=$(printf '%.2X%.2X%.2X%.2X\n' `echo $ipaddr | sed -e 's/\./ /g'`)
    ip_hex_rand=$(printf %.8X `echo $(( 0x$ip_hex + $rand ))`)
    ip_rand=$(printf '%d.%d.%d.%d\n' `echo $ip_hex_rand | sed -r 's/(..)/0x\1 /g'`)
    echo "$ip_rand"
}

generate_container_random_ip() {
    gateway_ip=$(docker network inspect $1 | jq -r '.[0].IPAM.Config'[0].Gateway)
    echo $(iprand ${gateway_ip})
}

main() {
    OPTS=`getopt -o 'hvdfi:m:n:N:o:t:p:' --long verbose,help,detach,force,ipaddr:,mac:,name:,network:,entrypoint:,tag:,port:,options: -n 'parse-options' -- "$@"`

    if [ $? != 0 ] ; then err "Failed parsing options." >&2 ; usage; exit 1 ; fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -v | --verbose)     VERBOSE=true; shift ;;
            -h | --help)        usage; exit 0; shift ;;
            -d | --detach)      DETACH=true; shift ;;
            -f | --force)       FORCE=true; shift ;;
            -i | --ipaddr)      IPADDR="$2"; shift; shift ;;
            -m | --mac)         BASE_MAC="$2"; shift; shift ;;
            -n | --name)        NAME="$2"; shift; shift ;;
            -t | --tag)         TAG=":$2"; shift; shift ;;
            -p | --port)        PORT=":$2"; shift; shift ;;
            -N | --network)     NETWORK="$2"; shift; shift ;;
            --entrypoint)       ENTRYPOINT="$2"; shift; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    docker image inspect prplmesh-runner$TAG >/dev/null 2>&1 || {
        [ -n "$TAG" ] && { err "image prplmesh-runner$TAG doesn't exist, aborting"; exit 1; }
        dbg "Image prplmesh-runner$TAG does not exist, creating..."
        run ${scriptdir}/image-build.sh
    }

    docker network inspect ${NETWORK} >/dev/null 2>&1 || {
        dbg "Network $NETWORK does not exist, creating..."
        run docker network create ${NETWORK} >/dev/null 2>&1
    }

    [ -z "$IPADDR" -a -n "$NETWORK" ] && {
        dbg "Generate random IP for container $NAME for network $NETWORK"
        IPADDR=$(generate_container_random_ip $NETWORK)
    }
    dbg "VERBOSE=${VERBOSE}"
    dbg "DETACH=${DETACH}"
    dbg "NETWORK=${NETWORK}"
    dbg "IMAGE=prplmesh-runner$TAG"
    dbg "IPADDR=${IPADDR}"
    dbg "BASE_MAC=${BASE_MAC}"
    dbg "PORT=${PORT}"
    dbg "NAME=${NAME}"

    DOCKEROPTS="-e USER=${SUDO_USER}
                -e INSTALL_DIR=${installdir}
                --privileged
                --network ${NETWORK}
                --expose ${PORT}
                -v ${installdir}:${installdir}
                -v ${sourcesdir}:${sourcesdir}
                --name ${NAME}"

    [ -n "$ENTRYPOINT" ] && DOCKEROPTS="$DOCKEROPTS --entrypoint $ENTRYPOINT"
    if [ "$DETACH" = "false" ]; then
        DOCKEROPTS="$DOCKEROPTS --interactive --tty --rm"
    else
        DOCKEROPTS="$DOCKEROPTS -d"
    fi

    if [ -n "$(docker ps -q -l -f name="${NAME}")" ]; then
        info "Container ${NAME} is already running"
        if [ "$FORCE" = "true" ]; then
            run docker container rm -f "$NAME"
        else
            exit 1
        fi
    fi
    
    run docker container run ${DOCKEROPTS} prplmesh-runner$TAG $IPADDR "$BASE_MAC" "$@"
}

VERBOSE=false
DETACH=false
FORCE=false
NETWORK=prplMesh-net
IPADDR=
NAME=prplMesh
ENTRYPOINT=
PORT=5000
BASE_MAC=44:55:66:77

main $@
