#!/bin/sh
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

dbg() {
    [ "$VERBOSE" = "true" ] && echo "$@"
}

err() {
    printf '\033[1;31m'"$@\n"'\033[0m'
}

success() {
    printf '\033[1;32m'"$@\n"'\033[0m'
}

run() {
    dbg "$*"
    "$@" || exit $?
}


usage() {
    echo "usage: $(basename $0) [-hvsmdCD]"
}

main() {
    OPTS=`getopt -o 'hvdb:i:' --long verbose,help,deploy,build-dir:,target-ip:,target-user:,target-pass:,proxy-ip:,proxy-user:,proxy-pass: -n 'parse-options' -- "$@"`

    if [ $? != 0 ] ; then err "Failed parsing options." >&2 ; usage; exit 1 ; fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -v | --verbose)       VERBOSE=true; shift ;;
            -h | --help)          usage; exit 0; shift ;;
            -d | --deploy)        DEPLOY=true; shift ;;
            -b | --build-dir)     BUILD_DIR="$2"; shift; shift ;;
            --proxy-ip)           PROXY_IP="$2"; shift; shift ;;
            --proxy-user)         PROXY_USER="$2"; shift; shift ;;
            --proxy-pass)         PROXY_PASS="$2"; shift; shift ;;
            --target-ip)          TARGET_IP="$2"; shift; shift ;;
            --target-user)        TARGET_USER="$2"; shift; shift ;;
            --target-pass)        TARGET_PASS="$2"; shift; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    echo "Start"
    tmpdir=$(mktemp -d)
    mkdir -p ${tmpdir}/opt/prplmesh
    mkdir -p ${tmpdir}/usr

    cp -r ${BUILD_DIR}/out/* ${tmpdir}/opt/prplmesh
    mv ${tmpdir}/opt/prplmesh/lib ${tmpdir}/usr

    dbg "create prplmesh.tar.gz..."
    tar czvf ${BUILD_DIR}/prplmesh.tar.gz -C ${tmpdir}/ .
    rm -rf ${tmpdir}

    [ "$DEPLOY" = "true" ] && {
        
        if [ -n "$PROXY_IP" -a -n "$PROXY_USER" -a -n "$PROXY_PASS" ]; then
            echo "local md5sum"
            run md5sum ${BUILD_DIR}/prplmesh.tar.gz
            
            echo "scp ${BUILD_DIR}/prplmesh.tar.gz to ${PROXY_USER}@${PROXY_IP}"
            run sshpass -p ${PROXY_PASS} scp ${SSH_OPTIONS} ${BUILD_DIR}/prplmesh.tar.gz ${PROXY_USER}@${PROXY_IP}:/home/${PROXY_USER}/

            echo "scp ${PROXY_USER}@${PROXY_IP}:/home/${PROXY_USER} to ${TARGET_USER}@${TARGET_IP}:/tmp"
            run sshpass -p ${PROXY_PASS} ssh ${SSH_OPTIONS} ${PROXY_USER}@${PROXY_IP} "sshpass -p ${TARGET_PASS} scp ${SSH_OPTIONS} /home/${PROXY_USER}/prplmesh.tar.gz ${TARGET_USER}@${TARGET_IP}:/tmp/"

            echo "remote md5sum"
            run sshpass -p ${PROXY_PASS} ssh ${SSH_OPTIONS} ${PROXY_USER}@${PROXY_IP} "sshpass -p ${TARGET_PASS} ssh ${SSH_OPTIONS} ${TARGET_USER}@${TARGET_IP} md5sum /tmp/prplmesh.tar.gz"

            echo "backaup /opt/prplmesh/beerocks_agent.conf on target"
            run sshpass -p ${PROXY_PASS} ssh ${SSH_OPTIONS} ${PROXY_USER}@${PROXY_IP} "sshpass -p ${TARGET_PASS} ssh ${SSH_OPTIONS} ${TARGET_USER}@${TARGET_IP} cp /opt/prplmesh/config/beerocks_agent.conf /tmp/"

            echo "untar /tmp/prplmesh.tar.gz on target"
            run sshpass -p ${PROXY_PASS} ssh ${SSH_OPTIONS} ${PROXY_USER}@${PROXY_IP} "sshpass -p ${TARGET_PASS} ssh ${SSH_OPTIONS} ${TARGET_USER}@${TARGET_IP} tar -C / -xzvf /tmp/prplmesh.tar.gz"

            echo "restore /opt/prplmesh/beerocks_agent.conf on target"
            run sshpass -p ${PROXY_PASS} ssh ${SSH_OPTIONS} ${PROXY_USER}@${PROXY_IP} "sshpass -p ${TARGET_PASS} ssh ${SSH_OPTIONS} ${TARGET_USER}@${TARGET_IP} cp /tmp/beerocks_agent.conf /opt/prplmesh/config/"
    
        # else
            # TODO COMPLETE BY CORAL

            # sshpass -p ${TARGET_PASS} scp -oUserKnownHostsFile=/dev/null -oStrictHostKeyChecking=no ${BUILD_DIR}/prplmesh.tar.gz ${TARGET_USER}@${TARGET_IP}:/tmp/

            # echo "untar prplmesh in target"
            # ssh -oUserKnownHostsFile=/dev/null -oStrictHostKeyChecking=no root@192.168.1.1 'tar -xzvf /tmp/prplmesh.tar.gz -C /'

            # echo "Deploy prplmesh completed, on target run /etc/init.d/prplmesh.sh restart for changes to take effect."
        fi
    }

    success "Done! Run /etc/init.d/prplmesh.sh restart on target to apply new settings."
}

VERBOSE=false
BUILD_DIR="prplMesh/build"
DEPLOY=false
TARGET_IP=
TARGET_PASS=
TARGET_USER=
PROXY_IP=
PROXY_PASS=
PROXY_USER=
SSH_OPTIONS="-oUserKnownHostsFile=/dev/null -oStrictHostKeyChecking=no"

main $@
