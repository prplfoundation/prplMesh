###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

import time

from .prplmesh_base_test import PrplMeshBaseTest
# TODO: Remove as soon, as test works for prplWRT device
from ..devices.prplmesh_prplwrt import PrplMeshPrplWRT
from capi import tlv
from opts import debug


class ClientAssociationLinkMetrics(PrplMeshBaseTest):
    ''' This test verifies that a MAUT with an associated STA responds to
    an Associated STA Link Metrics Query message with an Associated STA Link Metrics
    Response message containing an Associated STA Link Metrics TLV for the associated STA.'''

    def runTest(self):
        # Locate test participants
        agent = self.dev.DUT.agent_entity
        controller = self.dev.wan.controller_entity
        sta = self.dev.wifi

        # This test doesn't work for real HW.
        # Skip it for prplWRT device.
        # TODO: Remove as soon, as test works for prplWRT device
        if self.dev.DUT is PrplMeshPrplWRT:
            self.skipTest("This test isn't ready for prplWRT devices")

        # Regression check
        # Don't connect nonexistent Station
        self.dev.DUT.wired_sniffer.start(self.__class__.__name__ + "-" + self.dev.DUT.name)
        sta_mac = "11:11:33:44:55:66"
        debug("Send link metrics query for unconnected STA")
        controller.ucc_socket.dev_send_1905(agent.mac, 0x800D,
                                            tlv(0x95, 0x0006, '{sta_mac}'.format(sta_mac=sta_mac)))
        self.check_log(agent,
                       "client with mac address {sta_mac} not found".format(sta_mac=sta_mac),
                       timeout=30)
        time.sleep(1)

        debug('sta: {}'.format(sta.mac))
        sta.wifi_connect(agent.radios[0].vaps[0])

        time.sleep(1)

        mid = controller.ucc_socket.dev_send_1905(agent.mac, 0x800D,
                                                  tlv(0x95, 0x0006,
                                                      '{sta_mac}'.format(sta_mac=sta.mac)))
        time.sleep(1)
        self.check_log(agent,
                       "Send AssociatedStaLinkMetrics to controller, mid = {}".format(mid),
                       timeout=30)
        sta.wifi_disconnect(agent.radios[0].vaps[0])

    @classmethod
    def teardown_class(cls):
        """Teardown method, optional for boardfarm tests."""
        test = cls.test_obj
        print("Sniffer - stop")
        test.dev.DUT.wired_sniffer.stop()
        # Send additional Ctrl+C to the device to terminate "tail -f"
        # Which is used to read log from device. Required only for tests on HW
        try:
            test.dev.DUT.agent_entity.device.send('\003')
        except AttributeError:
            # If AttributeError was raised - we are dealing with dummy devices.
            # We don't have to additionaly send Ctrl+C for dummy devices.
            pass
