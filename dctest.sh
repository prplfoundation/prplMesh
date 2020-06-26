#!/bin/bash

function dc {
    ROOT_DIR=$(dirname $(pwd)) docker-compose -f tools/docker/boardfarm-ci/docker-compose.yml $@
}
if [ $# -eq 0 ]; then
    dc up boardfarm
else
    if [ $1 == "-h" ]; then
        print "Usage ./dctest.sh | ./dctest.sh --clean"
    elif [ $1 == "--clean" ]; then
        dc down --remove-orphans --rmi all
    fi
fi
