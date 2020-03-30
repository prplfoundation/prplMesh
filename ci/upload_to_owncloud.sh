#!/bin/bash
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2020 Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

scriptdir="$(cd "${0%/*}" && pwd)"
rootdir=$(realpath "$scriptdir/../")

# shellcheck source=../tools/functions.sh
. "$rootdir/tools/functions.sh"

usage() {
    echo "usage: $(basename "$0") [-hv] [--overwrite-remote-path <temp-path>] <user> <password> <remote-path> <local-path>"
    echo "  options:"
    echo "      -h|--help - display this help."
    echo "      -v|--verbose - enable verbosity."
    echo "      --overwrite-remote-path - replace any existing folder on remote-path with local-path."
    echo "                      The first argument is the path to a temporary folder to use."
    echo "                      The temporary folder is first emptied."
    echo "      -u|--url - webDAV URL (default https://ftp.essensium.com/owncloud/remote.php/dav/files/USERNAME)"
    echo
    echo "  positional arguments:"
    echo "      remote-path - path in the cloud, relative to the user home to upload"
    echo "      local-path - local file or folder to upload"
    echo ""
    echo "  Credentials are taken from $HOME/.netrc"
    echo "  Make sure it has a line like this:"
    echo "    machine ftp.essensium.com login <user> password <password>"
    echo ""
}

main() {
    if ! OPTS=$(getopt -o 'hvu:' --long help,verbose,overwrite-remote-path:,url: -n 'parse-options' -- "$@"); then
        echo echo "Failed parsing options." >&2; usage; exit 1
    fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -h | --help)            usage; exit 0;;
            -v | --verbose)         VERBOSE=true; QUIET=false; shift;;
            --overwrite-remote-path)          REMOTE_TEMP_PATH="$2"; shift 2;;
            -u | --owncloud-url)    OWNCLOUD_URL="$2"; shift 2;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1;;
        esac
    done

    [ -n "$1" ] || { usage; err "Missing remote-path"; exit 1; }
    remote_path="$1"; shift
    [ -n "$1" ] || { usage; err "Missing local-path"; exit 1; }
    local_path="$1"; shift
    [ -z "${REMOTE_TEMP_PATH-unset}" ] && { usage; err "Temporary remote path is set but empty!"; exit 1; }
    base_folder="$(basename "$local_path")"
    status=0
    OWNCLOUD_BROWSE_URL="https://ftp.essensium.com/owncloud/index.php/apps/files/?dir=$remote_path/$base_folder"

    info "upload $local_path to $OWNCLOUD_BROWSE_URL"
    if [ -n "$REMOTE_TEMP_PATH" ] ; then
        UPLOAD_URL="$OWNCLOUD_URL/$user/$REMOTE_TEMP_PATH"
        info "Using temporary path on remote: \"$UPLOAD_URL\""
        curl ${QUIET:+-s -S} -f -n -X DELETE "$UPLOAD_URL/" > /dev/null 2>&1
        curl ${QUIET:+-s -S} -f -n -X MKCOL "$UPLOAD_URL/"
    else
        UPLOAD_URL="$OWNCLOUD_URL/$user/$remote_path"
    fi

    find "$local_path" -type d -exec \
        realpath {} --relative-to="$(dirname "$local_path")" \; | {
            return=0
            while read -r -s dir; do
                printf . # show progress
                dbg "Create directory: $dir"
                echo "$UPLOAD_URL"
                curl ${QUIET:+-s -S} -f -n -X MKCOL "$UPLOAD_URL/$dir" || {
                        err "Failed to create dir: $remote_path/$dir/"
                        return=1
                }
                printf . # show progress
                # get the list of files to upload in the format <file>,<file>,...,<file>
                files=$(find "$(dirname "$local_path")/$dir/" -type f -maxdepth 1 -print0 | tr '\0' ',' | sed 's/,$//')
                dbg "$files"
                [ -n "$files" ] && {
                    curl ${QUIET:+-s -S} -f -n -T "{$files}" "$UPLOAD_URL/$dir/" || {
                        err "Failed to upload files to $remote_path/$dir/"
                        return=1
                    }
                }
            done
            return $return
        }
    status=$?

    if [ -n "$REMOTE_TEMP_PATH" ] && [ $status = 0 ] ; then
        # We'll move the temporary folder to the target remote_path, which will also
        # empty remote_path if it already exists!
        dbg "Moving \"$REMOTE_TEMP_PATH\" to \"$remote_path\" on the remote"
        curl ${QUIET:+-s -S} -f -n -X MOVE --header "Destination: $OWNCLOUD_URL/$user/$remote_path/" "$UPLOAD_URL" || {
            err "Failed to move the temporary folder to its final location."
            status=1
        }
    fi
    printf '\n'
    info "done"
    return $status
}

OWNCLOUD_URL="https://ftp.essensium.com/owncloud/remote.php/dav/files"
user="${user:=$(cut -f 4 -d ' '  < ~/.netrc)}"

main "$@"
