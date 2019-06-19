# prplMesh manifest
Deploy all prplMesh repos in a single tree using google repo tool

## Requirements
Download & install google repo - https://source.android.com/setup/build/downloading

On Ubuntu, the following packages need to be installed:
```
sudo apt install -y cmake automake libtool libreadline-dev libnl-3-dev \
    autoconf pip busybox bridge-utils wireshark
```


## Initialize & sync
Optional - to avoid ssh keys and password reprompt, add the following to ~/.gitconfig:
```
[url "https://github.com/"]
        insteadOf = git@github.com:

[url "https://github.com/"]
        insteadOf = ssh://git@github.com/

```
Then run the following commands to fetch the code:
```
repo init -u https://github.com/prplfoundation/prplMesh-manifest.git
repo sync
repo forall -p -c 'git checkout $REPO_RREV'
```

## Build Instructions
Each component can be built with CMAKE, or use the [tools/maptools.py](tools/README.md) build command.

Make sure the dependencies of maptools are installed:
```
cd prplMesh/tools
pip install -r requirements.txt
```

Build everything:
```
python maptools.py build -n -f CMAKE_INSTALL_LIBDIR=lib MSGLIB=zmq BWL_TYPE=DWPAL TARGET_PLATFORM=linux all
```

The code can be run in a container, so you can see the communication between the controller and agent on different devices.

Create the container directory:
```
prplMesh/tools/scripts/make-container
```

Start the emulated interfaces on the host:
```
sudo prplMesh/tools/scripts/prplMesh-platform-init.sh start
```

Now start two containers, in two separate terminals:
```
prplMesh/tools/scripts/start-container agent
prplMesh/tools/scripts/start-container controller
```

Then you can start wireshark on the interface br-prplmesh.
