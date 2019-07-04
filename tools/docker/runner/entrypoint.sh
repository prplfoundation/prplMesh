#!/bin/sh

run() {
    echo "$*"
    "$@" || exit $?
}

run ip link add br-lan type bridge
run ip link add wlan0 type dummy up
run ip link add wlan2 type dummy up
run ip link add sim-eth0 type dummy up
run ip link set sim-eth0 master br-lan
run ip link set eth0 master br-lan
run ip link set wlan0 master br-lan
run ip link set wlan2 master br-lan
run ifconfig eth0 0.0.0.0
run ifconfig sim-eth0 up
run ifconfig wlan0 up
run ifconfig wlan2 up
run ifconfig br-lan ${1} up

shift

cd ${INSTALL_DIR}
exec /bin/bash "$@"
