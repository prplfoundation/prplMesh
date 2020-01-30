#!/bin/sh -e

usage() {
    echo "usage: $(basename "$0") [-h]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      --certification-mode - enable certification-mode on the target"
}

deploy() {
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
opkg remove prplmesh prplmesh-dwpal prplmesh-nl80211
# currently opkg remove does not remove everything from /opt/prplmesh:
rm -rf /opt/prplmesh
opkg install -V2 "$DEST_FOLDER/$IPK_FILENAME"
EOF

    if [ "$CERTIFICATION_MODE" = true ] ; then
        echo "Certification mode will be enabled on the target"
        ssh "$TARGET" "uci set prplmesh.config.certification_mode=1 && uci commit"
        echo "Certification mode enabled on the target."
    fi
    echo "Done"
}

main() {
        OPTS=$(getopt -o 'h' --long help,certification-mode -n 'parse-options' -- "$@")

    if [ "$?" != 0 ] ; then err "Failed parsing options." >&2 ; usage; exit 1 ; fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            --certification-mode)
                CERTIFICATION_MODE=true
                shift
                ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    deploy "$@"
}

main "$@"
