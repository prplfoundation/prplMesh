# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

from .prplmesh_base_test import PrplMeshBaseTest


class InitialApConfig(PrplMeshBaseTest):
    """Check initial configuration on device."""

    def runTest(self):
        # Locate test participants
        agent = self.dev.DUT.agent_entity

        self.dev.DUT.wired_sniffer.start(self.__class__.__name__ + "-" + self.dev.DUT.name)

        self.prplmesh_status_check(agent)
        self.check_log(agent.radios[0],
                       r"\(WSC M2 Encrypted Settings\)")
        self.check_log(agent.radios[1],
                       r"\(WSC M2 Encrypted Settings\)")
        self.check_log(agent.radios[0],
                       r"WSC Global authentication success")
        self.check_log(agent.radios[1],
                       r"WSC Global authentication success")
        self.check_log(agent.radios[0],
                       r"KWA \(Key Wrap Auth\) success")
        self.check_log(agent.radios[1],
                       r"KWA \(Key Wrap Auth\) success")

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
