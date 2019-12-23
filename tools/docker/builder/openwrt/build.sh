#!/bin/sh -e

scriptdir="$(cd "${0%/*}"; pwd)"
rootdir="${scriptdir%/*/*/*/*}"

. "${rootdir}/tools/docker/functions.sh"

usage() {
    echo "usage: $(basename $0) -d <target_device> [-hfiortv]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -b|--build-options - add docker build options"
    echo "      -d|--target-device the device to build for"
    echo "      -f|--prpl-feed the prpl feed to use"
    echo "      -i|--image - build the docker image only"
    echo "      -o|--openwrt-version - the openwrt version to use"
    echo "      -r|--openwrt-repository - the openwrt repository to use"
    echo "      -t|--tag - the tag to use for the builder image"
    echo "      -v|--verbose - verbosity on"
    echo " -d is always required."
}

build_image() {
    # We first need to build the corresponding images
    docker build --tag "$image_tag" \
           --build-arg OPENWRT_REPOSITORY \
           --build-arg OPENWRT_VERSION \
           --build-arg TARGET \
           --build-arg PRPL_FEED \
           $BUILD_OPTIONS \
           "$scriptdir/"
}

build_prplmesh() {
    dbg "Container name will be $container_name"
    container_name="prplmesh-builder-$(date +%F_%H-%M-%S)"
    docker run -i \
           --name "$container_name" \
           -e TARGET \
           -e OPENWRT_VERSION \
           -e PRPLMESH_VERSION \
           -v "$scriptdir/scripts:/home/openwrt/openwrt_sdk/build_scripts/:ro" \
           -v "${rootdir}:/home/openwrt/prplMesh_source:ro" \
           "$image_tag" \
           ./build_scripts/build.sh

    docker cp "${container_name}:/home/openwrt/openwrt_sdk/prplmesh-${TARGET}-${OPENWRT_VERSION}-${PRPLMESH_VERSION}.ipk" .

    docker rm "${container_name}"
}

main() {
    OPTS=`getopt -o 'hb:d:f:io:r:t:v' --long help,build-options:,device:,prpl-feed:,image,openwrt-version:,openwrt-repository:,tag:,verbose -n 'parse-options' -- "$@"`

    if [ $? != 0 ] ; then err "Failed parsing options." >&2 ; usage; exit 1 ; fi

    eval set -- "$OPTS"

    SUPPORTED_TARGETS="turris-omnia glinet-b1300"

    while true; do
        case "$1" in
            -h | --help)               usage; exit 0; shift ;;
            -b | --build-options)      BUILD_OPTIONS="$2"; shift; shift ;;
            -d | --target-device)      TARGET_DEVICE="$2"; shift ; shift ;;
            -f | --prpl-feed)          PRPL_FEED="$2"; shift; shift ;;
            -i | --image)              IMAGE_ONLY=true; shift ;;
            -o | --openwrt-version)    OPENWRT_VERSION="$2"; shift; shift ;;
            -r | --openwrt-repository) OPENWRT_REPOSITORY="$2"; shift; shift ;;
            -t | --tag)                TAG="$2"; shift ; shift ;;
            -v | --verbose)            VERBOSE=true; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    dbg "BUILD_OPTIONS=$BUILD_OPTIONS"
    dbg "OPENWRT_REPOSITORY=$OPENWRT_REPOSITORY"
    dbg "OPENWRT_VERSION=$OPENWRT_VERSION"
    dbg "PRPL_FEED=$PRPL_FEED"
    dbg "IMAGE_ONLY=$IMAGE_ONLY"
    dbg "TARGET_DEVICE=$TARGET_DEVICE"
    dbg "TAG=$TAG"

    case "$TARGET_DEVICE" in
        turris-omnia)
            TARGET=mvebu
            ;;
        glinet-b1300)
            TARGET=ipq40xx
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

    if [ -n "$TAG" ] ; then
        image_tag="$TAG"
    else
        image_tag="prplmesh-builder-${TARGET}:${OPENWRT_VERSION}"
        dbg "image tag not set, using default value $image_tag"
    fi

    export OPENWRT_REPOSITORY
    export OPENWRT_VERSION
    export TARGET
    # We want to exclude tags from the git-describe output because we
    # have no relevant tags to use at the moment.
    # The '--exclude' option of git-describe is not available on older
    # git version, so we use sed instead.
    PRPLMESH_VERSION="$(git describe --always --dirty | sed -e 's/.*-g//')"
    export PRPLMESH_VERSION
    export PRPL_FEED

    if [ $IMAGE_ONLY = true ] ; then
        build_image
        exit $?
    fi

    build_image
    build_prplmesh

}

IMAGE_ONLY=false
VERBOSE=false
BUILD_OPTIONS=""
OPENWRT_REPOSITORY='https://git.prpl.dev/prplmesh/prplwrt.git'
OPENWRT_VERSION='9d2efd'
PRPL_FEED='https://git.prpl.dev/prplmesh/iwlwav.git^06a0126d5fb53b1d65bad90757a5f9f5f77419ca'

main "$@"
