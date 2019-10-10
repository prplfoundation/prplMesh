Building PrplMesh for GlInet B1300 openwrt image.


Build OpenWRT image and toolchain:

1. Follow the openwrt build instruction for setting up the build system.
   We were using Ubuntu 16.04, here are the opewrt build system usage instructions (for reference):
   https://openwrt.org/docs/guide-developer/build-system/use-buildsystem

2. Update and install the feeds
   cd <openwrt_dir>
   ./scripts/feeds update -a
   ./scripts/feeds install -a

3. Download the diffconfig and build full .config file for Gl.Inet B1300
   cd <openwrt_dir>
   wget -O .config https://github.com/prplfoundation/prplMesh/raw/master/documentation/glinet_b1300/glinet_b1300_diffconfig
   make defconfig

4. Build an image (choose the number of cores for -j# option suitable for your build system).
   cd <openwrt_dir>
   make download
   make -j4

5. Load the image to the target router device, the image is located here:
   ls -l <openwrt_dir>/bin/targets/ipq40xx/generic/openwrt-snapshot-*-*-ipq40xx-generic-glinet_gl-b1300-squashfs-sysupgrade.bin
   Revert to factory defaults and verify that the image works as expectd.


Build PurplMesh for the target platform:

1. Clone purplmesh repo using the instructions here:
   https://github.com/prplfoundation/prplMesh/README.md
   Example (checking out into "purplmesh_root_owrt")
     mkdir prplmesh_root_owrt
     cd prplmesh_root_owrt
     repo init -u https://github.com/prplfoundation/prplMesh-manifest.git
     repo sync
     repo forall -p -c 'git checkout $REPO_RREV'

   Note: we only need the "purplMesh" git repository out of all of them listed in the manifest,
         but since the build system expects the folder structure where "purplMesh" is in
         directory underneath the root of the tree we use the repo approach

2. Check out the branch of the prplMesh code for for glinet-b1300 platform
   repo checkout dev/glinet-b1300-support prplMesh

3. Set up the environment:
   export OPENWRT_DIR=<openwrt_dir>
   export STAGING_DIR="$OPENWRT_DIR/target-arm_cortex-a7+neon-vfpv4_musl_eabi"
   export CMAKE_TOOLCHAIN_FILE=./prplMesh/tools/cmake/toolchain/glinet-b1300.cmake

4. Build purplmesh (debug version) for openwrt by running
   ./prplMesh/tools/maptools.py build map -f MSGLIB=zmq BUILD_TESTS=ON CMAKE_BUILD_TYPE=Debug

