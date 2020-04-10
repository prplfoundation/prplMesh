#!/bin/sh
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

scriptdir="$(cd "${0%/*}" && pwd)"
rootdir="${scriptdir%/*/*}"

dbg() {
    [ "$VERBOSE" = "true" ] && echo "$@"
}

err() {
    printf "\033[1;31m%s\n\033[0m" "$@"
}

success() {
    printf "\033[1;32m%s\n\033[0m" "$@"
}

run() {
    dbg "$*"
    "$@" || exit $?
}


usage() {
    echo "usage: $(basename "$0") [-hvdb] [--target <user@target>] [--proxy <user@proxy>]"
}

main() {

    if ! OPTS=$(getopt -o 'hvdb:' --long help,verbose,deploy,build-dir:,target:,proxy: -n 'parse-options' -- "$@"); then
        err "Failed parsing options." >&2
        usage
        exit 1 
    fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -v | --verbose)       VERBOSE=true; shift ;;
            -h | --help)          usage; exit 0 ;;
            -d | --deploy)        DEPLOY=true; shift ;;
            -b | --build-dir)     BUILD_DIR="$2"; shift; shift ;;
            --proxy)              SSH_OPTIONS="$SSH_OPTIONS \"-oProxyJump $2\""; shift 2 ;;
            --target)             TARGET="$2"; shift 2 ;;
            --keep-conf)          KEEP_CONF=true; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    echo "Start"
    tmpdir=$(mktemp -d)
    mkdir -p "${tmpdir}"/opt/prplmesh
    mkdir -p "${tmpdir}"/usr

    cp -r "${BUILD_DIR}"/install/bin "${tmpdir}"/opt/prplmesh
    cp -r "${BUILD_DIR}"/install/config "${tmpdir}"/opt/prplmesh
    cp -r "${BUILD_DIR}"/install/lib "${tmpdir}"/usr/
    cp -r "${BUILD_DIR}"/install/scripts "${tmpdir}"/opt/prplmesh
    cp -r "${BUILD_DIR}"/install/share "${tmpdir}"/opt/prplmesh

    dbg "create prplmesh.tar.gz..."
    tar czvf "${BUILD_DIR}"/prplmesh.tar.gz -C "${tmpdir}"/ .
    rm -rf "${tmpdir}"

    [ "$DEPLOY" = "true" ] && {

        echo "local md5sum"
        run md5sum "${BUILD_DIR}"/prplmesh.tar.gz

        echo "scp ${BUILD_DIR}/prplmesh.tar.gz to ${TARGET}"
        eval scp "${SSH_OPTIONS} ${BUILD_DIR}/prplmesh.tar.gz ${TARGET}:/tmp/"

        if [ "$KEEP_CONF" = "true" ]; then
            echo "backaup /opt/prplmesh/beerocks_agent.conf on target"
            eval ssh "${SSH_OPTIONS} ${TARGET} cp /opt/prplmesh/config/beerocks_agent.conf /tmp/"
        fi

        echo "untar /tmp/prplmesh.tar.gz on target"
        eval ssh "${SSH_OPTIONS} ${TARGET} tar -C / -xzvf /tmp/prplmesh.tar.gz"

        if [ "$KEEP_CONF" = "true" ]; then
            echo "restore /opt/prplmesh/beerocks_agent.conf on target"
            eval ssh "${SSH_OPTIONS} ${TARGET} mv /tmp/beerocks_agent.conf /opt/prplmesh/config/"
        fi
    
    }

    success "Done! Run /etc/init.d/prplmesh.sh restart on target to apply new settings."
}

VERBOSE=false
BUILD_DIR=${rootdir}/build
DEPLOY=false
KEEP_CONF=false
TARGET=
SSH_OPTIONS="\"-oBatchMode yes\" \"-oStrictHostKeyChecking no\""

main "$@"
