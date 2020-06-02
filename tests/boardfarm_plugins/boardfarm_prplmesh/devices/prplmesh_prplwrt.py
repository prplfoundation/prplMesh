###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

import boardfarm
import json
import os
import pexpect
import subprocess
import sys

from .prplmesh_base import PrplMeshBase
from boardfarm.devices import connection_decider
from boardfarm.devices.openwrt_router import OpenWrtRouter
from environment import ALEntityPrplWrt, _get_bridge_interface
from ipaddress import IPv4Network, IPv4Address
from sniffer import Sniffer


class PrplMeshPrplWRT(OpenWrtRouter, PrplMeshBase):
    """prplWRT burned device with prplMesh installed."""

    model = ("prplWRT")
    prompt = ['root\\@OpenWrt:/#', '/#', '@OpenWrt:/#']
    wan_iface = "eth1"
    uboot_eth = "eth0_1"
    linesep = "\r"
    agent_entity = None
    controller_entity = None

    def __init__(self, *args, **kwargs):
        """Initialize device."""
        self.args = args
        self.kwargs = kwargs
        config = kwargs.get("config", kwargs)

        self.unique_id = os.getenv("SUDO_USER", os.getenv("USER", ""))
        self.docker_network = config.get("docker_network",
                                         "prplMesh-net-{}".format(self.unique_id))
        self.role = config.get("role", "agent")
        self.connection_type = config.get("connection_type", None)
        self.conn_cmd = config.get("conn_cmd", None)

        self.connection = connection_decider.connection(device=self,
                                                        conn_type=kwargs['connection_type'],
                                                        **kwargs)
        self.connection.connect()

        self.name = "-".join((config.get("name", "rax40"), self.unique_id))
        self.consoles = [self]
        self.logfile_read = sys.stdout

        self.wan_network = self.get_docker_subnet(self.docker_network)
        self.set_iface_to_bridge(self.wan_iface)
        self.set_iface_ip("br-lan", self.wan_network[+245], self.wan_network.prefixlen)
        self.wired_sniffer = Sniffer(_get_bridge_interface(self.docker_network),
                                     boardfarm.config.output_dir)
        if self.role == "controller":
            self.controller_entity = ALEntityPrplWrt(self, is_controller=True)
        else:
            self.agent_entity = ALEntityPrplWrt(self, is_controller=False)
            self.prplMesh_start_agent()

    def _prplMesh_exec(self, mode: str):
        """Send line to prplmesh initd script."""
        self.sendline("/etc/init.d/prplmesh {}".format(mode))

    def kill_console_at_exit(self):
        """Kill connections on device termination."""
        self.connection.close()

    def check_status(self) -> bool:
        """Check status of device, return bool to indicate state.

        It is used by boardfarm to indicate that spawned device instance is ready for test
        and also after test - to insure that device still operational.
        """
        return True

    def prplMesh_check_state(self) -> bool:
        """ Check prplMesh status. Return True if operational."""
        self.sendline("/etc/init.d/prplmesh status")
        match = self.expect(
                ["operational", "FAIL", pexpect.EOF, pexpect.TIMEOUT],
                timeout=10)
        if match == 0:
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

    def get_docker_subnet(self, docker_network: str) -> IPv4Network:
        """Get subnet used by docker network."""
        docker_network_inspect_cmd = ('docker', 'network', 'inspect', docker_network)
        inspect_raw = subprocess.run(docker_network_inspect_cmd, stdout=subprocess.PIPE)
        if inspect_raw.returncode != 0:
            # Assume network doesn't exist yet. Create it.
            # Raise an exception if it fails (check=True).
            subprocess.run(('docker', 'network', 'create', docker_network), check=True,
                           stdout=subprocess.DEVNULL)
            # Inspect again, now raise if it fails (check=True).
            inspect_raw = subprocess.run(docker_network_inspect_cmd, check=True,
                                         stdout=subprocess.PIPE)
        inspect_json = json.loads(inspect_raw.stdout)

        return IPv4Network(inspect_json[0]["IPAM"]["Config"][0]["Subnet"])

    def set_iface_to_bridge(self, iface: str) -> bool:
        """Add specified interface to the specified bridge."""
        ip_command = ("ip link set {} master br-lan".format(iface))
        self.sendline(ip_command)
        self.expect(self.prompt, timeout=10)

    def set_iface_ip(self, iface: str, ip: IPv4Address, prefixlen :int) -> bool:
        """Set interface IPv4 address."""
        self.sendline("ip a add {}/{} dev {}".format(ip, prefixlen, iface))
        self.expect(self.prompt, timeout=10)

    def prplMesh_start_agent(self) -> bool:
        """Start prplMesh in certification_mode agent. Return true if done."""
        self._prplMesh_exec("certification_mode agent")
        self.expect_exact("CAC timer expired", timeout=120)
        self.expect_exact("device br-lan entered promiscuous mode", timeout=40)
