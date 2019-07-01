#!/bin/sh

dbg() {
    [ "$VERBOSE" = "true" ] && echo "$*"
}

err() {
	echo -e '\033[1;31m'"$@"'\033[0m'
}

run() {
    dbg "$*"
    "$@" || exit $?
}

scriptdir="$(cd "${0%/*}"; pwd)"
installdir="${scriptdir%/*/*/*/*}/build/install"

usage() {
    echo "usage: $(basename $0) [-hvd] [-i ip] [-n name] [-N network]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -v|--verbose - verbosity on"
    echo "      -d|--detach - run in background"
    echo "      -i|--ipaddr - ipaddr for container br-lan (should be in network subnet)"
    echo "      -n|--name - container name (for later easy attach)"
    echo "      -N|--network - docker network to which to attach the container"
}

iprand(){
    ipaddr=$1
    rand=`shuf -i 2-254 -n 1`
    ip_hex=$(printf '%.2X%.2X%.2X%.2X\n' `echo $ipaddr | sed -e 's/\./ /g'`)
    ip_hex_rand=$(printf %.8X `echo $(( 0x$ip_hex + $rand ))`)
    ip_rand=$(printf '%d.%d.%d.%d\n' `echo $ip_hex_rand | sed -r 's/(..)/0x\1 /g'`)
    echo "$ip_rand"
}

generate_container_random_ip() {
    gateway_ip=$(docker network inspect $1 | jq -r '.[0].IPAM.Config'[0].Gateway)
    echo $(iprand ${gateway_ip})
}

main() {
    OPTS=`getopt -o 'hvdi:n:N:o:' --long verbose,help,detach,ipaddr:,name:,network:,options: -n 'parse-options' -- "$@"`

    if [ $? != 0 ] ; then err "Failed parsing options." >&2 ; usage; exit 1 ; fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -v | --verbose) VERBOSE=true; shift ;;
            -h | --help)    usage; exit 0; shift ;;
            -d | --detach)  DETACH=true; shift ;;
            -i | --ipaddr)  IPADDR="$2"; shift; shift ;;
            -n | --name)    NAME="$2"; shift; shift ;;
            -N | --network) NETWORK="$2"; shift; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    docker image inspect prplmesh-runner >/dev/null 2>&1 || {
        dbg "Image prplmesh-runner does not exist, creating..."
        run ${scriptdir}/image-build.sh
    }

    docker network inspect ${NETWORK} >/dev/null 2>&1 || {
        dbg "Network $NETWORK does not exist, creating..."
        run docker network create ${NETWORK} >/dev/null 2>&1
    }

    [ -z "$IPADDR" -a -n "$NETWORK" ] && {
        dbg "Generate random IP for container $NAME for network $NETWORK"
        IPADDR=$(generate_container_random_ip $NETWORK)
    }
    dbg "VERBOSE=${VERBOSE}"
    dbg "DETACH=${DETACH}"
    dbg "NETWORK=${NETWORK}"
    dbg "IPADDR=${IPADDR}"
    dbg "NAME=${NAME}"

    DOCKEROPTS="-e USER=${SUDO_USER}
                --privileged
                --network ${NETWORK}
                -v ${installdir}:${installdir}
                --name ${NAME}"

    if [ "$DETACH" = "false" ]; then
        DOCKEROPTS="$DOCKEROPTS --interactive --tty --rm"
    else
        DOCKEROPTS="$DOCKEROPTS -d"
    fi
    
    run docker container run ${DOCKEROPTS} prplmesh-runner $IPADDR "$@"
}

VERBOSE=false
DETACH=false
NETWORK=prplMesh-net
IPADDR=
NAME=prplMesh

main $@
