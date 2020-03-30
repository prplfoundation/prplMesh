# prplMesh
[![Build Status](https://gitlab.com/prpl-foundation/prplMesh/badges/master/pipeline.svg)](https://gitlab.com/prpl-foundation/prplMesh/pipelines)

prplMesh is an open-source, carrier-grade and certifiable implementation of the WiFi Alliance’s Multi-AP specification.

The result will be source-code covering both the agent and the controller part of the Multi-AP specification.
However, it is scoped as a reference implementation and will leave ample room for differentiation, for example for proprietary IP algorithms making intelligent decisions for the whole Multi-AP network.

In short, the project’s aim is to create a baseline for OEMs and developers to easily integrate Multi-AP into various products and platforms.
Initial targets include prplWrt and RDK-B with support for WiFi chipsets from almost any SoC vendor to be used in residential gateways, WiFi extenders from both retail brands and internet service providers.

This project is part of a wider collaboration between Prpl Foundation and Broadband Forum, and is based on a proven full mesh solution contributed by Intel Corp (Controller and Agent running on actual HW).

Architecture documentation can be found in the [documentation](documentation/) folder.

## Requirements

To build prplMesh, you need (on Ubuntu) the following packages:

```bash
sudo apt install curl gcc cmake binutils git autoconf autogen libtool pkg-config \
     libreadline-dev libncurses-dev libssl-dev libjson-c-dev libnl-genl-3-dev libzmq3-dev \
     python python-yaml python-paramiko repo bridge-utils clang-format ninja-build
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

## Build Instructions

Use standard CMake to build prplMesh, with a configure-build-install cycle.

To build prplMesh natively in debug mode (for being able to debug with gdb), with all features and tests, and installed in a local directory, run

```bash
cmake -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../build/install -H. -B../build -G Ninja
ninja -C ../build install
```

If you prefer, `make` can be used instead of `ninja` by removing the `-G Ninja` part.

For system-level install, the standard DESTDIR approach can be used for installing prplMesh as a package.

```bash
DESTDIR=/tmp/prplMesh-install ninja install
```

Alternatively, [tools/maptools.py](tools/README.md) can be used to build and install prplMesh with a single command.

Note - to build and run with Docker, see provided [building with docker](tools/docker/README.md)

Build artifacts are also available through the following link: https://ftp.essensium.com/owncloud/index.php/s/Sp7esHfmfH3bfwn .

## Running Instructions

Once built, prplMesh controller, agent and framework can be started using `prplmesh_utils.sh`:

```bash
cd <path/to/install/dir>/scripts
sudo ./prplmesh_utils.sh start
```

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
- radio agent wlan0 (son_slave_thread & ap manager) `/tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log`
- radio agent wlan2 (son_slave_thread & ap manager) `/tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log`

### Checking status

System is operational if you see `FSM: CONNECTED --> OPERATIONAL` in the main agent log. In the future there will be a bml cli command to verify operational state.

### Displaying the connection map (GUI)

There is a tool to display the connection map on a GUI in `tools/beerocks_analyzer`.
Its [README file](tools/beerocks_analyzer/README.md) explains how to use it.
