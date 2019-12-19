#!/bin/sh -e

# We have to copy the source directory, because we may not have
# write access to it, and openwrt needs to at least write '.source_dir':
cp -r /home/openwrt/prplMesh_source /home/openwrt/prplMesh
# We want to make sure that we do not keep anything built from the host:
rm -rf /home/openwrt/prplMesh/build

make package/feeds/prpl/prplmesh/prepare USE_SOURCE_DIR="/home/openwrt/prplMesh" V=s
make package/feeds/prpl/prplmesh/compile V=sc -j"$(nproc)"
find bin -name 'prplmesh_*.ipk' -exec cp -v {} "prplmesh-${TARGET}-${OPENWRT_VERSION}-${PRPLMESH_VERSION}.ipk" \;
