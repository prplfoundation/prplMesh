#!/bin/sh

scriptdir="$(cd "${0%/*}"; pwd)"
${scriptdir}/tools/docker/build.sh "$@"
