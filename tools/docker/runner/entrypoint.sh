#!/bin/bash
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2019 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

run() {
    echo "$*"
    "$@" || exit $?
}

bridge_ip="$1"; shift
base_mac="$1"; shift

# If the requested bridge ip is 0.0.0.0, use the ip address that
# was allocated by the daemon to this container (from eth0)
if [[ ${bridge_ip} == 0.0.0.0* ]]; then
    bridge_ip="$(ip addr show dev eth0 | grep inet | awk '{print $2}')"
fi

run ip link add          br-lan   address "${base_mac}:00:00" type bridge
run ip link add          wlan0    address "${base_mac}:00:10" type dummy
run ip link add          wlan2    address "${base_mac}:00:20" type dummy
run ip link set      dev eth0     master br-lan
run ip link set      dev wlan0    master br-lan
run ip link set      dev wlan2    master br-lan
run ip address flush dev eth0
run ip link set      dev wlan0    up
run ip link set      dev wlan2    up
run ip address add   dev br-lan "$bridge_ip"
run ip link set      dev br-lan   up

cd ${INSTALL_DIR}
exec /bin/bash "$@"
