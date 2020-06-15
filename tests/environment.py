###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

from enum import Enum
import json
import os
import platform
import re
import subprocess
import time
import yaml

from capi import UCCSocket
from collections import namedtuple
from opts import opts, debug, err
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
        self.start_wps_registration = self.ucc_socket.start_wps_registration

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


ChannelInfo = namedtuple("ChannelInfo", "channel bandwidth center_channel")


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

    def get_current_channel(self) -> ChannelInfo:
        '''Get the current channel information.

        Returns
        -------
        ChannelInfo
            3-tuple with the channel information: channel index, bandwidth as an integer, and
            center channel for VHT.
        '''
        raise NotImplementedError("get_current_channel is not implemented in abstract class Radio")


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


class StationEvent(Enum):
    '''An enum representing the possible station events
    based on client association event TLV assoc_event_flags'''
    CONNECT = 0x00000080
    DISCONNECT = 0x00000000


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


def checkpoint() -> None:
    '''Checkpoint the current state.

    Any subsequent calls to functions that query cumulative state (e.g. log files, packet captures)
    will not match any of the state that was accumulated up till now, but only afterwards.

    TODO: Implement for log functions.
    '''
    wired_sniffer.checkpoint()


# Concrete implementation with docker

rootdir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
installdir = os.path.join(rootdir, 'build', 'install')
on_wsl = "microsoft" in platform.uname()[3].lower()


# Temporary workaround
# Since we have multiple log files that correspond to a radio, multiple programs are passed
# as argument. In the log messages, we only use the first one.
# This should be reverted again as part of Unified Agent.
def _docker_wait_for_log(container: str, programs: [str], regex: str, start_line: int,
                         timeout: float) -> bool:
    def logfilename(program):
        logfilename = os.path.join(rootdir, 'logs', container, 'beerocks_{}.log'.format(program))
        # WSL doesn't support symlinks on NTFS, so resolve the symlink manually
        if on_wsl:
            logfilename = os.path.join(
                rootdir, 'logs', container,
                subprocess.check_output(["tail", "-2", logfilename]).decode('utf-8').
                rstrip(' \t\r\n\0'))
        return logfilename

    logfilenames = [logfilename(program) for program in programs]

    deadline = time.monotonic() + timeout
    try:
        while True:
            for logfilename in logfilenames:
                with open(logfilename, 'rb') as logfile:
                    for (i, v) in enumerate(logfile.readlines()):
                        if i <= start_line:
                            continue
                        search = re.search(regex.encode('utf-8'), v)
                        if search:
                            debug("Found '{}'\n\tin {}".format(regex, logfilename))
                            return (True, i, search.groups())
            if time.monotonic() < deadline:
                time.sleep(.3)
            else:
                err("Can't find '{}'\n\tin log of {} on {} after {}s".format(regex, programs[0],
                                                                             container, timeout))
                return (False, start_line, None)
    except OSError:
        err("Can't read log of {} on {}".format(programs[0], container))
        return (False, start_line, None)


def _device_wait_for_log(device: None, log_path: str, regex: str,
                         timeout: int, start_line: int = 0):
    """Waits for log matching regex expression to show up."""
    device.sendline("tail -f -n +{:d} {}".format(start_line + 1, log_path))
    device.expect(regex, timeout=timeout)
    match = device.match.group(0)
    # Send Ctrl-C to interrupt tail -f
    device.send('\003')
    device.expect(device.prompt)
    if match:
        device.sendline("tail -n +{:d} {} | grep -n \"{}\"".format(start_line, log_path, match))
        # Typical output of grep -n from log: "line_num:severity"
        # this regex has to capture just number of line in log
        device.expect(r"(?P<line_number>[0-9]+):[A-Z]+\s[0-9]", timeout=timeout)
        matched_line = int(device.match.group('line_number')) + start_line
        return (True, matched_line, match)
    else:
        return (False, start_line, None)


