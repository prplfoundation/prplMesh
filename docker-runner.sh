#!/bin/sh

scriptdir="$(cd "${0%/*}" || exit 1; pwd)"
"${scriptdir}/tools/docker/run.sh" "$@"
