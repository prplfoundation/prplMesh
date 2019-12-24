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
/opt/prplmesh/prplmesh_utils.sh restart -d
TIMEOUT=30
for _ in $(seq 1 "$TIMEOUT") ; do
    if /opt/prplmesh/prplmesh_utils.sh status ; then
        exit 0
    fi
    sleep 1
done
exit 1
EOF

# Save exit status and capture the logs
TEST_STATUS=$?
ssh "$TARGET" "tar -C /tmp/beerocks/logs -czvf - ./" > "${TARGET}_logs.tar.gz"
echo "Logs file:"
ls -l "$PWD/${TARGET}_logs.tar.gz"

echo "Stopping prplMesh"
ssh "$TARGET" /opt/prplmesh/prplmesh_utils.sh stop

exit $TEST_STATUS

