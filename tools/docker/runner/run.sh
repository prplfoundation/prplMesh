#!/bin/sh

run() {
    echo "$*"
    "$@" || exit $?
}

scriptdir="$(cd "${0%/*}"; pwd)"
installdir="${scriptdir%/*/*/*/*}/build/install"

main() {
    run docker container run --privileged --network prplMesh-net -v ${installdir}:${installdir} --interactive --tty --rm prplmesh-runner $@
}

main $@
