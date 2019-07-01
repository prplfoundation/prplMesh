#!/bin/sh

run() {
    echo "$*"
    "$@" || exit $?
}

scriptdir="$(cd "${0%/*}"; pwd)"
topdir="${scriptdir%/*/*/*/*}"

main() {
    run docker container run -u root -v ${topdir}:${topdir} --interactive --tty --rm prplmesh-build $@
}

main $@
