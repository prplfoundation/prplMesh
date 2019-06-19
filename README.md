# prplMesh

TODO describe this project

## Requirements

To build prplMesh, you need (on Ubuntu) the following packages:
```
sudo apt install curl gcc cmake binutils git autoconf autogen libtool pkg-config \
    libreadline-dev libncurses-dev libssl-dev libjson-c-dev libnl-genl-3-dev libzmq3-dev \
     python python-yaml python-paramiko
```

If you haven't done so already, set up your git configuration:
```
git config --global user.email your@email.address
git config --global user.name "Your Name"
```

There are several dependencies for which we require a specific version. To ease
deployment of these, they are collected in a google repo manifest file:
```
mkdir -p ~/bin
curl https://storage.googleapis.com/git-repo-downloads/repo > ~/bin/repo
chmod +x ~/bin/repo
export PATH=$PATH:${HOME}/bin

mkdir ~/prplMesh
cd ~/prplMesh
repo init -u https://github.com/prplfoundation/prplMesh-manifest.git
repo sync
repo forall -p -c 'git checkout $REPO_RREV'
```

## Build Instructions
Each component can be built with CMAKE, or use the [tools/maptools.py](tools/README.md) build command.

To build everything the first time, with all features enabled, run
```
cd prplMesh/tools
./maptools.py build all -n -f MSGLIB=zmq BWL_TYPE=DWPAL BUILD_TESTS=ON
```

Subsequent builds don't need to repeat all of these options. Also, you don't
need to re-build the dependencies when you're developing prplMesh itself.
```
./maptools.py build map
```

Run `./maptools.py build --help` for more options.

