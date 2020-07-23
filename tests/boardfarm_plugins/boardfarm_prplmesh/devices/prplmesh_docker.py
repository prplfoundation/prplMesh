# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

import os
import time
import boardfarm

from .prplmesh_base import PrplMeshBase
from environment import ALEntityDocker, _get_bridge_interface
from sniffer import Sniffer

rootdir = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../../..'))


class PrplMeshDocker(PrplMeshBase):
    """Dockerized prplMesh device."""

    model = ("prplmesh_docker")
    agent_entity = None
    controller_entity = None

    def __init__(self, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs

        config = kwargs.get("config", kwargs)

        skip_init = False
        try:
            if config["skip-init"] == "True":
                skip_init = True
        except (KeyError, ValueError):
            pass

        # List of device's consoles test can interact with
        self.consoles = [self]

        # Getting unic ID to distinguish devices and network they belong to
        self.unique_id = os.getenv("SUDO_USER", os.getenv("USER", ""))

        self.name = config.get("name", "prplmesh_docker")
        self.docker_name = "-".join((config.get("name", "prplmesh_docker"), self.unique_id))
        self.role = config.get("role", "agent")
        self.cleanup_cmd = config.get("cleanup_cmd", None)
        self.conn_cmd = config.get("conn_cmd", None)
        self.delay = config.get("delay", 7)
        self.docker_network = config.get("docker_network",
                                         "prplMesh-net-{}".format(self.unique_id))

        docker_cmd = os.path.join(rootdir, "tools", "docker", "run.sh")
        docker_args = ["--verbose", "--detach", "--force", "--name", self.docker_name,
                       "--network", self.docker_network, "--expose", "8002"]

        if self.role == "controller":
            # Spawn dockerized controller
            docker_args.append("start-controller-agent")
            if not skip_init:
                self._run_shell_cmd(docker_cmd, docker_args)
                time.sleep(self.delay)
            else:
                print("Skipping init for {}".format(self.name))

            self.controller_entity = ALEntityDocker(self.docker_name,
                                                    device=self, is_controller=True)
        else:
            # Spawn dockerized agent
            docker_args.append("start-agent")
            if not skip_init:
                self._run_shell_cmd(docker_cmd, docker_args)
                time.sleep(self.delay)
            else:
                print("Skipping init for {}".format(self.name))

            self.agent_entity = ALEntityDocker(self.docker_name,
                                               device=self, is_controller=False)

        self.wired_sniffer = Sniffer(_get_bridge_interface(self.docker_network),
                                     boardfarm.config.output_dir)
        self.check_status()

    def __del__(self):
        if self.cleanup_cmd:
            self._run_shell_cmd(self.cleanup_cmd)

    def check_status(self):
        """Method required by boardfarm.

        It is used by boardfarm to indicate that spawned device instance is ready for test
        and also after test - to insure that device still operational.
        """
        self._run_shell_cmd("printf",
                            ["device_get_info", "|", "nc", "-w", "1", "controller-rme", "8002"])

    def isalive(self):
        """Method required by boardfarm.

        States that device is operational and its consoles are accessible.
        """
        return True

    def prprlmesh_status_check(self) -> bool:
        """Check prplMesh status by executing status command to initd service.
        Return True if operational.
        """
        self.check_status()
        return True
