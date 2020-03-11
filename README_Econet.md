# prplMesh
[![Build Status](https://gitlab.com/prpl-foundation/prplMesh/badges/master/pipeline.svg)](https://gitlab.com/prpl-foundation/prplMesh/pipelines)

prplMesh is an open-source, carrier-grade and certifiable implementation of the WiFi Alliance’s Multi-AP specification.

The result will be source-code covering both the agent and the controller part of the Multi-AP specification.
However, it is scoped as a reference implementation and will leave ample room for differentiation, for example for proprietary IP algorithms making intelligent decisions for the whole Multi-AP network.

In short, the project’s aim is to create a baseline for OEMs and developers to easily integrate Multi-AP into various products and platforms.
Initial targets include prplWrt and RDK-B with support for WiFi chipsets from almost any SoC vendor to be used in residential gateways, WiFi extenders from both retail brands and internet service providers.

This project is part of a wider collaboration between Prpl Foundation and Broadband Forum, and is based on a proven full mesh solution contributed by Intel Corp (Controller and Agent running on actual HW).

Architecture documentation can be found in the [documentation](documentation/) folder.


## Requirements Econet

To build prplMesh to run in the Econet target, you need (on Ubuntu) the following packages:

```bash
sudo apt install curl gcc cmake binutils git autoconf autogen libtool pkg-config \
     libreadline-dev libncurses-dev libssl-dev libjson-c-dev libnl-genl-3-dev libzmq3-dev \
     python python-yaml python-paramiko repo bridge-utils clang-format ninja-build
```

Extra packages for Econet:

```bash
sudo apt install  gcc-5-mipsel-linux-gnu libc6-mipsel-cross libc6-dev-mipsel-cross
```

If you haven't done so already, set up your git configuration:

```bash
git config --global user.email your@email.address
git config --global user.name "Your Name"
```

There are several dependencies for which we require a specific version. To ease
deployment of these, they are collected in a google repo manifest file:

```bash
mkdir prplmesh_root
cd prplmesh_root
repo init -u https://github.com/prplfoundation/prplMesh-manifest.git
repo sync
repo forall -p -c 'git checkout $REPO_RREV'
```

### Checkout Econet branch

Econet source code is located in a separate branch: econet/master.

cd prplMesh
git checkout econet/master


## Build Dependencies for Econet

Library dependencies need to be crosscompiled with the previously installed toolchain to be used at prplmesh build.

For the 'dummy' BWL implementation, the dependencies are: a) json-c b) zeromq c) openssl
The type of build (static or dynamic) should be consistent with the type of build that will be done later at prplmesh. Instructions here assume a full static build.

For a) and b) you can normally use the master branch in some git repos online (each one in its own folder)
git clone https://github.com/json-c/json-c.git
git clone https://github.com/zeromq/libzmq.git


Assuming INSTALL_FOLDER_LIBS environment variable is set as the folder where the compiled libraries will be installed, they can be configured and compiled as follows:
cd json-c
./autogen.sh
./configure --host=mipsel-linux-gnu --enable-shared=no --prefix=$INSTALL_FOLDER_LIBS
make V=1 install

The same procedure for ZeroMQ.

For c) you can copy the apps/public/openssl 1.0.1p folder from the SDK (already configured):
make INSTALL_PREFIX=$INSTALL_FOLDER_LIBS INSTALLTOP=$INSTALL_FOLDER_LIBS OPENSSLDIR=$INSTALL_FOLDER_LIBS  clean
make INSTALL_PREFIX=$INSTALL_FOLDER_LIBS INSTALLTOP=$INSTALL_FOLDER_LIBS OPENSSLDIR=$INSTALL_FOLDER_LIBS install




## Build Instructions for Econet

Use standard CMake to build prplMesh, with a configure-build-install cycle.

To build prplMesh natively in debug mode (for being able to debug with gdb), with all features and tests, and installed in a local directory, run

```bash
cmake -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/tmp/prpl/ -H. -B../build  -DCMAKE_VERBOSE_MAKEFILE=1  -DCMAKE_TOOLCHAIN_FILE=tools/cmake/toolchain/econet.cmake   -DCMAKE_FIND_ROOT_PATH=/home/juan/Development/customers/EcoNet/prpl/dependencies/install -DPKG_CONFIG_PATH=/home/juan/Development/customers/EcoNet/prpl/dependencies/install  -DBUILD_FULL_STATIC=ON -DPRPLMESH_PLATFORM_BASE_DIR=/home/juan/Development/customers/EcoNet/releasebsp_testprof -DPRPLMESH_PLATFORM_TOOLCHAIN_PREFIX=/usr/bin/mipsel-linux-gnu- -DPRPLMESH_PLATFORM_TOOLCHAIN_C_COMPILER=gcc-5 -DPRPLMESH_PLATFORM_TOOLCHAIN_CXX_COMPILER=g++-5 -G Ninja

ninja -C ../build install
```

