#!/bin/bash

scriptdir="$(cd "${0%/*}" && pwd)"
rootdir="${scriptdir}"

docker run --rm -it -w "$rootdir" -v "$rootdir:$rootdir"  prplfoundationinc/prplmesh-builder:alpine-3.11.3 "$rootdir"/tools/docker/static-analysis/cppcheck.sh -j"$(nproc)" .
