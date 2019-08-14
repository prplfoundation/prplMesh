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

success() {
	printf '\033[1;35m'"$@\n"'\033[0m'
}

err() {
    printf '\033[1;31m'"$@\n"'\033[0m'
}

info() {
	printf '\033[1;35m'"$(basename $0): $@"'\033[0m\n'
}

status() {
    printf '\033[1;36m'"$@\n"'\033[0m'
}

run() {
    echo "$*"
    "$@" || exit $?
}

report() {
    msg="$1"; shift
    if "$@"; then
        success "OK $msg"
    else
        err "FAIL $msg"
        error=1
    fi
}