class ALEntityDocker(ALEntity):
    '''Docker implementation of ALEntity.

    The entity is defined from the name of the container, the rest is derived from that.
    '''
    # NOTE: name arg can be also extracted from the device class itself, but test_flows.py
    # don't have it. We can remove this arg as soon, as we drop test_flows.py
    def __init__(self, name: str, device: None = None, is_controller: bool = False):
        self.name = name
        self.bridge_name = 'br-lan'
        if device:
            self.device = device

        # First, get the UCC port from the config file
        if is_controller:
            config_file_name = 'beerocks_controller.conf'
        else:
            config_file_name = 'beerocks_agent.conf'
        with open(os.path.join(installdir, 'config', config_file_name)) as config_file:
            ucc_port = \
                re.search(r'ucc_listener_port=(?P<port>[0-9]+)',
                          config_file.read()).group('port')

        # On WSL, connect to the locally exposed container port
        if on_wsl:
            published_port_output = subprocess.check_output(
                ["docker", "port", name, ucc_port]).decode('utf-8').split(":")
            device_ip = published_port_output[0]
            ucc_port = int(published_port_output[1])
        else:
            device_ip_output = self.command(
                'ip', '-f', 'inet', 'addr', 'show', self.bridge_name)
            device_ip = re.search(r'inet (?P<ip>[0-9.]+)',
                                  device_ip_output.decode('utf-8')).group('ip')

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
        return _docker_wait_for_log(self.name, [program], regex, start_line, timeout)

    def prprlmesh_status_check(self):
        return self.device.prprlmesh_status_check()


class RadioDocker(Radio):
    '''Docker implementation of a radio.'''
    def __init__(self, agent: ALEntityDocker, iface_name: str):
        self.iface_name = iface_name
        ip_output = agent.command("ip", "-o",  "link", "list", "dev", self.iface_name).decode()
        mac = re.search(r"link/ether (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})",
                        ip_output).group(1)
        super().__init__(agent, mac)

        # Since dummy bwl always uses the first VAP, in practice we always have a single VAP with
        # the radio MAC as the bssid.
        VirtualAPDocker(self, mac)

    def wait_for_log(self, regex: str, start_line: int, timeout: float) -> bool:
        '''Poll the radio's logfile until it contains "regex" or times out.'''
        programs = ("agent_" + self.iface_name, "ap_manager_" + self.iface_name)
        return _docker_wait_for_log(self.agent.name, programs, regex, start_line, timeout)

    def send_bwl_event(self, event: str) -> None:
        # The file is only available within the docker container so we need to use an echo command.
        # Inside the container, $USER is set to the username that was used for starting it.
        command = "echo \"{}\" > /tmp/$USER/beerocks/{}/EVENT".format(event, self.iface_name)
        self.agent.command('sh', '-c', command)

    def read_tmp_file(self, filename: str) -> bytes:
        '''Read the file given by `filename` from the radio's status directory'''
        # The file is only available within the docker container so we need to use cat.
        # Inside the container, $USER is set to the username that was used for starting it.
        # We need to pass through shell to expand it.
        path = "/tmp/$USER/beerocks/{}/{}".format(self.iface_name, filename)
        return self.agent.command("sh", "-c", 'cat "{}"'.format(path))

    def get_current_channel(self) -> ChannelInfo:
        channel_info = yaml.safe_load(self.read_tmp_file("channel"))
        return ChannelInfo(channel_info["channel"], channel_info["bw"],
                           channel_info["center_channel"])


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


def _get_bridge_interface(docker_network):
    '''Use docker network inspect to get the docker bridge interface.'''
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


def launch_environment_docker(unique_id: str, skip_init: bool = False, tag: str = ""):
    global wired_sniffer
    iface = _get_bridge_interface('prplMesh-net-{}'.format(unique_id))
    wired_sniffer = sniffer.Sniffer(iface, opts.tcpdump_dir)

    gateway = 'gateway-' + unique_id
    repeater1 = 'repeater1-' + unique_id
    repeater2 = 'repeater2-' + unique_id

    if not skip_init:
        command = [os.path.join(rootdir, "tests", "test_gw_repeater.sh"), "-f", "-u", unique_id,
                   "-g", gateway, "-r", repeater1, "-r", repeater2, "-d", "7"]
        if tag:
            command += ["-t", tag]
        wired_sniffer.start('init')
        try:
            subprocess.check_call(command)
        finally:
            wired_sniffer.stop()

    global controller, agents
    controller = ALEntityDocker(name=gateway, is_controller=True)
    agents = (ALEntityDocker(name=repeater1), ALEntityDocker(name=repeater2))

    debug('controller: {}'.format(controller.mac))
    debug('agent1: {}'.format(agents[0].mac))
    debug('agent1 wlan0: {}'.format(agents[0].radios[0].mac))
    debug('agent1 wlan2: {}'.format(agents[0].radios[1].mac))
    debug('agent2: {}'.format(agents[1].mac))
    debug('agent2 wlan0: {}'.format(agents[1].radios[0].mac))
    debug('agent2 wlan2: {}'.format(agents[1].radios[1].mac))


