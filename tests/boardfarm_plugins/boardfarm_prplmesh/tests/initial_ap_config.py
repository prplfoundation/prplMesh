# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

from boardfarm.tests import bft_base_test
from utils import Utils


class InitialApConfig(bft_base_test.BftBaseTest):
    """Check initial configuration on device."""

    def runTest(self):
        for dev in self.dev:
            if dev.agent_entity:
                dev.wired_sniffer.start(self.__class__.__name__)

                Utils.check_log(dev.agent_entity.radios[0],
                                r"WSC Global authentication success")
                Utils.check_log(dev.agent_entity.radios[1],
                                r"WSC Global authentication success")
                Utils.check_log(dev.agent_entity.radios[0],
                                r"KWA \(Key Wrap Auth\) success")
                Utils.check_log(dev.agent_entity.radios[1],
                                r"KWA \(Key Wrap Auth\) success")
                Utils.check_log(dev.agent_entity.radios[0],
                                r".* Controller configuration \(WSC M2 Encrypted Settings\)")
                Utils.check_log(dev.agent_entity.radios[1],
                                r".* Controller configuration \(WSC M2 Encrypted Settings\)")

                dev.wired_sniffer.stop()

    @classmethod
    def teardown_class(cls):
        """Teardown method, optional for boardfarm tests."""
