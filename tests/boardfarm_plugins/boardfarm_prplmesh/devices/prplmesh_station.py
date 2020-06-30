###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

from boardfarm.devices.debian_wifi import DebianWifi
from boardfarm.devices import connection_decider
from environment import VirtualAPHostapd


class PrplMeshStation(DebianWifi):
    """Client of prplMesh enabled Access Point."""

    linesep = "\r"
    model = "prplWRT_STA"
    prompt = ['.*:~', '/.*#']

    def __init__(self, *args, **kwargs):
        """Init station and wlan iface."""
        self.args = args
        self.kwargs = kwargs

        config = kwargs.get("config", kwargs)
        self.connection_type = config.get("connection_type", None)
        self.connection = connection_decider.connection(device=self,
                                                        conn_type=self.connection_type,
                                                        **kwargs)
        self.connection.connect()
        self.consoles = [self]
        super(PrplMeshStation, self).__init__(*args, **kwargs)
        self.iface_dut = self.iface_wifi = self.kwargs.get(
            'iface', 'wlan0')
        self.driver_name = config.get("driver", "nl80211,wext")
        self.mac = self.get_mac()

        self.wifi_disconnect()
        # kill all wpa_supplicant relevant to active interface

    def wifi_connect(self, vap: VirtualAPHostapd) -> bool:
        """Connect to the Access Point. Return True if successful."""
        config_file_name = "boardfarm_tmp.conf"
        config_file_path = "/tmp/{}".format(config_file_name)

        # Create network configuration for SSID
        bssid = "bssid={}".format(vap.bssid)
        ssid = "ssid=\"{}\"".format(vap.get_ssid())
        key = "psk=\"{}\"".format(vap.get_psk())
        network_config = "network={{\n{}\n{}\n{}\n}}".format(bssid, ssid, key)
        # Clean up previous configuration
        self.sendline("rm -f \"{}\"".format(config_file_path))
        self.expect(self.prompt)
        self.sendline("echo \'{}\' > \"{}\"".format(network_config, config_file_path))
        self.expect(self.prompt)
        # Start wpa_supplicant with created configuration
        # Typical coommand on RPI: wpa_supplicant -B -c/tmp/temp.conf -iwlan0 -Dnl80211,wext
        self.sudo_sendline("wpa_supplicant -B -D{} -i {} -c {}".format(
            self.driver_name, self.iface_wifi, config_file_path))
        self.expect("Successfully initialized wpa_supplicant")
        return bool(self.match)

    def get_mac(self) -> str:
        """Get MAC of STA iface"""
        self.sendline("iw {} info".format(self.iface_dut))
        # We are looking for MAC definition of STA
        # wdev 0x1
        # addr 96:4e:c9:cc:7a:2c
        # type managed
        self.expect("addr (?P<mac>..:..:..:..:..:..)\r\n\ttype")
        return self.match.group('mac')
