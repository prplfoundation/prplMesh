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
    echo "usage: $(basename "$0") [-hv] <user> <password> <remote-path> <local-path>"
    echo "  options:"
    echo "      -h|--help - display this help."
    echo "      -v|--verbose - enable verbosity"
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
    if ! OPTS=$(getopt -o 'hvu:' --long help,verbose,url: -n 'parse-options' -- "$@"); then
        echo echo "Failed parsing options." >&2; usage; exit 1
    fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -h | --help)            usage; exit 0;;
            -v | --verbose)         export VERBOSE=true; shift;;
            -u | --owncloud-url)    OWNCLOUD_URL="$2"; shift 2;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1;;
        esac
    done

    [ -n "$1" ] || { usage; err "Missing remote-path"; exit 1; }
    remote_path="$1"; shift
    [ -n "$1" ] || { usage; err "Missing local-path"; exit 1; }
    local_path="$1"; shift
    status=0
    OWNCLOUD_BROWSE_URL="https://ftp.essensium.com/owncloud/index.php/apps/files/?dir=$remote_path/$(basename "$local_path")"

    info "upload $local_path to $OWNCLOUD_BROWSE_URL"
    find "$local_path" -type d -exec \
        realpath {} --relative-to="$(dirname "$local_path")" \; | {
            return=0
            while read -r -s dir; do
                printf . # show progress
                dbg "Create directory: $dir"
                echo "$OWNCLOUD_URL/$user/$remote_path/$dir"
                curl -s -S -f -n -X MKCOL "$OWNCLOUD_URL/$user/$remote_path/$dir" || {
                        err "Failed to create dir: $remote_path/$dir/"
                        return=1
                }
                printf . # show progress
                # get the list of files to upload in the format <file>,<file>,...,<file>
                files=$(find "$(dirname "$local_path")/$dir/" -type f -maxdepth 1 -print0 | tr '\0' ',' | sed 's/,$//')
                dbg "$files"
                [ -n "$files" ] && {
                    curl -s -S -f -n -T "{$files}" "$OWNCLOUD_URL/$user/$remote_path/$dir/" || {
                        err "Failed to upload files to $remote_path/$dir/"
                        return=1
                    }
                }
            done
            return $return
        }
    status=$?
    printf '\n'
    info "done"
    return $status
}

OWNCLOUD_URL="https://ftp.essensium.com/owncloud/remote.php/dav/files"
user="${user:=$(cut -f 4 -d ' '  < ~/.netrc)}"

main "$@"
