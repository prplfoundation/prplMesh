# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

from .prplmesh_base_test import PrplMeshBaseTest


class InitialApConfig(PrplMeshBaseTest):
    """Check initial configuration on device."""

    def runTest(self):
        for dev in self.dev:
            if dev.agent_entity:
                dev.wired_sniffer.start(self.__class__.__name__ + "-" + dev.name)

                self.prplmesh_status_check(dev.agent_entity)
                self.check_log(dev.agent_entity.radios[0],
                               r"\(WSC M2 Encrypted Settings\)")
                self.check_log(dev.agent_entity.radios[1],
                               r"\(WSC M2 Encrypted Settings\)")
                self.check_log(dev.agent_entity.radios[0],
                               r"WSC Global authentication success")
                self.check_log(dev.agent_entity.radios[1],
                               r"WSC Global authentication success")
                self.check_log(dev.agent_entity.radios[0],
                               r"KWA \(Key Wrap Auth\) success")
                self.check_log(dev.agent_entity.radios[1],
                               r"KWA \(Key Wrap Auth\) success")

    @classmethod
    def teardown_class(cls):
        """Teardown method, optional for boardfarm tests."""
        test = cls.test_obj
        for dev in test.dev:
            if dev.agent_entity:
                print("Sniffer - stop")
                # Send Ctrl+C to the device to terminate "tail -f"
                # Which is used to read log from device. Required only for tests on HW
                try:
                    dev.agent_entity.device.send('\003')
                except AttributeError:
                    pass
                dev.wired_sniffer.stop()
