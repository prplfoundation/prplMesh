#!/bin/bash
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2020 Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

scriptdir="$(cd "${0%/*}" && pwd)"
rootdir=$(realpath "$scriptdir/../../")

# shellcheck source=../../tools/functions.sh
. "$rootdir/tools/functions.sh"

# shellcheck source=./owncloud_definitions.sh
. "$scriptdir/owncloud_definitions.sh"


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

upload() {
    # Recursively upload directory from local path to remote path
    #
    # $1 - local path to directory
    # $2 - remote path to upload to

    local local_path remote_path
    local_path="$1"
    remote_path="$2"

    create_dir "$remote_path"
    find "$local_path" -type d -exec \
            realpath {} --relative-to="$(dirname "$local_path")" \; | {
                error=0
                while read -r -s dir; do
                    printf . # show progress
                    create_dir "$remote_path/$dir" || {
                        error="$?"
                        continue
                    }
                    printf . # show progress
                    # get the list of files to upload in the format <file>,<file>,...,<file>
                    files=$(find "$(dirname "$local_path")/$dir/" -type f -maxdepth 1 -print0 | tr '\0' ',' | sed 's/,$//')
                    dbg "$files"
                    [ -n "$files" ] && {
                        upload_files "$remote_path/$dir" "$files" || {
                            error="$?"
                        }
                    }
                done
                echo
                return $error
            }
}

main() {
    if ! OPTS=$(getopt -o 'hvu:' --long help,verbose,url: -n 'parse-options' -- "$@"); then
        echo echo "Failed parsing options." >&2; usage; exit 1
    fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -h | --help)            usage; exit 0;;
            -v | --verbose)         export VERBOSE=true; QUIET=; shift;;
            -u | --owncloud-url)    OWNCLOUD_URL="$2"; shift 2;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1;;
        esac
    done

    [ -n "$1" ] || { usage; err "Missing remote-path"; exit 1; }
    remote_path="$1"; shift
    [ -n "$1" ] || { usage; err "Missing local-path"; exit 1; }
    local_path="$1"; shift

    size=$(du -hcs $local_path | grep total | cut -f1)
    info "upload $size from $local_path to $remote_path at $OWNCLOUD_URL (using user $user)"

    if ! command -v uuidgen > /dev/null ; then
        err "You need uuidgen to use this script. Please install it and try again."
        exit 1
    fi

    # We'll group uploads in the following directory in the user home
    # we don't care if create_dir fails because it may already exist:
    create_dir "temp_uploads" > /dev/null 2>&1
    remote_temp_path="temp_uploads/$(uuidgen)"
    
    echo "upload $local_path to $remote_path"
    start=$(date +%s)
    upload "$local_path" "$remote_temp_path" || { err "Uploading to a temporary directory failed with status $?"; exit 1; }
    end=$(date +%s)
    echo "Runtime: $((end-start)) seconds"

    echo "Moving the temporary directory $remote_temp_path to $remote_path"
    start=$(date +%s)
    move "$remote_temp_path" "$remote_path" || { err "Move temporary directory failed with status $?"; exit 1; }
    end=$(date +%s)
    echo "Runtime: $((end-start)) seconds"
    success "Upload $local_path to $remote_path succeeded."
}

QUIET=true

main "$@"
