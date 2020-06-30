###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

from boardfarm.devices.debian_wifi import DebianWifi
from boardfarm.devices import connection_decider
from environment import VirtualAPHostapd
from .prplmesh_base import PrplMeshBase


class PrplMeshStation(DebianWifi):
    """Client of prplMesh enabled Access Point."""

    linesep = "\r"
    model = "prplWRT_STA"
    prompt = ['.*:~', '/ #']

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

    def wifi_connect(self, vap: VirtualAPHostapd) -> bool:
        """Connect to the Access Point. Return True if successful."""
        return True
