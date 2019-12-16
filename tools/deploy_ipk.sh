#!/bin/sh -e

SSH_OPTIONS="-oBatchMode=yes"

if ! ssh "$SSH_OPTIONS" "$1" /bin/true ; then
    echo "Error: $1 unreachable via ssh"
    exit 1
fi

TARGET="$1"
IPK="$2"
IPK_FILENAME="$(basename "$2")"
DEST_FOLDER=/tmp/prplmesh_ipks

echo "Removing previous ipks"
ssh "$TARGET" "rm -rf \"$DEST_FOLDER\" ; mkdir -p \"$DEST_FOLDER\""
echo "Copying $IPK to $TARGET:$DEST_FOLDER/$IPK_FILENAME"
scp "$IPK" "$TARGET:$DEST_FOLDER/$IPK_FILENAME"

ssh "$TARGET" <<EOF
# we don't want opkg to stay locked with a previous failed invocation.
# when using this, make sure no one is using opkg in the meantime! 
pgrep opkg | xargs kill -SIGINT
# remove any previously installed prplmesh:
opkg remove prplmesh
# currently opkg remove does not remove everything from /opt/prplmesh:
rm -rf /opt/prplmesh
opkg install -V2 "$DEST_FOLDER/$IPK_FILENAME"
EOF

echo "Done"
