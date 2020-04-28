#!/bin/sh

scriptdir=$(dirname "$(readlink -f "$0")")
rootdir=$(realpath "$scriptdir/../..")

usage() {
    echo "usage: $(basename "$0") <target>"
}

if [ -z "$1" ] ; then
    usage
    exit 1
fi

TARGET="$1"
LOG_DIR="$rootdir/logs/${TARGET}"

mkdir -p "$LOG_DIR"

# Collect diagnostic data before the restart
echo "Collecting pre-test-run diagnostic data..."
ssh "$TARGET" <<"EOF" > "$LOG_DIR/${TARGET}_diags.log"
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
# Some devices still use the old path (outside of "scripts")
ln -s /opt/prplmesh/scripts/prplmesh_utils.sh /opt/prplmesh/prplmesh_utils.sh || true
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
ssh "$TARGET" <<"EOF" > "$LOG_DIR/conn_map.txt"
date
echo "Output of bml_conn_map:"
/opt/prplmesh/bin/beerocks_cli -c bml_conn_map 2>&1
echo "Output of bml_nw_map_query:"
/opt/prplmesh/bin/beerocks_cli -c bml_nw_map_query 2>&1
EOF

ssh "$TARGET" "dmesg" > "$LOG_DIR/dmesg.txt"

# Capture the logs
echo "Capturing the prplMesh logs..."
scp -r "$TARGET:/tmp/beerocks/logs/*" "$LOG_DIR"

scp "$TARGET:/var/run/hostapd-phy*.conf" "$LOG_DIR"

echo "Stopping prplMesh"
ssh "$TARGET" /opt/prplmesh/prplmesh_utils.sh stop

exit $TEST_STATUS
