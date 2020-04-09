###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

import re
from typing import Dict

import environment as env

'''Regular expression to match a MAC address in a bytes string.'''
RE_MAC = rb"(?P<mac>([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})"


class MapVap:
    '''Represents a VAP in the connection map.'''

    def __init__(self, bssid: str, ssid: bytes):
        self.bssid = bssid
        self.ssid = ssid


class MapRadio:
    '''Represents a radio in the connection map.'''

    def __init__(self, uid: str):
        self.uid = uid
        self.vaps = {}

    def add_vap(self, bssid: str, ssid: bytes):
        vap = MapVap(bssid, ssid)
        self.vaps[bssid] = vap
        return vap


class MapDevice:
    '''Represents a device in the connection map.'''

    def __init__(self, mac: str):
        self.mac = mac
        self.radios = {}

    def add_radio(self, uid: str):
        radio = MapRadio(uid)
        self.radios[uid] = radio
        return radio


def get_conn_map() -> Dict[str, MapDevice]:
    '''Get the connection map from the controller.'''
    conn_map = {}
    for line in env.beerocks_cli_command("bml_conn_map").split(b'\n'):
        # TODO we need to parse indentation to get the exact topology.
        # For the time being, just parse the repeaters.
        bridge = re.search(rb' {8}IRE_BRIDGE: .* mac: ' + RE_MAC, line)
        radio = re.match(rb' {16}RADIO: .* mac: ' + RE_MAC, line)
        vap = re.match(rb' {20}fVAP.* bssid: ' + RE_MAC + rb', ssid: (?P<ssid>.*)$', line)
        if bridge:
            cur_agent = MapDevice(bridge.group('mac').decode('utf-8'))
            conn_map[cur_agent.mac] = cur_agent
        elif radio:
            cur_radio = cur_agent.add_radio(radio.group('mac').decode('utf-8'))
        elif vap:
            cur_radio.add_vap(vap.group('mac').decode('utf-8'), vap.group('ssid'))
    return conn_map
