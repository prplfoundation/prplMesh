###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

import json
import os
import platform
import re
import subprocess
import time

from capi import UCCSocket
from opts import debug, err
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
    def __init__(self, mac: str, ucc_socket: UCCSocket, installdir: str,
                 is_controller: bool = False):
        self.mac = mac
        self.ucc_socket = ucc_socket
        self.installdir = installdir
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

    def prplmesh_command(self, command: str, *args: str) -> bytes:
        '''Run `command` with "args" on the device and return its output as bytes.

        "command" is relative to the installation directory of prplmesh, e.g. "bin/beerocks_cli".
        '''
        return self.command(os.path.join(self.installdir, command), *args)

    def wait_for_log(self, regex: str, start_line: int, timeout: float) -> bool:
        '''Poll the entity's logfile until it contains "regex" or times out.'''
        raise NotImplementedError("wait_for_log is not implemented in abstract class ALEntity")


class Radio:
    '''Abstract representation of a radio on a MultiAP agent.

    This provides basic information about the radio, e.g. its mac address, and functionality for
    checking its status.
    '''

    def __init__(self, agent: ALEntity, mac: str):
        self.agent = agent
        agent.radios.append(self)
        self.mac = mac
        self.vaps = []

    def wait_for_log(self, regex: str, start_line: int, timeout: float) -> bool:
        '''Poll the radio's logfile until it contains "regex" or times out.'''
        raise NotImplementedError("wait_for_log is not implemented in abstract class Radio")


class Station:
    '''Placeholder for a wireless (fronthaul) station.

    Unlike the other classes, this is not an abstract class. Instead, it is a placeholder that
    represents a station. Handling the station is actually done through the VirtualAP concrete
    implementation.
    '''
    def __init__(self, mac: str):
        self.mac = mac

    __mac_base = 0

    @staticmethod
    def create():
        '''Generate a Station placeholder with a random MAC address.'''
        mac = '51:a1:10:20:{:02x}:{:02x}'.format(int(Station.__mac_base / 256),
                                                 Station.__mac_base % 256)
        Station.__mac_base += 1
        if Station.__mac_base > 256*256:
            Station.__mac_base = 0
        return Station(mac)


class VirtualAP:
    '''Abstract representation of a VAP on a MultiAP Radio.'''
    def __init__(self, radio: Radio, bssid: str):
        self.radio = radio
        radio.vaps.append(self)
        self.bssid = bssid

    def associate(self, sta: Station) -> bool:
        '''Associate "sta" with this VAP.'''
        raise NotImplementedError("associate is not implemented in abstract class VirtualAP")

    def disassociate(self, sta: Station) -> bool:
        '''Disassociate "sta" from this VAP.'''
        raise NotImplementedError("disassociate is not implemented in abstract class VirtualAP")


# The following variables are initialized as None, and have to be set when a concrete test
# environment is started.
wired_sniffer = None
controller = None
agents = []


def beerocks_cli_command(command: str) -> bytes:
    '''Execute `command` beerocks_cli command on the controller and return its output.'''
    debug("Send CLI command " + command)
    res = controller.prplmesh_command("bin/beerocks_cli", "-c", command)
    debug("  Response: " + res.decode('utf-8', errors='replace').strip())
    return res


# Concrete implementation with docker

rootdir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
installdir = os.path.join(rootdir, 'build', 'install')
on_wsl = "microsoft" in platform.uname()[3].lower()


def _docker_wait_for_log(container: str, program: str, regex: str, start_line: int,
                         timeout: float) -> bool:
    logfilename = os.path.join(rootdir, 'logs', container, 'beerocks_{}.log'.format(program))
    # WSL doesn't support symlinks on NTFS, so resolve the symlink manually
    if on_wsl:
        logfilename = os.path.join(
            rootdir, 'logs', container,
            subprocess.check_output(["tail", "-2", logfilename]).decode('utf-8').
            rstrip(' \t\r\n\0'))

    deadline = time.monotonic() + timeout
    try:
        while True:
            with open(logfilename) as logfile:
                for (i, v) in enumerate(logfile.readlines()):
                    if i <= start_line:
                        continue
                    search = re.search(regex, v)
                    if search:
                        debug("Found '{}'\n\tin {}".format(regex, logfilename))
                        return (True, i, search.groups())
            if time.monotonic() < deadline:
                time.sleep(.3)
            else:
                err("'{}'\n\tin log of {} on {} after {}s".format(regex, program, container,
                                                                  timeout))
                return (False, start_line, None)
    except OSError:
        err("Can't read log of {} on {}".format(program, container))
        return (False, start_line, None)


