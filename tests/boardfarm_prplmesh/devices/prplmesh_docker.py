#! /usr/bin/env python3
#
# Work-In-Progress. Non-working code yet
#

from boardfarm.devices import debian

import os
import subprocess
from pprint import pprint
from environment import ALEntriryDocker

# rootdir = os.path.abspath(os.path.join(os.path.dirname(__file__), '../..'))


class PrplMeshDocker(debian.DebianBox):
    '''
    Dockerized prplMesh device containing controller or agent
    '''
    model = ('prplmesh_docker')
    name = "prplmesh_docker"
    ptyproc = None

    def __init__(self, *args, **kwargs):
        name = kwargs.pop("name", "gateway")
        net = kwargs.pop("net", "prplMesh-net")

        if "controller" in kwargs:
            controller_data = kwargs.pop("controller", {})
            ucc_port = controller_data.get("ucc_port", 8002)
            subprocess.check_call((os.path.join(rootdir, "tools", "docker", "run.sh"),
                                   "--verbose", "--detach", "--force", 
                                   "--expose {ucc_port} --publish 127.0.0.1::{ucc_port} --ipaddr 0.0.0.0".format(ucc_port=ucc_port),
                                   "--name", name,
                                   "--mac", "00:11:22:33",
                                   "start-controller-agent")
            self.controller = ALEntityDocker(name, is_controller=True)

        if "agent" in kwargs:
            agent_data = kwargs.pop("agent", {})
            ucc_port = agent_data.get("ucc_port", 8005)
            index = agent_data.get("index", 1)
            subprocess.check_call((os.path.join(rootdir, "tools", "docker", "run.sh"),
                                   "--verbose", "--detach", "--force", 
                                   "--expose {ucc_port} --publish 127.0.0.1::{ucc_port} --ipaddr 0.0.0.0".format(ucc_port=ucc_port),
                                   "--name", name,
                                   "--mac", "aa:bb:cc:{idx}{idx}".format(idx=index),
                                   "start-agent")
            self.agent = ALEntityDocker(name)