class ALEntityPrplWrt(ALEntity):
    """Abstraction of ALEntity in real device."""

    def __init__(self, device: None, is_controller: bool = False):
        self.device = device
        self.name = device.name

        if is_controller:
            self.config_file_name = '/opt/prplmesh/config/beerocks_controller.conf'
        else:
            self.config_file_name = '/opt/prplmesh/config/beerocks_agent.conf'

        ucc_port_raw = self.command("grep \"ucc_listener_port\" {}".format(self.config_file_name))
        ucc_port = int(re.search(r'ucc_listener_port=(?P<port>[0-9]+)',
                                 ucc_port_raw).group('port'))
        bridge_name_raw = self.command("grep \"bridge_iface\" {}".format(self.config_file_name))
        self.bridge_name = re.search(r'bridge_iface=(?P<bridge>.+)\r\n',
                                     bridge_name_raw).group('bridge')
        log_folder_raw = self.command(
            "grep log_files_path {}".format(self.config_file_name))
        self.log_folder = re.search(r'log_files_path=(?P<log_path>[a-zA-Z0-9_\/]+)',
                                    log_folder_raw).group('log_path')
        ucc_socket = UCCSocket(str(self.device.wan_ip), int(ucc_port))
        mac = ucc_socket.dev_get_parameter('ALid')

        super().__init__(mac, ucc_socket, installdir, is_controller)

        # We always have two radios, wlan0 and wlan2
        RadioHostapd(self, "wlan0")
        RadioHostapd(self, "wlan2")

    def command(self, *command: str) -> bytes:
        """Execute `command` in device and return its output."""
        self.device.sendline(" ".join(command))
        self.device.expect(self.device.prompt, timeout=10)
        return self.device.before

    def wait_for_log(self, regex: str, start_line: int, timeout: float) -> bool:
        """Poll the entity's logfile until it contains "regex" or times out."""
        program = "controller" if self.is_controller else "agent"
        # Multiply timeout by 100, as test sets it in float.
        return _device_wait_for_log(self.device,
                                    "{}/beerocks_{}.log".format(self.log_folder, program),
                                    regex, start_line, timeout)

    def prprlmesh_status_check(self):
        return self.device.prprlmesh_status_check()


class RadioHostapd(Radio):
    """Abstraction of real Radio in prplWRT device."""

    def __init__(self, agent: ALEntityPrplWrt, iface_name: str):
        self.iface_name = iface_name
        self.agent = agent
        ip_raw = self.agent.command("ip link list dev {}".format(self.iface_name))
        mac = re.search(r"link/ether (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})",
                        ip_raw).group(1)
        self.log_folder = agent.log_folder
        super().__init__(agent, mac)

        VirtualAPHostapd(self, mac)
        VirtualAPHostapd(self, mac)

    def wait_for_log(self, regex: str, start_line: int, timeout: float):
        ''' Poll the Radio's logfile until it match regular expression '''
        # Multiply timeout by 100, as test sets it in float.
        return _device_wait_for_log(self.agent.device, "{}/beerocks_agent_{}.log".format(
            self.log_folder, self.iface_name), regex, timeout, start_line)


class VirtualAPHostapd(VirtualAP):
    """Docker implementation of a VAP."""

    def __init__(self, radio: RadioHostapd, bssid: str):
        super().__init__(radio, bssid)

    def associate(self, sta: Station) -> bool:
        ''' Associate "sta" with this VAP '''
        # TODO: complete this stub
        return True

    def disassociate(self, sta: Station) -> bool:
        ''' Disassociate "sta" from this VAP.'''
        # TODO: complete this stub
        return True
