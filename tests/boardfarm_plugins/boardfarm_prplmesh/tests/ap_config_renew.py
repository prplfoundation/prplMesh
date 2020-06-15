###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################
import time

from .prplmesh_base_test import PrplMeshBaseTest
from capi import tlv


class ApConfigRenew(PrplMeshBaseTest):
    """Check initial configuration on device."""

    def runTest(self):
        # Locate agents and controller
        for dev in self.dev:
            if dev.controller_entity:
                controller = dev.controller_entity
            if dev.agent_entity:
                agent = dev.agent_entity

        dev.wired_sniffer.start(self.__class__.__name__ + "-" + dev.name)
        # Regression test: MAC address should be case insensitive
        mac_repeater1_upper = agent.mac.upper()
        controller.ucc_socket.cmd_reply("DEV_RESET_DEFAULT")
        controller.ucc_socket.cmd_reply(
            "DEV_SET_CONFIG,"
            "bss_info1,{} 8x Multi-AP-24G-1 0x0020 0x0008 maprocks1 0 1,"
            "bss_info2,{} 8x Multi-AP-24G-2 0x0020 0x0008 maprocks2 1 0"
            .format(mac_repeater1_upper, agent.mac))
        controller.ucc_socket.dev_send_1905(agent.mac, 0x000A,
                                            tlv(0x01, 0x0006, "{" + controller.mac + "}"),
                                            tlv(0x0F, 0x0001, "{0x00}"),
                                            tlv(0x10, 0x0001, "{0x00}"))

        time.sleep(30)
        self.check_log(agent.radios[0],
                       r"ssid: Multi-AP-24G-1 .*"
                       r"fronthaul: true backhaul: false",
                       timeout=60)
        self.check_log(agent.radios[0],
                       r"ssid: Multi-AP-24G-2 .*"
                       r"fronthaul: false backhaul: true",
                       timeout=60)
        self.check_log(agent.radios[1],
                       r"tear down radio",
                       timeout=60)
        bssid1 = agent.ucc_socket.dev_get_parameter('macaddr',
                                                    ruid='0x' +
                                                    agent.radios[0].mac.replace(':', ''),
                                                    ssid='Multi-AP-24G-1')
        if not bssid1:
            self.fail("repeater1 didn't configure Multi-AP-24G-1")
        # simulate wps onboarding to the backhaul vap
        agent.ucc_socket.start_wps_registration("24G")
        self.check_log(agent.radios[0], r"Start WPS PBC", timeout=60)

    @classmethod
    def teardown_class(cls):
        """Teardown method, optional for boardfarm tests."""
        test = cls.test_obj
        for dev in test.dev:
            if dev.agent_entity:
                print("Sniffer - stop")
                dev.wired_sniffer.stop()
