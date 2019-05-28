# prplMesh Controller

The prplMesh Controller implements a Multi-AP v1.0 certifiable controller (work in progress).

# Build

Build system is CMAKE, and supports RDKB and UGW cross compilation builds.
Building natively (Linux only) is partially supported only for development process. Some of the components are skipped / built as stubs in this mode.

# Run Controller on Linux

The agent needs to run either on the same host as the controller or in another host with L2 connectivity (provided the BTL is using the local bus and not UDS mode).

Currently, this mode was tested on an Ubuntu18.04 with hwsim, the agent was compiled to UDS mode.

## Prerequisites

- Interface exists for every slave in the configuration file
- bridge named br-lan (can be empty)
- agent, controller, common and framework built natively (see prplMesh-tools for build instructions)

One time setup `modprobe mac80211_hwsim; brctl addbr br-lan`

Build all components `cd tools; python3 ./maptools.py build all`

## Running and debugging

Run the controller `sudo ./build/install/bin/beerocks_controller`

Run the agent `sudo ./build/install/bin/beerocks_agent`

Run the cli to see network map `sudo ./build/install/bin/beerocks_cli -c bml_conn_map`

>Note - Logs are available in /tmp/beerocks/logs