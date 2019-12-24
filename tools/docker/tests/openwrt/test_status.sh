#!/bin/sh

usage() {
    echo "usage: $(basename $0) <target>"
}

if [ -z "$1" ] ; then
    usage
    exit 1
fi

TARGET="$1"

# Collect diagnostic data before the restart
echo "Collecting pre-test-run diagnostic data..."
ssh "$TARGET" <<"EOF" > "${TARGET}_diags.log"
date
echo "Release:"
cat /etc/*_release
echo "Version:"
cat /etc/*_version
echo
echo "UCI Configuration (prplMesh, network and wireless):"
uci export prplmesh
uci export network
uci export wireless
echo
echo "ifconfig output:"
ifconfig
echo "'iw list' output:"
iw list
EOF

# The first start currently never succeeds, we need to restart it first.
echo "Attempting to start/restart prplMesh ..."
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
# Save exit status
TEST_STATUS=$?

# Collect post-run diagnostics
echo "Collecting post-test-run diagnostic data..."
ssh "$TARGET" <<"EOF" >> "${TARGET}_diags.log"
date
echo "Output of bml_conn_map:"
/opt/prplmesh/bin/beerocks_cli -c bml_conn_map 2>&1
echo "Output of bml_nw_map_query:"
/opt/prplmesh/bin/beerocks_cli -c bml_nw_map_query 2>&1
echo "Kernel messages:"
dmesg
EOF

# Capture the logs
echo "Capturing the prplMesh logs..."
ssh "$TARGET" "tar -C /tmp/beerocks/logs -czvf - ./" > "${TARGET}_logs.tar.gz"

echo "Logs file:"
ls -l "$PWD/${TARGET}_logs.tar.gz"

echo "Stopping prplMesh"
ssh "$TARGET" /opt/prplmesh/prplmesh_utils.sh stop

exit $TEST_STATUS
