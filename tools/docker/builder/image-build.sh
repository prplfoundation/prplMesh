#!/bin/sh

dbg() {
    [ "$VERBOSE" = "true" ] && echo "$*"
}

err() {
	echo -e '\033[1;31m'"$@"'\033[0m'
}

run() {
    dbg "$*"
    "$@" || exit $?
}

scriptdir="$(cd "${0%/*}"; pwd)"
topdir="${scriptdir%/*/*/*/*}"

usage() {
    echo "usage: $(basename $0) [-hvn]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -v|--verbose - verbosity on"
    echo "      -n|--native - Use the host OS's for the FROM image"
}

main() {
    OPTS=`getopt -o 'hvn' --long verbose,help,native -n 'parse-options' -- "$@"`

    if [ $? != 0 ] ; then err "Failed parsing options." >&2 ; usage; exit 1 ; fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -v | --verbose) VERBOSE=true; shift ;;
            -h | --help)    usage; exit 0; shift ;;
            -n | --native)  NATIVE=true; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    [ "$NATIVE" = "true" ] && {
        . /etc/lsb-release
        IMAGE=$(echo $DISTRIB_ID | awk '{print tolower($0)}'):$DISTRIB_RELEASE
    }
    dbg IMAGE=$IMAGE
    dbg topdir=$topdir

    run docker image build \
        --build-arg image=$IMAGE \
        --build-arg topdir=$topdir \
        --build-arg uid=${SUDO_UID:-0} \
        --build-arg gid=${SUDO_GID:-0} \
        --tag prplmesh-build \
        ${scriptdir}
}

VERBOSE=false
NATIVE=false
IMAGE=ubuntu:18.04

main $@
