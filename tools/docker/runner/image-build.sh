#!/bin/sh

run() {
    echo "$*"
    "$@" || exit $?
}

scriptdir="$(cd "${0%/*}"; pwd)"
installdir="${scriptdir%/*/*/*/*}/build/install"

main() {
    run docker image build \
        --build-arg installdir=$installdir \
        --build-arg uid=${SUDO_UID:-0} \
        --build-arg gid=${SUDO_GID:-0} \
        --tag prplmesh-runner \
        ${scriptdir}
}

main $@
