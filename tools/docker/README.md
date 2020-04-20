# prplMesh docker support

prplMesh provides docker images and helper scripts for building and running prplMesh using docker.

The `image-build.sh` supports building 2 images:

- **builder image** for building prplMesh using docker
- **runner image** for running prplMesh using docker

Building the images should be done first prior to attempting to run the builder and runner helper scripts, as it fetches the base (FROM) image from the docker hub and generates local images - `prplmesh-builder` and `prplmesh-runner`.

---

If `build.sh` or `run.sh` are called without first calling `image-build.sh`, then the docker image will be built automatically.

---

## Docker post-installation
To remove the need of using `sudo` before each docker command, please refer to the next guide:
https://docs.docker.com/install/linux/linux-postinstall/

Note that if you choose not to follow the guide, you should add `sudo` before each command.

---

## Docker builder

The docker builder image can be used to build prplMesh via `maptools.py build` command running in a prplmesh-build container via `build.sh`.

Build prplMesh in container:

```bash
./build.sh <maptools.py build arguments>
```

## Docker runner

The docker runner image can be used to run prplMesh inside containers using `run.sh`.

The runner docker allows running multiple containers, for example one with a controller+agent, the other with an agent only:
The 2 containers are connected using a docker network (bridge), so can
communicate + sniffed by running `wireshark` / `tcpdump` on the bridge from the host to see 1905 packets.

## Docker all

The all docker allows building and running prplMesh inside containers. It includes all the tools used for building and running prplMesh, and is mainly used for running prplMesh on Windows at the moment.

Build this image:

```bash
cd tools/docker/all
docker image build --tag prplmesh-all .
```

The current use case for using the `all` image is to run prplMesh on a Windows host for connecting to the WiFi TestSuite wts application (UCC).
This is done by running the container (mapping the sources or cloning them from within the container) in priviliged mode on port X which is set in the UCC as the DUT port:

```bash
docker container run -v //c/Users/prplMesh/dev1/:/work/dev1 --privileged -p 5000:5000 --expose 5000 --name gateway -d --user=0:0 --interactive --tty --entrypoint bash --rm prplmesh-windows
```

## Docker tester

A simple wrapper which calls `docker exec <container name> -it <path/to>/build/install/scripts/prplmesh_utils.sh status` which prints the prplMesh operational status of the main and radio agents.

### Prerequisites

Build prplMesh (build/install directory will be the only directory mapped to the
containers).

```bash
./maptools.py build map -f MSGLIB=zmq BUILD_TESTS=ON CMAKE_BUILD_TYPE=Debug
```

### Run the Containers

Finally, run 2 containers - we will run one controller, naming it "controller", and one "agent".
Naming is important to be able to attach to the container easily from other shells (for tailing the logs).

Helper scripts for running controller and agent are provided in the image for running in detached mode - `start-agent`, `start-controller`, `start-controller-agent`.
In this mode, the container entry command is to run the prplMesh controller / agent / controller & agent.
After running, we can connect to the container with a new shell using `docker container exec -it <name> bash`.

#### Run Agent+Controller container (detached)

```bash
# start controller+agent in a new container named 'controller' in detached mode
./run.sh -n controller -d start-controller-agent
# attach the controller container and open new bash shell
docker container exec -it controller bash
# Now we are inside the container and can tail the controller log
tail -F /tmp/beerocks/logs/beerocks_controller.log
```

#### Run Agent container (detached)

```bash
# start agent in a new container named 'agent' in detached mode
./run.sh -n agent -d start-agent
# attach the agent container and open new bash shell
docker container exec -it agent bash
# Now we are inside the container and can tail the agent log
tail -F /tmp/beerocks/logs/beerocks_agent.log
```

#### Running container interactive

If needed, run the container interactively without specifying the daemon to run:

```bash
# start container named <name> and start interactive session using bash (supplied by the image)
./run.sh -n <name>
```

---

**Notes**

1. The script automatically selects an IP based on the network, but its not 100% full proof - it generates a random IP in the network subnet, except for the gateway. If more than one container is created on the same network **they might get the same IP** (very rare but keep in mind)
2. List all running containers `docker container ls`
3. Connect to any running container using `docker container attach <name>`
4. Connect to any running container opening a new shell using `docker container exec -it <name> bash`
5. Kill all containers (running and stopped) `docker rm -f $(docker ps -a -q)` 

---

## Docker cheat-sheet

Show all images - `docker images`

Delete all images - `docker rmi $(docker images -q)`

Delete all containers - `docker rm $(docker ps -a -q)`

Fetch the logs of a specific container - `docker logs CONTAINER`

Proxy - The recommended way to work with docker behind a proxy is by [configuring the docker client to pass proxy information to containers automatically](https://docs.docker.com/network/proxy/).