If you prefer, `make` can be used instead of `ninja` by removing the `-G Ninja` part.

Explanation of settings:
- CMAKE_INSTALL_PREFIX: Installation path. For the moment /tmp/prpl is used for local 'installation' on the cross-compilation host and the Econet device.
- CMAKE_TOOLCHAIN_FILE: econet cmake configuration file
- CMAKE_BUILD_TYPE: Release or Debug
- CMAKE_FIND_ROOT_PATH: INSTALL_FOLDER_LIBS (see above). Path for finding library dependencies
- PKG_CONFIG_PATH: INSTALL_FOLDER_LIBS (see above). Path where dependencies are installed. Pkgconfig tool would search for related pkconfig info on dependencies
- BUILD_FULL_STATIC: Build the application full static. For this is necessary that all library dependencies (e.g.: json-c) are built as static libraries
- PRPLMESH_PLATFORM_BASE_DIR: Location of Econet's SDK
- PRPLMESH_PLATFORM_TOOLCHAIN_PREFIX: Toolchain prefix
- PRPLMESH_PLATFORM_TOOLCHAIN_C_COMPILER. Default value is 'gcc-5' and not 'gcc'.
- PRPLMESH_PLATFORM_TOOLCHAIN_CXX_COMPILER. Default value is 'g++-5' and not 'g++'

For system-level install, the standard DESTDIR approach can be used for installing prplMesh as a package.

```bash
ninja -C ../build install
```

Or using make:
```bash
make -C ../build install
```

DESTDIR is not necessary if CMAKE_INSTALL_PREFIX was set previously

Alternatively, [tools/maptools.py](tools/README.md) can be used to build and install prplMesh with a single command.

Note - to build and run with Docker, see provided [building with docker](tools/docker/README.md)


## Copying to the device

```bash
cd /tmp/prpl/ 
tar czf ../prpl_inst.tgz * 
cp ../prpl_inst.tgz /home/test
```

Where /home/test is the root folder of an FTP server that can be accessed from the device.

Then, *on the Econet device* run:

```bash
cd /tmp
mkdir prpl
cd prpl
ftpget 192.168.1.2 prpl_inst.tgz prpl_inst.tgz -u test -p test
tar xzf prpl_inst.tgz
rm prpl_inst.tgz
```
Where 192.168.1.2 is the IP of the PC where the built was made and where the FTP server is accessible.

Since everything under /tmp/ by default is stored in ramfs (in RAM), in order not to occupy RAM after copying files, an alternative is to mount an USB stick with an EXT2 partition and then create a soft link under /tmp/prpl to the mount point (e.g. ln -s /mnt/usb1_2 /tmp/prpl). Remounting would be necessary after every copy to release the RAM.


## Running Instructions

Once built, prplMesh controller, agent and framework can be started using `prplmesh_utils.sh`:

```bash
/tmp/prpl/scripts/prplmesh_utils.sh  -m CA -C ra0 -D ra1 -- start
```
This starts a Controller and an Agent over ra0 and ra1 interfaces. 

## Debugging Instructions

To debug prplMesh controller, agent or cli it is needed to install 'Native Debug'
extension for Visual Studio Code. Also prplMesh solution should be compiled with
`CMAKE_BUILD_TYPE=Debug` flag.
Debug instruction: 
1. Start prplMesh solution(read Running Instructions)
2. To remote debug it is needed to start gdbserver
```bash
gdbserver :9999 --attach <pid of controller/agent/cli>
```
2. Go to debug tab in the VSCode and choose one the option from the dropdown list.
3. Add breakpoint and click start debugging.

### Log files locations

- framework `/tmp/$USER/mapf`
- controller `/tmp/$USER/beerocks/logs/beerocks_controller.log`
- main agent (platform manager & backhaul manager) `/tmp/$USER/beerocks/logs/beerocks_agent.log`
- radio agent ra0 (son_slave_thread & ap manager) `/tmp/$USER/beerocks/logs/beerocks_agent_ra0.log`
- radio agent ra1 (son_slave_thread & ap manager) `/tmp/$USER/beerocks/logs/beerocks_agent_ra1.log`

### Checking status

System is operational if you see `FSM: CONNECTED --> OPERATIONAL` in the main agent log. In the future there will be a bml cli command to verify operational state.

### Displaying the connection map (GUI)

There is a tool to display the connection map on a GUI in `tools/beerocks_analyzer`.
Its [README file](tools/beerocks_analyzer/README.md) explains how to use it.
