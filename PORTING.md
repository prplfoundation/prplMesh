# prplMesh porting guide

## Prerequisites and dependencies

* Toolchain:
    * Need to support C++11.
    * Advisable to use gcc version 5.* and onwards.
    * Ideally use the cross compilation toolchain already existing for your target device (use the same libc).
        * If not possible due to the mentioned requirements, it is possible to use compilation toolchains in Debian, Ubuntu or generate your own one (Buildroot).
        * In order to avoid using two different libc installations you can consider building prplMesh statically. (Would need to link against static libraries).

* Dependencies:
    * Basic ones are covered in the main README file. (Those should cover any of the BWLs supported).
    * Further dependencies depend on your interfaces with your platform (for example, how to interact with your wireless driver).
        * Identify how your wireless driver(s) interact with the system and see if an already existing BWL is enough for you (nl80211 if your wireless driver is well supported in GNU/Linux).
        * Otherwise create or extend an existing BWL.

* Resources:
    * An installation covering controller and agent in the same device can consume up to 20MB of RAM ( compiled statically, without activity ).

## prplMesh Startup

* Adapt steps needed for prplMesh environment from [prplmesh_utils](common/beerocks/scripts/prplmesh_utils.sh.in).

## Porting guideline

* To get a grasp of how prplMesh should work, setup and analyze a prplMesh dummy setup using [Docker](tools/docker/README.md).

* First try to make prplMesh work cross-compiled for your target platform using generic platform and DUMMY BWL.
    * Identify this way any changes needed to be done to prplMesh for your target environment.
    * Check that controller and agents can establish connection in your platform through wired backhaul.

* To support your wireless device, make sure the methods in BWL are implemented and working.
    * STA events.
        * Implement event handling for STA connection and disconnection.
    * Allow/disallow STA to be connected to an agent.
        * Required to steer STAs, forcefully disconnect them.
    * STA, VAP, Radio statistics. 
    * 802.11k support in the driver to request 11k beacon measure responses from the clients.
        * Implement 11k beacon request.
    * 802.11v support in the driver to request steering.
        * Implement 11v steering request.
    * The better supported the BWL is, the more data available there would be for optimal path to decide.
