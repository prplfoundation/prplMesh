#!/bin/bash

dbg() {
    [ "$VERBOSE" == "true" ] && echo "$@"
}

err() {
	echo -e '\033[1;31m'"$@"'\033[0m'
}

run() {
    dbg "$*"
    "$@" || exit $?
}

start_function() {
    echo "$0: start"
    if [ `id -u` -ne 0 ]; then
        err "$0: This command must be run as root"
        exit 1
    fi
    run modprobe mac80211_hwsim
    sleep 1
    run $HOSTAPD -P /var/run/hostapd.pid -B $HOSTAPD_CONF
}

stop_function() {
    echo "$0: stop"
    if [ `id -u` -ne 0 ]; then
        err "$0: This script must be run as root"
        exit 1
    fi
    run killall hostapd
    run rmmod mac80211_hwsim
}

status_function() {
    echo "$0: status"
    lsmod | grep mac80211_hwsim
    ps -aux | grep hostapd | grep -v grep
}

usage() {
    echo "usage: $(basename $0) {start|stop|restart|status} [-hv] [-H|--hostapd <hostapd>] [-C|--hostapd-conf <hostapd.conf>]"
}

main() {
    OPTS=`getopt -o 'hvH:C:' --long verbose,help,hostapd:,hostapd-conf:,hostapd-pid: -n 'parse-options' -- "$@"`

    if [ $? != 0 ] ; then err "Failed parsing options." >&2 ; usage; exit 1 ; fi

    #echo "$OPTS"
    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -v | --verbose)      VERBOSE=true; shift ;;
            -h | --help)         usage; exit 0; shift ;;
            -H | --hostapd)      HOSTAPD="$2"; shift; shift ;;
            -C | --hostapd-conf) HOSTAPD_CONF="$2"; shift; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    dbg VERBOSE=$VERBOSE
    dbg HOSTAPD=$HOSTAPD
    dbg HOSTAPD_CONF=$HOSTAPD_CONF

    case $1 in
        "start")
            start_function
            ;;
        "stop")
            stop_function
            ;;
        "restart")
            stop_function
            start_function
            ;;
        "status")
            status_function
            ;;
        *)
            err "unsupported argument \"$1\""; usage; exit 1 ;;
    esac
}

VERBOSE=false
HELP=false
HOSTAPD=`which hostapd 2>/dev/null`
HOSTAPD_CONF=$(realpath ${0%/*}/hostapd.conf)
HOSTAPD_PID=/var/run/hostapd.pid

main $@