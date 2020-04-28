#!/bin/bash
# Simple wrapper script to easymesh_cert/run_test_file.sh
# which also updates to the latest prplmesh IPK, run the tests and
# uploads the results
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################


scriptdir="$(cd "${0%/*}" && pwd)"

# shellcheck source=./owncloud/owncloud_definitions.sh
. "$scriptdir/owncloud/owncloud_definitions.sh"

rootdir=$(realpath "$scriptdir/../")

# shellcheck source=../tools/functions.sh
. "$rootdir/tools/functions.sh"

usage() {
    echo "usage: $(basename "$0") [-hboev] <test> [test]"
    echo "  options:"
    echo "      -h|--help - display this help."
    echo "      -v|--verbose - set verbosity (ucc logs also redirected to stdout)"
    echo "      -o|--log-folder - path to put the logs to (make sure it does not contain previous logs)."
    echo "      -e|--easymesh-cert - path to easymesh_cert repository (default ../easymesh_cert)"
    echo "      -d|--device - select DUT device to use, valid options - glinet-1300, netgear-rax40, turris-omnia, marvell, qualcomm, broadcom, mediatek (default: netgear-rax40)"
    echo "      -s|--ssh - target device ssh name (defined in ~/.ssh/config). (default: $TARGET_DEVICE_SSH)"
    echo "      --owncloud-upload - whether or not to upload results to owncloud. (default: false)"
    echo "      --owncloud-path - relative path in the owncloud server to upload results to. (default: $OWNCLOUD_PATH)"
    echo "      --skip-upgrade - skip prplmesh upgrade (default: false)"
    echo "      --skip-download - skip prplmesh download (default: false)"
    echo "'test' can either be a test name, or the path to a file containing"
    echo " test names (newline-separated)."
    echo
    echo "Multiple test files/names are allowed, and they can also be mixed."
    echo "For example, the following call would run the test 'MAP-5.3.1', and all the tests"
    echo "contained in the 'tests/all_controller_test.txt' file:"
    echo "$(basename "$0") MAP-5.3.1 tests/all_controller_test.txt"
    echo ""
    echo "The default is to run all agent certification tests."
    echo ""
    echo "Credentials for uploading to owncloud are taken from $HOME/.netrc"
    echo "Make sure it has a line like this:"
    echo "  machine ftp.essensium.com login <user> password <password>"
    echo ""
}

is_prplmesh_device() {
    case "$1" in
        netgear-rax40) return 0;;
        turris-omnia) return 0;;
        glinet-1300) return 0;;
    esac
    return 1
}

deploy_dummy_prplmesh() {
    info "Deploy prplmesh locally to /opt/prplMesh"
    rm -rf /opt/prplMesh
    mkdir -p /opt/
    if ! mv "$rootdir/build/install" /opt/prplMesh ; then
        err "Failed to deploy prplMesh locally, aborting"
        exit 1
    fi
}

upgrade_prplmesh() {
    if [ "$TARGET_DEVICE" = prplmesh ] ; then
        deploy_dummy_prplmesh && return 0 || return 1
    fi
    if ! is_prplmesh_device "$TARGET_DEVICE"; then
        echo "skip prplmesh upgrade for non prplmesh device $TARGET_DEVICE"
        return 1
    fi
    local ipk_dir="$rootdir/build/$TARGET_DEVICE/"

    if [ "$DOWNLOAD_PRPLMESH" = true ] ; then
        info "download latest ipk in $ipk_dir"
        mkdir -p "$ipk_dir"
        "$TOOLS_PATH"/download_ipk.sh --path "$ipk_dir" ${VERBOSE:+ -v} || {
            err "Failed to download prplmesh.ipk, abort"
            return 1
        }
    fi

    info "prplmesh build info:"
    cat "$ipk_dir/$PRPLMESH_BUILDINFO"

    info "deploy latest ipk to $TARGET_DEVICE_SSH"
    "$TOOLS_PATH"/deploy_ipk.sh "$TARGET_DEVICE_SSH" "$ipk_dir/$PRPLMESH_IPK" || {
        err "Failed to deploy prplmesh.ipk, abort"
        return 1
    }
    mv "$ipk_dir/$PRPLMESH_IPK" "$LOG_FOLDER"
    mv "$ipk_dir/$PRPLMESH_BUILDINFO" "$LOG_FOLDER"
}

main() {
    if ! OPTS=$(getopt -o 'hvb:o:e:d:s:' --long help,verbose,log-folder:,easymesh-cert:,device:,ssh:,owncloud-upload,owncloud-path:,skip-upgrade,skip-download -n 'parse-options' -- "$@"); then
        err "Failed parsing options." >&2
        usage
        exit 1
    fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -h | --help)            usage; exit 0;;
            -v | --verbose)         VERBOSE=true; shift;;
            -o | --log-folder)      LOG_FOLDER="$2"; shift 2;;
            -e | --easymesh-cert)   EASYMESH_CERT_PATH="$2"; shift 2;;
            -d | --device)          TARGET_DEVICE="$2"; shift 2;;
            -s | --ssh)             TARGET_DEVICE_SSH="$2"; shift 2;;
            --owncloud-upload)      OWNCLOUD_UPLOAD=true; shift;;
            --owncloud-path)        OWNCLOUD_PATH="$2"; shift 2;;
            --skip-upgrade)         UPGRADE_PRPLMESH=false; shift;;
            --skip-download)        DOWNLOAD_PRPLMESH=false; shift;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1;;
        esac
    done

    TESTS="$*"

    [ -n "$TESTS" ] || TESTS="$EASYMESH_CERT_PATH/tests/all_agent_tests.txt"
    [ -n "$LOG_FOLDER" ] || LOG_FOLDER="$EASYMESH_CERT_PATH/logs/$(date +%F_%H-%M-%S)"

    mkdir -p "$LOG_FOLDER"
    info "Logs location: $LOG_FOLDER"
    info "Tests to run: $TESTS"
    info "Device: $TARGET_DEVICE"

    [ "$UPGRADE_PRPLMESH" = "true" ] && {
        upgrade_prplmesh
    }
    
    info "Start running tests"
    "$EASYMESH_CERT_PATH"/run_test_file.sh -o "$LOG_FOLDER" -d "$TARGET_DEVICE" "$TESTS" ${VERBOSE:+ -v}
    test_results="$?"

    if [ -n "$OWNCLOUD_UPLOAD" ]; then
        if is_prplmesh_device "$TARGET_DEVICE"; then
            REMOTE_PATH="$TARGET_DEVICE"
        else
            REMOTE_PATH="certified/$TARGET_DEVICE"
        fi
        info "Uploading $LOG_FOLDER to $OWNCLOUD_PATH/$REMOTE_PATH"
        "$scriptdir"/owncloud/upload_to_owncloud.sh --direct "$OWNCLOUD_PATH/$REMOTE_PATH" "$LOG_FOLDER" || {
            err "Failed to upload $LOG_FOLDER"
            exit 1
        }
        success "URL: $(browse_url certification "$OWNCLOUD_PATH/$REMOTE_PATH/$(basename "$LOG_FOLDER")")"
    fi
    info "done"
    return "$test_results"
}

PRPLMESH_IPK=prplmesh.ipk
PRPLMESH_BUILDINFO=prplmesh.buildinfo
TOOLS_PATH="$rootdir/tools"
EASYMESH_CERT_PATH=$(realpath "$rootdir/../easymesh_cert")
TARGET_DEVICE="netgear-rax40"
TARGET_DEVICE_SSH="$TARGET_DEVICE-1"
OWNCLOUD_PATH=Nightly/agent_certification
UPGRADE_PRPLMESH=true
DOWNLOAD_PRPLMESH=true

main "$@"
