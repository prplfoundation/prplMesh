###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2020 Arnout Vandecappelle (Essensium/Mind)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

import json
import os
import platform
import re
import subprocess

from capi import UCCSocket
from opts import debug
import sniffer


class ALEntity:
    '''Abstract representation of a MultiAP device (1905.1 AL Entity).

    Derived classes provide concrete implementations for a specific device (e.g. docker).

    This provides basic information about the entity, e.g. its AL MAC address. How this information
    is retrieved is implementation-specific.

    It also provides an abstract interface to interact with the entity, e.g. for sending CAPI
    commands.

    If a device runs both the agent and the controller, two ALEntities should be created for it,
    with the same MAC address. This is not how it is modeled in 1905.1, but it corresponds to how
    it is implemented in prplMesh and it allows us to have e.g. a separate UCCSocket to controller
    and agent.
    '''
    def __init__(self, mac: str, ucc_socket: UCCSocket, is_controller: bool = False):
        self.mac = mac
        self.ucc_socket = ucc_socket
        self.is_controller = is_controller
        self.radios = []

        # Convenience functions that propagate to ucc_socket
        self.cmd_reply = self.ucc_socket.cmd_reply
        self.dev_get_parameter = self.ucc_socket.dev_get_parameter
        self.dev_send_1905 = self.ucc_socket.dev_send_1905

    def command(self, *command: str) -> bytes:
        '''Run `command` on the device and return its output as bytes.

        Example: command('ip', 'addr') to get IP addresses of all interfaces.
        '''
        raise NotImplementedError("command is not implemented in abstract class ALEntity")


# The following variables are initialized as None, and have to be set when a concrete test
# environment is started.
wired_sniffer = None
controller = None
agents = []


# Concrete implementation with docker

rootdir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
installdir = os.path.join(rootdir, 'build', 'install')


class ALEntityDocker(ALEntity):
    '''Docker implementation of ALEntity.

    The entity is defined from the name of the container, the rest is derived from that.
    '''
    def __init__(self, name: str, is_controller: bool = False):
        self.name = name
        self.bridge_name = 'br-lan'
        self.on_wsl = "microsoft" in platform.uname()[3].lower()

        # First, get the UCC port from the config file
        if is_controller:
            config_file_name = 'beerocks_controller.conf'
        else:
            config_file_name = 'beerocks_agent.conf'
        with open(os.path.join(installdir, 'config', config_file_name)) as config_file:
            ucc_port = \
                re.search(r'ucc_listener_port=(?P<port>[0-9]+)', config_file.read()).group('port')

        # On WSL, connect to the locally exposed container port
        if self.on_wsl:
            published_port_output = subprocess.check_output(
                ["docker", "port", name, ucc_port]).decode('utf-8').split(":")
            device_ip = published_port_output[0]
            ucc_port = int(published_port_output[1])
        else:
            device_ip_output = self.command(
                'ip', '-f', 'inet', 'addr', 'show', self.bridge_name)
            device_ip = re.search(
                r'inet (?P<ip>[0-9.]+)', device_ip_output.decode('utf-8')).group('ip')

        ucc_socket = UCCSocket(device_ip, ucc_port)
        mac = ucc_socket.dev_get_parameter('ALid')

        super().__init__(mac, ucc_socket, is_controller)

    def command(self, *command: str) -> bytes:
        '''Execute `command` in docker container and return its output.'''
        return subprocess.check_output(("docker", "exec", self.name) + command)


def _get_bridge_interface(unique_id):
    '''Use docker network inspect to get the docker bridge interface.'''
    docker_network = 'prplMesh-net-{}'.format(unique_id)
    docker_network_inspect_cmd = ('docker', 'network', 'inspect', docker_network)
    inspect_result = subprocess.run(docker_network_inspect_cmd, stdout=subprocess.PIPE)
    if inspect_result.returncode != 0:
        # Assume network doesn't exist yet. Create it.
        # This is normally done by test_gw_repeater.sh, but we need it earlier to be able to
        # start tcpdump
        # Raise an exception if it fails (check=True).
        subprocess.run(('docker', 'network', 'create', docker_network), check=True,
                       stdout=subprocess.DEVNULL)
        # Inspect again, now raise if it fails (check=True).
        inspect_result = subprocess.run(docker_network_inspect_cmd, check=True,
                                        stdout=subprocess.PIPE)

    inspect = json.loads(inspect_result.stdout)
    prplmesh_net = inspect[0]
    # podman adds a 'plugins' indirection that docker doesn't have.
    if 'plugins' in prplmesh_net:
        bridge = prplmesh_net['plugins'][0]['bridge']
    else:
        # docker doesn't report the interface name of the bridge. So format it based on the ID.
        bridge_id = prplmesh_net['Id']
        bridge = 'br-' + bridge_id[:12]

    return bridge


def launch_environment_docker(unique_id: str, skip_init: bool = False):
    global wired_sniffer
    wired_sniffer = sniffer.Sniffer(_get_bridge_interface(unique_id))

    gateway = 'gateway-' + unique_id
    repeater1 = 'repeater1-' + unique_id
    repeater2 = 'repeater2-' + unique_id

    if not skip_init:
        wired_sniffer.start('init')
        try:
            subprocess.check_call((os.path.join(rootdir, "tests", "test_gw_repeater.sh"),
                                   "-f", "-u", unique_id, "-g", gateway,
                                   "-r", repeater1, "-r", repeater2, "-d", "7"))
        finally:
            wired_sniffer.stop()

    global controller, agents
    controller = ALEntityDocker(gateway, True)
    agents = (ALEntityDocker(repeater1), ALEntityDocker(repeater2))

    debug('controller: {}'.format(controller.mac))
    debug('agent1: {}'.format(agents[0].mac))
    debug('agent2: {}'.format(agents[1].mac))
