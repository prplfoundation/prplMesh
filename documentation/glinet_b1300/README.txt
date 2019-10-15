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

   The top of master branch currently has no firmware upgrade working
   in the Lucy web UI. You can copy the firmware .bin file to /tmp/fw.bin:
     scp <path_to_sysupgrade.bin> root@192.168.1.1:/tmp/fw.bin
   and upgrade by running sysupgrade on the target:
     ssh root@192.168.1.1 -C "sysupgrade /tmp/fw.bin"

6. Set up 2 wireless networks w/ WPA2-PSK security (one on radio0 another on radio1)


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
   export STAGING_DIR="$OPENWRT_DIR/staging_dir/target-arm_cortex-a7+neon-vfpv4_musl_eabi"
   export CMAKE_TOOLCHAIN_FILE=./prplMesh/tools/cmake/toolchain/glinet-b1300.cmake

4. Build purplmesh (debug version) for openwrt by running
     ./prplMesh/tools/maptools.py build map -f MSGLIB=zmq BUILD_TESTS=ON CMAKE_BUILD_TYPE=Debug


Run PurplMesh on the target platform:

1. The ./prplMesh/tools/commands/package_owrt.sh script strips, then packages beerocks binaries
   into a tarball, and can optionally upload to and extract the tarball on the target.
   Run it with -h option to see help. The following example uploads and extracts the files:
     ./prplMesh/tools/commands/package_owrt.sh root@192.168.1.1

2. Ssh to the target and start everything by running:
     /opt/beerocks/scripts/prplmesh_utils.sh start

3. Check the status by running:
     /opt/beerocks/scripts/prplmesh_utils.sh status

4. Examine the logs under /tmp/beerocks/logs:
     ls -l /tmp/beerocks/logs/


Example output:
root@MINIM:~# /opt/beerocks/scripts/prplmesh_utils.sh status
/opt/beerocks/scripts/prplmesh_utils.sh: status
14094 root      6412 S    /opt/beerocks/bin/local_bus
14095 root      6708 S    /opt/beerocks/bin/ieee1905_transport
14097 root     16736 S    /opt/beerocks/bin/beerocks_controller
14098 root     17656 S    /opt/beerocks/bin/beerocks_agent
14104 root     17792 S    /opt/beerocks/bin/beerocks_agent -i wlan0
14106 root     17792 S    /opt/beerocks/bin/beerocks_agent -i wlan1
14143 root      7752 S    /opt/beerocks/bin/beerocks_monitor -i wlan0
14145 root      7752 S    /opt/beerocks/bin/beerocks_monitor -i wlan1
14148 root      1104 S    /bin/sh /opt/beerocks/scripts/prplmesh_utils.sh status
14095 root      6708 S    /opt/beerocks/bin/ieee1905_transport
14094 root      6412 S    /opt/beerocks/bin/local_bus
OK Main agent operational
OK wlan0 radio agent operational
OK wlan1 radio agent operational
root@MINIM:~# ls -l /tmp/beerocks/logs/
-rw-r--r--    1 root     root         39982 Oct 11 22:22 beerocks_agent.20191011_204914.log
-rw-r--r--    1 root     root         27334 Oct 11 22:55 beerocks_agent.20191011_225521.log
lrwxrwxrwx    1 root     root            34 Oct 11 22:55 beerocks_agent.log -> beerocks_agent.20191011_225521.log
-rw-r--r--    1 root     root            24 Oct 11 22:55 beerocks_agent_std.log
-rw-r--r--    1 root     root         87318 Oct 11 22:23 beerocks_agent_wlan0.20191011_204915.log
-rw-r--r--    1 root     root         17568 Oct 11 22:55 beerocks_agent_wlan0.20191011_225522.log
lrwxrwxrwx    1 root     root            40 Oct 11 22:55 beerocks_agent_wlan0.log -> beerocks_agent_wlan0.20191011_225522.log
-rw-r--r--    1 root     root            24 Oct 11 22:55 beerocks_agent_wlan0_std.log
-rw-r--r--    1 root     root         91548 Oct 11 22:23 beerocks_agent_wlan1.20191011_204915.log
-rw-r--r--    1 root     root         24139 Oct 11 22:55 beerocks_agent_wlan1.20191011_225522.log
lrwxrwxrwx    1 root     root            40 Oct 11 22:55 beerocks_agent_wlan1.log -> beerocks_agent_wlan1.20191011_225522.log
-rw-r--r--    1 root     root            24 Oct 11 22:55 beerocks_agent_wlan1_std.log
-rw-r--r--    1 root     root         76574 Oct 11 22:22 beerocks_controller.20191011_204914.log
-rw-r--r--    1 root     root         54169 Oct 11 22:55 beerocks_controller.20191011_225521.log
lrwxrwxrwx    1 root     root            39 Oct 11 22:55 beerocks_controller.log -> beerocks_controller.20191011_225521.log
-rw-r--r--    1 root     root            24 Oct 11 22:55 beerocks_controller_std.log
-rw-r--r--    1 root     root          4954 Oct 11 22:23 beerocks_monitor_wlan0.20191011_204922.log
-rw-r--r--    1 root     root          2463 Oct 11 22:55 beerocks_monitor_wlan0.20191011_225529.log
lrwxrwxrwx    1 root     root            42 Oct 11 22:55 beerocks_monitor_wlan0.log -> beerocks_monitor_wlan0.20191011_225529.log
-rw-r--r--    1 root     root            24 Oct 11 22:55 beerocks_monitor_wlan0_std.log
-rw-r--r--    1 root     root          4962 Oct 11 22:23 beerocks_monitor_wlan1.20191011_204922.log
-rw-r--r--    1 root     root          2463 Oct 11 22:55 beerocks_monitor_wlan1.20191011_225529.log
lrwxrwxrwx    1 root     root            42 Oct 11 22:55 beerocks_monitor_wlan1.log -> beerocks_monitor_wlan1.20191011_225529.log
-rw-r--r--    1 root     root            24 Oct 11 22:55 beerocks_monitor_wlan1_std.log
