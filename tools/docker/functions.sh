###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

installdir="${topdir}/build/install"
sourcesdir="${topdir}/prplMesh"

dbg() {
    [ "$VERBOSE" = "true" ] && echo "$(basename $0): $*"
}

status() {
	printf '\033[1;35m'"$@\n"'\033[0m'
}

mac_to_hex(){
   echo "0x$1" | tr --delete :
}

err() {
    printf '\033[1;31m'"$@\n"'\033[0m'
}

info() {
	printf '\033[1;35m'"$(basename $0): $@"'\033[0m\n'
}

success() {
    printf '\033[1;32m'"$@\n"'\033[0m'
}

run() {
    echo "$*"
    "$@" || exit $?
}

check() {
    if "$@"; then
        dbg "OK $@"
    else
        err "FAIL $@"
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
