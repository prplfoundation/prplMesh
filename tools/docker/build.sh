#!/bin/sh

run() {
    echo "$*"
    "$@" || exit $?
}

scriptdir="$(cd "${0%/*}"; pwd)"
topdir="${scriptdir%/*/*/*}"

main() {
    docker image inspect prplmesh-builder >/dev/null 2>&1 || {
        echo "Image prplmesh-build does not exist, creating..."
        run ${scriptdir}/image-build.sh
    }

    run docker container run -v ${topdir}:${topdir} --interactive --tty --rm prplmesh-builder $@
}

main $@
