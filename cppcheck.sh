#!/bin/bash

# shellcheck source=../tools/functions.sh
. "$(dirname "${BASH_SOURCE[0]}")/../tools/functions.sh"

docker run --rm -it -w "$ROOT_DIR" -v "$ROOT_DIR:$ROOT_DIR"  prplfoundationinc/prplmesh-builder:alpine-3.11.3 "$ROOT_DIR"/tools/docker/static-analysis/cppcheck.sh -j"$(nproc)" .
