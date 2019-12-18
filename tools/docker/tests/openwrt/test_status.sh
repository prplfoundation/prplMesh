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
sleep 10
/opt/prplmesh/prplmesh_utils.sh status
EOF

