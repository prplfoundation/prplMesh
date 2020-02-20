#!/bin/sh -e
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 Raphaël Mélotte
# Copyright (c) 2020 Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

usage() {
    echo "usage: $(basename "$0") <user>@<target> <ipk> [-h]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      --certification-mode - enable certification-mode on the target"
    echo "      --proxy - use ssh proxy <user>@<proxy>"
}

deploy() {
    if ! eval ssh "$SSH_OPTIONS" "$1" /bin/true ; then
        echo "Error: $1 unreachable via ssh"
        exit 1
    fi

    TARGET="$1"
    IPK="$2"
    IPK_FILENAME="$(basename "$2")"
    DEST_FOLDER=/tmp/prplmesh_ipks

    echo "Removing previous ipks"
    eval ssh "$SSH_OPTIONS" "$TARGET" \""rm -rf \"$DEST_FOLDER\" ; mkdir -p \"$DEST_FOLDER\"\""
    echo "Copying $IPK to $TARGET:$DEST_FOLDER/$IPK_FILENAME"
    eval scp "$SSH_OPTIONS" "$IPK" "$TARGET:$DEST_FOLDER/$IPK_FILENAME"

    eval ssh "$SSH_OPTIONS" "$TARGET" <<EOF
# we don't want opkg to stay locked with a previous failed invocation.
# when using this, make sure no one is using opkg in the meantime!
pgrep opkg | xargs kill -SIGINT
# remove any previously installed prplmesh. Use force-depends in case
# there are packages depending on prplmesh which will cause opkg remove
# to fail without this:
opkg remove --force-depends prplmesh prplmesh-dwpal prplmesh-nl80211
# currently opkg remove does not remove everything from /opt/prplmesh:
rm -rf /opt/prplmesh
opkg install -V2 "$DEST_FOLDER/$IPK_FILENAME"
EOF

    if [ "$CERTIFICATION_MODE" = true ] ; then
        echo "Certification mode will be enabled on the target"
        eval ssh "$SSH_OPTIONS" "$TARGET" \""uci set prplmesh.config.certification_mode=1 && uci commit"\"
        echo "Certification mode enabled on the target."
    fi
    echo "Done"
}

main() {
    if ! OPTS=$(getopt -o 'h' --long help,certification-mode,proxy: -n 'parse-options' -- "$@"); then
        echo "Failed parsing options." >&2
        usage
        exit 1
    fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            --certification-mode)
                CERTIFICATION_MODE=true
                shift
                ;;
            --proxy)
                SSH_OPTIONS="$SSH_OPTIONS \"-oProxyJump \"$2\"\""
                shift 2
                ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    deploy "$@"
}

SSH_OPTIONS="\"-oBatchMode yes\" \"-oStrictHostKeyChecking no\""

main "$@"
