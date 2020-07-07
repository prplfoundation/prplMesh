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
import signal
import subprocess
import sys
import time

from .prplmesh_base import PrplMeshBase
from boardfarm.devices import connection_decider
from boardfarm.devices.openwrt_router import OpenWrtRouter
from environment import ALEntityPrplWrt, _get_bridge_interface
from ipaddress import IPv4Network, IPv4Address
from sniffer import Sniffer


class PrplMeshPrplWRT(OpenWrtRouter, PrplMeshBase):
    """prplWRT burned device with prplMesh installed."""

    model = "prplWRT"
    prompt = ['root\\@OpenWrt:/#', '/#', '@OpenWrt:/#', "@OpenWrt:~#"]
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
        self.wan_ip = config.get("wan_ip", None)
        self.username = config.get("username", "root")

        self.name = "-".join((config.get("name", "netgear-rax40"), self.unique_id))

        # If no WAN IP is set in config file retrieve IP from docker network set in config
        # X.X.X.245 IP will be selected from docker network
        if not self.wan_ip:
            self.connection = connection_decider.connection(device=self,
                                                            conn_type="local_serial",
                                                            **kwargs)
            self.connection.connect()
            self.consoles = [self]
            self.logfile_read = sys.stdout
            self.wan_network = self.get_docker_subnet()
            self.wan_ip = self.wan_network[+245]
            self.set_iface_ip("br-lan", self.wan_ip, self.wan_network.prefixlen)
            self.close()
            self.kill(signal.SIGTERM)
            # Removal of PID is required by pexpect in order to spawn a new process
            # serial connection should be terminated by 2 commands above
            self.pid = None

        self.wired_sniffer = Sniffer(_get_bridge_interface(self.docker_network),
                                     boardfarm.config.output_dir)
        self.connection = connection_decider.connection(device=self,
                                                        conn_type="ssh",
                                                        conn_cmd="ssh {}@{}".format(
                                                            self.username, self.wan_ip))
        self.connection.connect()
        # Append active connection to the general array for logging
        self.consoles = [self]
        # Point what to log as data read from child process of pexpect
        # Result: boardfarm will log communication in separate file
        self.logfile_read = sys.stdout
        self.add_iface_to_bridge(self.wan_iface, "br-lan")

        if self.role == "controller":
            self.controller_entity = ALEntityPrplWrt(self, is_controller=True)
        else:
            self.agent_entity = ALEntityPrplWrt(self, is_controller=False)
            self.prplmesh_start_agent()

    def _prplMesh_exec(self, mode: str):
        """Send line to prplmesh initd script."""
        self.sendline("/etc/init.d/prplmesh {}".format(mode))

    def _prplmesh_status_poll(self, timeout: int = 120) -> bool:
        """Poll prplMesh status for timeout time.

        Main agent and wlan0, wlan2 radios should be operational.
        Return True if status is operational and timeout not reached.
        """
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            if self.get_prplMesh_status():
                break
            time.sleep(5)
        else:
            return False
        return True

    def check_status(self) -> bool:
        """Check status of device, return bool to indicate state.

        It is used by boardfarm to indicate that spawned device instance is ready for test
        and also after test - to insure that device still operational.
        """
        return True

    def get_prplMesh_status(self) -> bool:
        """ Check prplMesh status. Return True if operational."""
        self.sendline("/etc/init.d/prplmesh status")
        self.expect(
                ["(?P<main_agent>OK) Main agent.+"
                 "(?P<wlan0>OK) wlan0.+"
                 "(?P<wlan2>OK) wlan2", pexpect.TIMEOUT],
                timeout=5)
        if self.match is not pexpect.TIMEOUT:
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

    def get_docker_subnet(self) -> IPv4Network:
        """Get subnet used by docker network."""
        docker_network_inspect_cmd = ('docker', 'network', 'inspect', self.docker_network)
        inspect_raw = subprocess.run(docker_network_inspect_cmd, stdout=subprocess.PIPE)
        if inspect_raw.returncode != 0:
            # Assume network doesn't exist yet. Create it.
            # Raise an exception if it fails (check=True).
            subprocess.run(('docker', 'network', 'create', self.docker_network), check=True,
                           stdout=subprocess.DEVNULL)
            # Inspect again, now raise if it fails (check=True).
            inspect_raw = subprocess.run(docker_network_inspect_cmd, check=True,
                                         stdout=subprocess.PIPE)
        inspect_json = json.loads(inspect_raw.stdout)

        return IPv4Network(inspect_json[0]["IPAM"]["Config"][0]["Subnet"])

    def add_iface_to_bridge(self, iface: str, bridge: str) -> bool:
        """Add specified interface to the specified bridge."""
        ip_command = ("ip link set {} master {}".format(iface, bridge))
        self.sendline(ip_command)
        self.expect(self.prompt, timeout=10)

    def set_iface_ip(self, iface: str, ip: IPv4Address, prefixlen: int) -> bool:
        """Set interface IPv4 address."""
        self.sendline("ip a add {}/{} dev {}".format(ip, prefixlen, iface))
        self.expect(self.prompt, timeout=10)

    def prplmesh_start_agent(self) -> bool:
        """Start prplMesh in certification_mode agent. Return true if done."""
        self._prplMesh_exec("certification_mode agent")
        self.expect(self.prompt)
        return True

    def prprlmesh_status_check(self) -> bool:
        """Check prplMesh status by executing status command to initd service.
        Return True if operational.
        """
        return self._prplmesh_status_poll()
