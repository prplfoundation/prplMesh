# prplMesh Agent
The prplMesh Agent implements a Multi-AP v1.0 certifiable agent (work in progress).

# Build
Build system is CMAKE, and supports RDKB and UGW cross compilation builds.
Building natively (Linux only) is partially supported only for development process.
Some of the components are skipped / built as stubs in this mode.

# Run agent on Linux
Running the agent on Linux allows easier development process without the need for real hardware.
Instead, mac80211_hwsim can be used to simulate WLAN radios.
A dummy bridge named br-lan must also exist since the agent uses the bridge MAC as the MultiAP AL-MAC.
Last, the controller needs to run either on the same host or in another host with L2 connectivity (provided the BTL is using the local bus and not UDS mode).

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