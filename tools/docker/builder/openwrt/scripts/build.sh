#!/bin/sh

sed -ri "s/-DTARGET_PLATFORM_TYPE=[^ ]*/-DTARGET_PLATFORM_TYPE=${TARGET_PLATFORM_TYPE}/" feeds/prpl/prplmesh/Makefile

# We have to copy the source directory, because we may not have
# write access to it, and openwrt needs to at least write '.source_dir':
cp -r /prplMesh /home/openwrt/prplMesh
# We want to make sure that we do not keep anything built from the host:
rm -rf /home/openwrt/prplMesh/build

make package/feeds/prpl/prplmesh/prepare USE_SOURCE_DIR="/home/openwrt/prplMesh" V=s
make package/feeds/prpl/prplmesh/compile V=sc -j"$(nproc)"
find bin -name 'prplmesh_*.ipk' -exec cp {} "prplmesh-${TARGET}-${TARGET_PLATFORM_TYPE}-${OPENWRT_VERSION}-${PRPLMESH_VERSION}.ipk" \;
