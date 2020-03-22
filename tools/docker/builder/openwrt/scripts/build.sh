#!/bin/sh -e

# We have to copy the source directory, because we may not have
# write access to it, and openwrt needs to at least write '.source_dir':
cp -r /home/openwrt/prplMesh_source /home/openwrt/prplMesh
# We want to make sure that we do not keep anything built from the host:
rm -rf /home/openwrt/prplMesh/build

make package/feeds/prpl/prplmesh/prepare USE_SOURCE_DIR="/home/openwrt/prplMesh" V=s
make package/feeds/prpl/prplmesh/compile V=sc -j"$(nproc)"
mkdir -p artifacts
cat << EOT >> artifacts/prplmesh.buildinfo
TARGET_PROFILE=${TARGET_PROFILE}
OPENWRT_VERSION=${OPENWRT_VERSION}
PRPLMESH_VERSION=${PRPLMESH_VERSION}
EOT
find bin -name 'prplmesh*.ipk' -exec cp -v {} "artifacts/prplmesh.ipk" \;
find bin/targets/"$TARGET_SYSTEM"/"$SUBTARGET"/ -type f -maxdepth 1 -exec cp -v {} "artifacts/" \;
