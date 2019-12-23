#!/bin/sh

usage() {
    echo "usage: $(basename $0) <target>"
}

if [ -z "$1" ] ; then
    usage
    exit 1
fi

TARGET="$1"

# the first start currently never succeeds, we need to restart it first.
ssh "$TARGET" <<"EOF"
/opt/prplmesh/prplmesh_utils.sh restart
TIMEOUT=30
for _ in $(seq 1 "$TIMEOUT") ; do
    if /opt/prplmesh/prplmesh_utils.sh status ; then
        exit 0
    fi
    sleep 1
done
exit 1
EOF

echo "Stopping prplMesh"
ssh "$TARGET" /opt/prplmesh/prplmesh_utils.sh stop
