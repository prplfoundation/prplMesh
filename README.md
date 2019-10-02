# prplMesh
[![Build Status](https://travis-ci.com/prplfoundation/prplMesh.svg?branch=master)](https://travis-ci.org/prplfoundation/prplMesh)

TODO describe this project

## Requirements

To build prplMesh, you need (on Ubuntu) the following packages:

```bash
sudo apt install curl gcc cmake binutils git autoconf autogen libtool pkg-config \
     libreadline-dev libncurses-dev libssl-dev libjson-c-dev libnl-genl-3-dev libzmq3-dev \
     python python-yaml python-paramiko repo bridge-utils clang-format
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

Each component can be built with CMAKE, or use the [tools/maptools.py](tools/README.md) build command.

To build prplMesh in debug mode (for being able to debug with gdb), with all features and tests, run

```bash
./prplMesh/tools/maptools.py build map -f MSGLIB=zmq BUILD_TESTS=ON CMAKE_BUILD_TYPE=Debug
```

Subsequent builds don't need to repeat all of these options:

```bash
./prplMesh/tools/maptools.py build map
```

Run `./prplMesh/tools/maptools.py build --help` for more options.

Note - to build with Docker, see provided [building with docker](tools/docker/builder/README.md)

## Running Instructions

Once built, prplMesh controller, agent and framework can be started using `prplmesh_utils.sh`:

```bash
cd <path/to/prplmesh_root>/build/install/scripts
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
