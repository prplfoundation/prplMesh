Building prplwrt for Nighthawk RAX40

# First execution:

1. Download prplmesh

    git clone https://github.com/prplfoundation/prplMesh.git

2. Enter in openwrt/toolchains directory

    cd prplMesh/tools/docker/builder/openwrt/toolchains

3. Build docker image

    docker build -t prplwrt-build .

4. Put prplwrt-env in /usr/bin/

    mv prplwrt-env /usr/bin/

5. Retrieve prplwrt in new empty folder

    git clone https://git.prpl.dev/prplmesh/prplwrt.git|

6. From new folder run prplwrt-env script

    prplwrt-env bash

7. After entering in docker container shell start building

    make world -j4 V=99

# Next executions:

1. Enter in directory with prplwrt folder
2. Run prplwrt-env script

     prplwrt-env bash

3. After entering in docker container shell start building

     make world -j4 V=99
