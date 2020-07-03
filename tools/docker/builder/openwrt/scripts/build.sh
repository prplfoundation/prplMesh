#!/bin/sh -e

# We have to copy the source directory, because we may not have
# write access to it, and openwrt needs to at least write '.source_dir':
cp -r /home/openwrt/prplMesh_source /home/openwrt/prplMesh
# We want to make sure that we do not keep anything built from the host:
(cd /home/openwrt/prplMesh && \
    rm -rf build ipkg-* .built* .configured* .pkgdir .prepared .quilt_checked .source_dir)

make package/prplmesh/prepare USE_SOURCE_DIR="/home/openwrt/prplMesh" V=s
make package/prplmesh/compile V=sc -j"$(nproc)"
mkdir -p artifacts
cat << EOT >> artifacts/prplmesh.buildinfo
TARGET_PROFILE=${TARGET_PROFILE}
OPENWRT_VERSION=${OPENWRT_VERSION}
PRPLMESH_VERSION=${PRPLMESH_VERSION}
EOT
find bin -name 'prplmesh*.ipk' -exec cp -v {} "artifacts/prplmesh.ipk" \;
find bin/targets/"$TARGET_SYSTEM"/"$SUBTARGET"/ -type f -maxdepth 1 -exec cp -v {} "artifacts/" \;
cp .config artifacts/openwrt.config
cp files/etc/prplwrt-version artifacts/
