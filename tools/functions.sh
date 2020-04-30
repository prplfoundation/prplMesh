#!/bin/bash
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

ROOT_DIR="$(realpath "$(dirname "${BASH_SOURCE[0]}")"/..)"
export ROOT_DIR

dbg() {
    if [ "$VERBOSE" = "true" ]; then echo "$(basename "$0"): $*"; fi
}

status() {
    printf '\033[1;35m%s\033[0m\n' "$*"
}

mac_to_hex(){
   echo "0x$1" | tr --delete :
}

err() {
    printf '\033[1;31m%s\033[0m\n' "$*"
}

info() {
    printf '\033[1;35m%s: %s\033[0m\n' "$(basename "$0")" "$*"
}

success() {
    printf '\033[1;32m%s\033[0m\n' "$*"
}

run() {
    echo "$*"
    "$@" || exit $?
}

check() {
    if "$@"; then
        dbg "OK $*"
    else
        err "FAIL $*"
        check_error=$((check_error+1))
    fi
}

report() {
    msg="$1"; shift
    if "$@"; then
        success "OK $msg"
    else
        err "FAIL $msg"
        error=$((error+1))
    fi
}
