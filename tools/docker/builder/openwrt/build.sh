#!/bin/sh -e
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

scriptdir="$(cd "${0%/*}"; pwd)"
rootdir="${scriptdir%/*/*/*/*}"

# shellcheck source=functions.sh
. "${rootdir}/tools/functions.sh"

usage() {
    echo "usage: $(basename "$0") -d <target_device> [-hfiortv]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -v|--verbose - increase the script's verbosity"
    echo "      -d|--target-device the device to build for"
    echo "      --docker-target-stage docker target build stage (implies -i)"
    echo "      -i|--image - build the docker image only"
    echo "      -o|--openwrt-version - the openwrt version to use"
    echo "      -r|--openwrt-repository - the openwrt repository to use"
    echo "      -t|--tag - the tag to use for the builder image"
    echo " -d is always required."
    echo ""
    echo "The following environment variables will affect the build:"
    echo " - PRPL_FEED: the prpl feed that will be used to install prplMesh."
    echo "   default: $PRPL_FEED"

}

build_image() {
    # We first need to build the corresponding images
    docker build --tag "$image_tag" \
           --build-arg OPENWRT_REPOSITORY="$OPENWRT_REPOSITORY" \
           --build-arg OPENWRT_VERSION="$OPENWRT_VERSION" \
           --build-arg TARGET_SYSTEM="$TARGET_SYSTEM" \
           --build-arg SUBTARGET="$SUBTARGET" \
           --build-arg TARGET_PROFILE="$TARGET_PROFILE" \
           --build-arg PRPL_FEED="$PRPL_FEED" \
           --build-arg PRPLMESH_VARIANT="$PRPLMESH_VARIANT" \
           --target="$DOCKER_TARGET_STAGE" \
           "$scriptdir/"
}

build_prplmesh() {
    build_dir="$1"
    container_name="prplmesh-builder-${TARGET_DEVICE}-$(uuidgen)"
    dbg "Container name will be $container_name"
    trap 'docker rm -f $container_name' EXIT
    docker run -i \
           --name "$container_name" \
           -e TARGET_SYSTEM \
           -e SUBTARGET \
           -e TARGET_PROFILE \
           -e OPENWRT_VERSION \
           -e PRPLMESH_VERSION \
           -v "$scriptdir/scripts:/home/openwrt/openwrt/build_scripts/:ro" \
           -v "${rootdir}:/home/openwrt/prplMesh_source:ro" \
           "$image_tag" \
           ./build_scripts/build.sh
    mkdir -p "$build_dir"
    # Note: docker cp does not support globbing, so we need to copy the folder
    docker cp "${container_name}:/home/openwrt/openwrt/artifacts/" "$build_dir"
    mv "$build_dir/artifacts/"* "$build_dir"
    rm -r "$build_dir/artifacts/"
}

main() {

    if ! command -v uuidgen > /dev/null ; then
        err "You need uuidgen to use this script. Please install it and try again."
        exit 1
    fi

    if ! OPTS=$(getopt -o 'hvd:io:r:t:' --long help,verbose,target-device:,docker-target-stage:,image,openwrt-version:,openwrt-repository:,tag: -n 'parse-options' -- "$@"); then
        err "Failed parsing options." >&2
        usage
        exit 1
    fi

    eval set -- "$OPTS"

    SUPPORTED_TARGETS="turris-omnia glinet-b1300 netgear-rax40"

    while true; do
        case "$1" in
            -h | --help)               usage; exit 0; shift ;;
            -v | --verbose)            VERBOSE=true; shift ;;
            -d | --target-device)      TARGET_DEVICE="$2"; shift ; shift ;;
            --docker-target-stage)     DOCKER_TARGET_STAGE="$2"; IMAGE_ONLY=true; shift 2 ;;
            -i | --image)              IMAGE_ONLY=true; shift ;;
            -o | --openwrt-version)    OPENWRT_VERSION="$2"; shift; shift ;;
            -r | --openwrt-repository) OPENWRT_REPOSITORY="$2"; shift; shift ;;
            -t | --tag)                TAG="$2"; shift ; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    case "$TARGET_DEVICE" in
        turris-omnia)
            TARGET_SYSTEM=mvebu
            SUBTARGET=cortexa9
            TARGET_PROFILE=DEVICE_cznic_turris-omnia
            ;;
        glinet-b1300)
            TARGET_SYSTEM=ipq40xx
            SUBTARGET=generic
            TARGET_PROFILE=DEVICE_glinet_gl-b1300
            ;;
        netgear-rax40)
            TARGET_SYSTEM=intel_mips
            SUBTARGET=xrx500
            TARGET_PROFILE=DEVICE_NETGEAR_RAX40
            PRPLMESH_VARIANT="-dwpal"
            ;;
        *)
            err "Unknown target device: $TARGET_DEVICE"
            info "Currently supported targets are:"
            for i in $SUPPORTED_TARGETS ; do
                info "$i"
            done
            exit 1
            ;;
    esac

    dbg "OPENWRT_REPOSITORY=$OPENWRT_REPOSITORY"
    dbg "OPENWRT_VERSION=$OPENWRT_VERSION"
    dbg "PRPL_FEED=$PRPL_FEED"
    dbg "IMAGE_ONLY=$IMAGE_ONLY"
    dbg "TARGET_DEVICE=$TARGET_DEVICE"
    dbg "TAG=$TAG"
    dbg "TARGET_SYSTEM=$TARGET_SYSTEM"
    dbg "SUBTARGET=$SUBTARGET"
    dbg "TARGET_PROFILE=$TARGET_PROFILE"
    dbg "PRPLMESH_VARIANT=$PRPLMESH_VARIANT"

    if [ -n "$TAG" ] ; then
        image_tag="$TAG"
    else
        image_tag="${DOCKER_TARGET_STAGE}-${TARGET_DEVICE}:${OPENWRT_VERSION}"
        dbg "image tag not set, using default value $image_tag"
    fi

    export OPENWRT_REPOSITORY
    export OPENWRT_VERSION
    export TARGET_SYSTEM
    export SUBTARGET
    export TARGET_PROFILE
    # We want to exclude tags from the git-describe output because we
    # have no relevant tags to use at the moment.
    # The '--exclude' option of git-describe is not available on older
    # git version, so we use sed instead.
    PRPLMESH_VERSION="$(git describe --always --dirty | sed -e 's/.*-g//')"
    export PRPLMESH_VERSION
    export PRPL_FEED
    export PRPLMESH_VARIANT

    if [ $IMAGE_ONLY = true ] ; then
        build_image
        exit $?
    fi

    build_image
    build_prplmesh "$rootdir/build/$TARGET_DEVICE"

}

VERBOSE=false
IMAGE_ONLY=false
OPENWRT_REPOSITORY='https://git.prpl.dev/prplmesh/prplwrt.git'
OPENWRT_VERSION='bd19f9ab26ad234b6f10cce23cd0dc41b9371929'
PRPL_FEED='https://git.prpl.dev/prplmesh/feed-prpl.git^c9f9407af79dd7ae8cfbe424ab6cf31fb09f5d57'
PRPLMESH_VARIANT="-nl80211"
DOCKER_TARGET_STAGE="prplmesh-builder"

main "$@"