class ALEntityDocker(ALEntity):
    '''Docker implementation of ALEntity.

    The entity is defined from the name of the container, the rest is derived from that.
    '''

    def __init__(self, name: str, is_controller: bool = False):
        self.name = name
        self.bridge_name = 'br-lan'

        # First, get the UCC port from the config file
        if is_controller:
            config_file_name = 'beerocks_controller.conf'
        else:
            config_file_name = 'beerocks_agent.conf'
        with open(os.path.join(installdir, 'config', config_file_name)) as config_file:
            ucc_port = \
                re.search(r'ucc_listener_port=(?P<port>[0-9]+)', config_file.read()).group('port')

        # On WSL, connect to the locally exposed container port
        if on_wsl:
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

        super().__init__(mac, ucc_socket, installdir, is_controller)

        # We always have two radios, wlan0 and wlan2
        RadioDocker(self, "wlan0")
        RadioDocker(self, "wlan2")

    def command(self, *command: str) -> bytes:
        '''Execute `command` in docker container and return its output.'''
        return subprocess.check_output(("docker", "exec", self.name) + command)

    def wait_for_log(self, regex: str, start_line: int, timeout: float) -> bool:
        '''Poll the entity's logfile until it contains "regex" or times out.'''
        program = "controller" if self.is_controller else "agent"
        return _docker_wait_for_log(self.name, program, regex, start_line, timeout)


class RadioDocker(Radio):
    '''Docker implementation of a radio.'''

    def __init__(self, agent: ALEntityDocker, iface_name: str):
        self.iface_name = iface_name
        ip_output = agent.command("ip", "-o",  "link", "list", "dev", self.iface_name).decode()
        mac = re.search(r"link/ether (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})", ip_output).group(1)
        super().__init__(agent, mac)

        # Since dummy bwl always uses the first VAP, in practice we always have a single VAP with
        # the radio MAC as the bssid.
        VirtualAPDocker(self, mac)

    def wait_for_log(self, regex: str, start_line: int, timeout: float) -> bool:
        '''Poll the radio's logfile until it contains "regex" or times out.'''
        program = "agent_" + self.iface_name
        return _docker_wait_for_log(self.agent.name, program, regex, start_line, timeout)

    def send_bwl_event(self, event: str) -> None:
        # The file is only available within the docker container so we need to use an echo command.
        # Inside the container, $USER is set to the username that was used for starting it.
        command = "echo \"{}\" > /tmp/$USER/beerocks/{}/EVENT".format(event, self.iface_name)
        self.agent.command('sh', '-c', command)


class VirtualAPDocker(VirtualAP):
    '''Docker implementation of a VAP.'''
    def __init__(self, radio: RadioDocker, bssid: str):
        super().__init__(radio, bssid)

    def associate(self, sta: Station) -> bool:
        '''Associate "sta" with this VAP.'''
        self.radio.send_bwl_event("EVENT AP-STA-CONNECTED {}".format(sta.mac))

    def disassociate(self, sta: Station) -> bool:
        '''Disassociate "sta" from this VAP.'''
        self.radio.send_bwl_event("EVENT AP-STA-DISCONNECTED {}".format(sta.mac))


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
    debug('agent1 wlan0: {}'.format(agents[0].radios[0].mac))
    debug('agent1 wlan2: {}'.format(agents[0].radios[1].mac))
    debug('agent2: {}'.format(agents[1].mac))
    debug('agent2 wlan0: {}'.format(agents[1].radios[0].mac))
    debug('agent2 wlan2: {}'.format(agents[1].radios[1].mac))
