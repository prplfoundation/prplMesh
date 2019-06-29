#!/bin/sh

scriptdir="$(cd "${0%/*}"; pwd)"
${scriptdir}/tools/docker/builder/build.sh "$@"
