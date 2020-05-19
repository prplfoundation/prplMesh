###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

import boardfarm
from boardfarm.devices import OpenWrtRouter
from environment import ALEntityPrplWrt, _get_bridge_interface
from .prplmesh_base import PrplMeshBase
from sniffer import Sniffer


class PrplMeshPrplWRT(OpenWrtRouter, PrplMeshBase):
    """prplWRT burned device with prplMesh installed."""

    model = "prplWRT"
    prompt = ['root\\@OpenWrt:/#', '/#', '@OpenWrt:/#']
    wan_iface = "eth1"
    uboot_eth = "eth0_1"
    agent_entity = None
    controller_entity = None

    def __init__(self, *args, **kwargs):
        """Initialize device."""
        self.args = args
        self.kwargs = kwargs
        config = kwargs.get("config", kwargs)
        self.docker_network = config.get("docker_network",
                                         "prplMesh-net-{}".format(self.unique_id))
        self.role = config.get("role", "agent")
        if self.role == "controller":
            self.controller_entity = ALEntityPrplWrt(self.name, is_controller=True)
        else:
            self.agent_entity = ALEntityPrplWrt(self.name, is_controller=False)

        self.wired_sniffer = Sniffer(_get_bridge_interface(self.docker_network),
                                     boardfarm.config.output_dir)
        self.check_status()

    def check_status(self) -> bool:
        """Check status of device, return bool to indicate state.

        It is used by boardfarm to indicate that spawned device instance is ready for test
        and also after test - to insure that device still operational.
        """
        self.sendline("/etc/init.d/prplmesh status")
        match = self.expect(
                ["OPERATIONAL", self.device.pexpect.EOF, self.device.pexpect.TIMEOUT],
                timeout=10)
        if match == 1:
            return True
        else:
            return False

    def isalive(self):
        """Check if device is alive.

        States that device is operational and its consoles are accessible.
        """
        return self.check_status()

    def touch(self):
        """Keep consoles alive.

        Purpose is to keep consoles active, so they don't disconnect for long running activities.
        """
        pass
