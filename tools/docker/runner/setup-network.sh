#!/bin/sh

run() {
    echo "$*"
    "$@" || exit $?
}

run sudo ip link add br-lan type bridge
run sudo ip link add wlan0 type dummy up
run sudo ip link add wlan2 type dummy up
run sudo ip link set eth0 master br-lan
run sudo ip link set wlan0 master br-lan
run sudo ip link set wlan2 master br-lan
run sudo ifconfig eth0 0.0.0.0
run sudo ifconfig br-lan ${1-172.20.0.99} up

exec /bin/bash
