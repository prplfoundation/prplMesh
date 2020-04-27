#!/bin/bash
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

run() {
    echo "$*"
    "$@" || exit $?
}

# Use the ip address that was allocated by the daemon to this
# container (from eth0)
bridge_ip="$(ip addr show dev eth0 | awk '/^ *inet / {print $2}')"

run ip link add          br-lan   type bridge
run ip link add          wlan0    type dummy
run ip link add          wlan2    type dummy
run ip link set      dev eth0     master br-lan
run ip link set      dev wlan0    master br-lan
run ip link set      dev wlan2    master br-lan
run ip address flush dev eth0
run ip link set      dev wlan0    up
run ip link set      dev wlan2    up
run ip address add   dev br-lan "$bridge_ip"
run ip link set      dev br-lan   up

cd "${INSTALL_DIR}" || exit 1
exec /bin/bash "$@"
