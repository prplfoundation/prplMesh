# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

from boardfarm.tests import bft_base_test


class InitialApConfig(bft_base_test.BftBaseTest):
    """PrplMesh sample test case, no actual testing, it just passes."""

    def runTest(self):
        """Main test logic must be implemented here."""

    @classmethod
    def teardown_class(cls):
        """Teardown method, optional for boardfarm tests."""
