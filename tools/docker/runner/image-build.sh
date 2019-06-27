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
        --tag prplmesh-runner \
        ${scriptdir}
}

main $@
