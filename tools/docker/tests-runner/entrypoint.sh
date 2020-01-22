#!/bin/sh

# start dockerd and wait for it to be available
dockerd-entrypoint.sh >/dev/null 2>&1 &

for _ in $(seq 1 10); do
    docker info -f '.' >/dev/null 2>&1
    status="$?"
    if [ "$status" -eq 0 ] ; then
	break
    fi
    sleep .5
done

if [ "$status" -eq 0 ] ; then
    # run the rest of the scripts:
    "$@"
    exit "$?"
fi

exit 1
