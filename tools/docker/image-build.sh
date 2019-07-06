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
    echo "usage: $(basename $0) [-hvbt]"
    echo "  mandatory:"
    echo "      type - image type <runner/builder>"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -v|--verbose - verbosity on"
    echo "      -b|--base-image - Base OS image to use (Dockerfile 'FROM')"
    echo "      -t|--tag - tag to add to prplmesh-builder and prplmesh-runner images"
}

main() {
    OPTS=`getopt -o 'hvb:t:' --long verbose,help,base-image,tag -n 'parse-options' -- "$@"`

    if [ $? != 0 ] ; then err "Failed parsing options." >&2 ; usage; exit 1 ; fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -v | --verbose)         VERBOSE=true; shift ;;
            -h | --help)            usage; exit 0; shift ;;
            -b | --base-image)      IMAGE="$2"; shift ; shift ;;
            -t | --tag)             TAG=":$2"; shift ; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    [ -z "$IMAGE" ] && IMAGE=$(
        . /etc/os-release
        echo "$(echo $NAME | awk '{print tolower($0)}'):$VERSION_ID"
    )

    dbg IMAGE=$IMAGE
    dbg TAG=$TAG
    dbg topdir=$topdir

    info "Base docker image $IMAGE"
    info "Generating builder docker image (prplmesh-builder$TAG)"
    run docker image build \
        --build-arg image=$IMAGE \
        --build-arg topdir=$topdir \
        --build-arg uid=${SUDO_UID:-0} \
        --build-arg gid=${SUDO_GID:-0} \
        --tag prplmesh-builder$TAG \
        ${scriptdir}/builder

    info "Generating runner docker image (prplmesh-runner$TAG)"
    run docker image build \
        --build-arg image=$IMAGE \
        --tag prplmesh-runner$TAG \
        ${scriptdir}/runner
}

VERBOSE=false
NATIVE=false
IMAGE=

main $@
