#!/bin/sh

dbg() {
    [ "$VERBOSE" = "true" ] && echo "$(basename $0): $*"
}

err() {
	echo '\033[1;31m'"$(basename $0): $@"'\033[0m'
}

info() {
	echo '\033[1;35m'"$(basename $0): $@"'\033[0m'
}

run() {
    dbg "$*"
    "$@" || exit $?
}

scriptdir="$(cd "${0%/*}"; pwd)"
topdir="${scriptdir%/*/*/*}"

usage() {
    echo "usage: $(basename $0) [-hvn] <type>"
    echo "  mandatory:"
    echo "      type - image type <runner/builder>"
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

     "$NATIVE" = "true" ] && IMAGE=$(
        . /etc/os-release
        echo "$(echo $NAME | awk '{print tolower($0)}'):$VERSION_ID"
    )

    dbg IMAGE=$IMAGE
    dbg topdir=$topdir

    info "Base docker image $IMAGE"
    info "Generating prplmesh-builder docker image"
    run docker image build \
        --build-arg image=$IMAGE \
        --build-arg topdir=$topdir \
        --build-arg uid=${SUDO_UID:-0} \
        --build-arg gid=${SUDO_GID:-0} \
        --tag prplmesh-build \
        ${scriptdir}/builder

    info "Generating prplMesh-runner docker image"
    run docker image build \
        --build-arg image=$IMAGE \
        --tag prplmesh-runner \
        ${scriptdir}/runner
}

VERBOSE=false
NATIVE=false
IMAGE=ubuntu:18.04

main $@
