#!/bin/sh -e

scriptdir="$(cd "${0%/*}"; pwd)"
rootdir="${scriptdir%/*/*/*/*}"

if [ ! -f "$1" ] ; then
    echo "Please specify a file containing the build args as the
    first argument"
    exit 1
fi

# Source the args file:
. "$(realpath "$1")"

if [ -z "$OPENWRT_REPOSITORY" ] ; then
    OPENWRT_REPOSITORY=https://git.prpl.dev/prplmesh/prplwrt.git
    echo "OPENWRT_REPOSITORY not set, using default value $OPENWRT_REPOSITORY"

fi

if [ -z "$OPENWRT_VERSION" ] ; then
    OPENWRT_VERSION=9d2efd
    echo "OPENWRT_VERSION not set, using default value $OPENWRT_VERSION"
fi

if [ -z "$TARGET" ] ; then
    TARGET=mvebu
    echo "TARGET not set, using default value $TARGET"
fi

if [ -z "$TARGET_PLATFORM_TYPE" ] ; then
    TARGET_PLATFORM_TYPE=turris-omnia
    echo "TARGET_PLATFORM_TYPE not set, using default value $TARGET_PLATFORM_TYPE"
fi

if [ -z "$PRPL_FEED" ] ; then
    PRPL_FEED='https://git.prpl.dev/prplmesh/iwlwav.git^06a0126d5fb53b1d65bad90757a5f9f5f77419ca'
    echo "PRPL_FEED not set, using default value $PRPL_FEED"
fi

export OPENWRT_REPOSITORY
export OPENWRT_VERSION
export TARGET
export TARGET_PLATFORM_TYPE
PRPLMESH_VERSION="$(git describe --always --dirty --exclude '*')"
export PRPLMESH_VERSION
export PRPL_FEED

image_tag="prplmesh-builder-${TARGET}:${OPENWRT_VERSION}"
container_name="prplmesh-builder-$(date +%F_%H-%M-%S)"

echo "Image will be tagged as $image_tag"
echo "Container name will be $container_name"

# We first need to build the corresponding images
docker build --tag "$image_tag" \
       --build-arg OPENWRT_REPOSITORY \
       --build-arg OPENWRT_VERSION \
       --build-arg TARGET \
       --build-arg PRPL_FEED \
       "$scriptdir/"

# Next we'll build the specified prplMesh version and copy out the ipk
docker run -i \
       --name "$container_name" \
       -e TARGET \
       -e TARGET_PLATFORM_TYPE \
       -e OPENWRT_VERSION \
       -e PRPLMESH_VERSION \
       -v "$scriptdir/scripts:/home/openwrt/openwrt_sdk/build_scripts/:ro" \
       -v "${rootdir}:/prplMesh:ro" \
       "$image_tag" \
       ./build_scripts/build.sh

docker cp "${container_name}:/home/openwrt/openwrt_sdk/prplmesh-${TARGET}-${TARGET_PLATFORM_TYPE}-${OPENWRT_VERSION}-${PRPLMESH_VERSION}.ipk" .

docker rm "${container_name}"
