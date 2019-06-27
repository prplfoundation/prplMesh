# Docker runner image

This folder includes a Dockerfile and helper scripts which can be used to run prplMesh inside containers.
It relies on an already built prplMesh by mapping the `build/install` directory to the container.

It includes 2 helper scripts - one to generate the image `image-build.sh` and one to actually run `maptools.py build` from the container - `run.sh`.

## Usage

Using this Dockerfile, you can run 2 containers, one with a controller+agent, the
other with an agent only.
The 2 containers are connected using a docker network (bridge), so can
communicate + sniffed by running wireshark / tcpdump on the bridge from the host to see 1905
packets.

### Prerequisites

build prplMesh which will be used by the containers (set cmake flag `SHARED_SERVER=OFF`)
(build/install directory will be the only directory mapped to the
containers). 

```bash
./maptools.py build map -f MSGLIB=zmq SHARED_SERVER=OFF BUILD_TESTS=ON CMAKE_BUILD_TYPE=Debug
```

Next, build the docker image using `./image-build.sh`.

Then, create a docker network (bridge) `sudo docker network create
prplMesh-net`. Write down the bridge IP address (using `ip a` or
`sudo docker network inspect prplMesh-net`) - we will need it for the next step.

### Run the Containers

Finally, run 2 containers, each with a different IP address in the docker
network subnet. So for example, if the subnet is 172.20.0.0/16 and the
docker bridge in the host is 172.20.0.1:

#### Run Agent+Controller container

```bash
./run.sh 172.20.0.2 //enter container shell
sudo ./scripts/prplmesh_utils.sh start -s
tail -F /tmp/beerocks/logs/beerocks_controller.log
```

#### Run Agent container

```bash
./run.sh 172.20.0.3
sudo ./scripts/prplmesh_utils.sh start -s -m A
tail -F /tmp/beerocks/logs/beerocks_agent.log
```

---

Proxy settings - the recommended way to work with docker behind a proxy is by [configuring the docker client to pass proxy information to containers automatically](https://docs.docker.com/network/proxy/).

---
