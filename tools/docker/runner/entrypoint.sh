#!/bin/sh

run() {
    echo "$*"
    "$@" || exit $?
}

run ip link add br-lan type bridge
run ip link add wlan0 type dummy
run ip link add wlan2 type dummy
run ip link add sim-eth0 type dummy
run ip link set sim-eth0 master br-lan
run ip link set eth0 master br-lan
run ip link set wlan0 master br-lan
run ip link set wlan2 master br-lan
run ip address flush dev eth0
run ip link set dev sim-eth0 up
run ip link set dev wlan0 up
run ip link set dev wlan2 up
run ip address add dev br-lan ${1}
run ip link set dev br-lan up

shift

cd ${INSTALL_DIR}
exec /bin/bash "$@"
