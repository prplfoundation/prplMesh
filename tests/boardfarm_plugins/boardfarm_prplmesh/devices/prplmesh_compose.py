# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

import os
import time

import boardfarm
from environment import ALEntityDocker, _get_bridge_interface
from .prplmesh_base import PrplMeshBase
from sniffer import Sniffer

rootdir = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../../..'))


class PrplMeshCompose(PrplMeshBase):
    """Dockerized prplMesh device."""

    model = ("prplmesh_compose")
    agent_entity = None
    controller_entity = None

    def __init__(self, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs

        config = kwargs.get("config", kwargs)

        # List of device's consoles test can interact with
        self.consoles = [self]

        # Getting unic ID to distinguish devices and network they belong to
        self.unique_id = os.getenv("SUDO_USER", os.getenv("USER", ""))

        self.name = "-".join((config.get("name", "prplmesh_compose"), self.unique_id))
        print('config.get("name") {}'.format(config.get("name")))
        # self.name = config.get("name")
        self.role = config.get("role", "agent")
        self.cleanup_cmd = config.get("cleanup_cmd", None)
        self.conn_cmd = config.get("conn_cmd", None)
        self.delay = config.get("delay", 7)
        self.docker_network = "boardfarm-ci_default"

        if self.role == "controller":
            self._docker_compose(["-d", "--name", self.name, "controller"],
                                 "run")
            time.sleep(self.delay)
            self.controller_entity = \
                ALEntityDocker(self.name, is_controller=True, compose=True)
        else:
            self._docker_compose(["-d", "--name", self.name, "agent"],
                                 "run")
            time.sleep(self.delay)
            self.agent_entity = \
                    ALEntityDocker(self.name, is_controller=False,
                                   compose=True)

        self.wired_sniffer = Sniffer(_get_bridge_interface(self.docker_network),
                                     boardfarm.config.output_dir)
        self.check_status()

    def _docker_compose(self, args, parameter=None):
        print('_docker_compose: args {}'.format(args))
        yml_path = "tools/docker/boardfarm-ci/docker-compose.yml"
        full_args = ["-f", os.path.join(rootdir, yml_path)]

        if parameter == "run":
            log_path = os.path.join(rootdir, "logs/{}".format(self.name))
            if not os.path.exists(log_path):
                os.mkdir(log_path)
            vol = '{}:/tmp/{}/beerocks/logs'.format(log_path, self.unique_id)
            full_args += ["run", "-v", vol]
            full_args += args

        print('_docker_compose: {}'.format(' '.join(full_args)))
        os.environ['CURRENT_UID'] = '1000:998'
        self._run_shell_cmd("/usr/local/bin/docker-compose", full_args)

    def __del__(self):
        # self._docker_compose(["stop", self.name])
        self._run_shell_cmd("docker", ["stop", self.name])
        self._run_shell_cmd("docker", ["container", "rm", "-f", self.name])

    def check_status(self):
        """Method required by boardfarm.

        It is used by boardfarm to indicate that spawned device instance is ready for test
        and also after test - to insure that device still operational.
        """
        # self._run_shell_cmd(os.path.join(rootdir, "tools", "docker", "test.sh"),
        #                    ["-v", "-n", "controller"])
        pass

    def isalive(self):
        """Method required by boardfarm.

        States that device is operational and its consoles are accessible.

        """
        return True
