# MultiAP Framework
This project includes the implementation of the following MultiAP modules as defined in the MultiAP low level architecture - broker (local & global), MultiAP library, WFA-CA, discovery agent and the 1905 transport, platform agent core & PAL (the PAL links to platform specific library implementing the PAL for the platform - libplat.so).

The MultiAP controller and agent are external to this project.

![Intel MultiAP Framework](resources/multiap_architecture.png)

### Table of Contents
<a name="table-of-contents"></a>
+ [Prerequisites](#prereq)
+ [Build & Install](#build)
+ [Tests](#test)
+ [References](#ref)
+ [Versioning](#ver)
+ [Authors](#authors)
+ [License](#license)

## Prerequisites
<a name="prereq"></a>

The following are needed for building the MultiAP framework on any Linux machine (local and cross compile):

| Package      |  Version  |
| --------- | -----:|
| cmake  | >=2.8 |
| c++11  |   |
| json-c  |  0.12.1  |
| ubus  |  any  |

Cross compiling - currently supporting UGW & RDKB cross compiling, see [feed_multiap](https://gts-chd.intel.com/projects/SW_UGW/repos/feed_multiap/browse).

## Build & Install
<a name="build"></a>

It is recommended to build "out of tree". This is specially useful if the same sources are used for compiling on the local machine and cross compiling.

    git clone ssh://git@gts-chd.intel.com:29418/sw_ugw/multiap.git 
    mkdir build && cd build
    cmake ../multiap && make install
> For building examples, add -DBUILD_EXAMPLES=ON to the cmake command.
See examples/*/README.md for each example instructions

At this point multiap framework is built, binaries are available in `build/install` folder.

## Tests
<a name="test"></a>

The MultiAP framework project has support for CMAKE CTEST.
To compile and run the tests: `cmake ../multiap && make install && make test`

### Unit Tests

Supported Unit Tests:
- version - messaging library sanity test and show version
- broker_test - test the broker functionality by running a broker, subscriber and publisher that sends a single message
- socket_test - test socket class
- poller_test - test poller class
- transport_test - Sending 1905 CMDUs over 1905.1 Transport service over local bus + raw socket

## References
<a name="ref"></a>
* [MultiAP low level architecture](https://polarion.imu.intel.com/polarion/#/project/IRE/wiki/Design/Intel%20MultiAP%20infrastructure%20HLD)

## Versioning
<a name="ver"></a>
1.4.0

## Authors
<a name="authors"></a>
* **Tomer Eliyahu**  tomer.b.eliyahu@intel.com

## License
<a name="license"></a>
This project is licensed under the BSD+Pantet License - see the [LICENSE](LICENSE) file for details

[__Table Of contents__](#table-of-contents)

