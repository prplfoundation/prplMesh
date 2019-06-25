#!/bin/sh

run() {
    echo "$*"
    "$@" || exit $?
}

scriptdir="$(cd "${0%/*}"; pwd)"
topdir="${scriptdir%/*/*/*/*}"

main() {
    run docker image build \
        --build-arg topdir=$topdir \
        --build-arg uid=${SUDO_UID:-0} \
        --build-arg gid=${SUDO_GID:-0} \
        --tag prplmesh-build \
        ${scriptdir}
}

main $@
