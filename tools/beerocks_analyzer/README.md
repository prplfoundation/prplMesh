# beerocks analyzer

Warning: there still is an enormous list of issues with `beerocks_analyzer`, and only the connection map currently works.
This README will walk you through installing dependencies and displaying the connection map.


## Prerequisites

You need to install a few python packages before using it.
This readme will walk you through installing them in a `virtualenv`.
Using a virtualenv allows to install the dependencies in a local directory, which avoids messing with the local Python installation.

You only need to create the `venv` once. Later on you just need to activate the `venv` to be able to use the application.


### Create a virtualenv to use the application

```sh
python3 -m venv analyzer-venv
```

1.  Activate the environment

```sh
source analyzer-venv/bin/activate
```

Your prompt should now have "analyzer-venv" appended to it.
You can deactivate it by running `deactivate`.

2.  Install all the required package inside the virtualenv

```sh
pip install -r requirements.txt
```


## Running the application

First make sure you activate the `venv`, or that you installed the required packages.

The analyzer currently connects to a running controller, then from there opens a socket back to the analyzer.
This means that you need to have access to the controller (locally, via docker, or via ssh) but that the controller also needs to be able to access the device on which the analyzer is running.


### Using a controller running locally

```sh
./beerocks_analyzer.py -map
```


### Using a controller in a local docker container

You need to give the container name:

```sh
DOCKER_CONTAINER="gateway"
./beerocks_analyzer.py -map -docker_container="$DOCKER_CONTAINER"
```


### Using a remote controller over SSH

You first need to have a private key (one that can connect to the remote device) allowed to connect to the remote device.

If you private key is password-protected, you first need to register it in your local agent, so that you don't get asked for a password.
To register your key, just do `ssh-add`.
If you get "Error connecting to agent", you probably have no ssh-agent running and can solve it by doing `` eval `ssh-agent -s` ``.

To run the analyzer, you need to give the IP of the gateway:

```sh
GW_IP=192.168.1.1
./beerocks_analyzer.py -map -gw_ip="$GW_IP"
```

Note that currently, the binary path is hard-coded to `/opt/beerocks/bin/beerocks_cli`.


## Troubleshooting


### `Path to beerocks_cli not found`

If you get `ValueError: Path to beerocks_cli not found`, it means the analyzer couldn't find the path to beerocks_cli's binary.
You can try to set it to the correct location yourself by providing the `-bin_path` option on the command line.
