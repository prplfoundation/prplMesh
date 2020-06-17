###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

import time

from .prplmesh_base_test import PrplMeshBaseTest
from capi import tlv
from environment import Station
from opts import debug


class ClientAssociationLinkMetrics(PrplMeshBaseTest):
    ''' This test verifies that a MAUT with an associated STA responds to
    an Associated STA Link Metrics Query message with an Associated STA Link Metrics
    Response message containing an Associated STA Link Metrics TLV for the associated STA.'''

    def runTest(self):
        # Locate test participants in array
        for dev in self.dev:
            if dev.controller_entity:
                controller = dev.controller_entity
            if dev.agent_entity:
                agent = dev.agent_entity

        # Regression check
        # Don't connect not existing STAtion
        dev.wired_sniffer.start(self.__class__.__name__ + "-" + dev.name)
        sta_mac = "11:11:33:44:55:66"
        debug("Send link metrics query for unconnected STA")
        controller.ucc_socket.dev_send_1905(agent.mac, 0x800D,
                                            tlv(0x95, 0x0006, '{sta_mac}'.format(sta_mac=sta_mac)))
        self.check_log(agent,
                       "client with mac address {sta_mac} not found".format(sta_mac=sta_mac),
                       timeout=30)
        time.sleep(1)

        sta = Station.create()
        debug('sta: {}'.format(sta.mac))

        agent.radios[0].vaps[0].associate(sta)

        time.sleep(1)

        mid = controller.ucc_socket.dev_send_1905(agent.mac, 0x800D,
                                                  tlv(0x95, 0x0006,
                                                      '{sta_mac}'.format(sta_mac=sta.mac)))
        time.sleep(1)
        self.check_log(agent,
                       "Send AssociatedStaLinkMetrics to controller, mid = {}".format(mid),
                       timeout=30)
        dev.agent_entity.radios[0].vaps[0].disassociate(sta)

    @classmethod
    def teardown_class(cls):
        """Teardown method, optional for boardfarm tests."""
        test = cls.test_obj
        for dev in test.dev:
            if dev.agent_entity:
                print("Sniffer - stop")
                dev.wired_sniffer.stop()
