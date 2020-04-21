# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

import pexpect

from boardfarm.devices import linux


class PrplMeshBase(linux.LinuxDevice):
    """PrplMesh abstract device."""

    def _run_shell_cmd(self, cmd: str = "", args: list = None, timeout: int = 30):
        """Wrapper that executes command with specified args on host machine and logs output."""

        res = pexpect.run(cmd, args=args, timeout=timeout, encoding="utf-8")
        entry = " ".join((cmd, " ".join(args)))
        self.log_calls += entry
        self.log += "$ " + entry + "\r\n" + res

    def check_status(self):
        """Method required by boardfarm.

        It is used by boardfarm to indicate that spawned device instance is ready for test
        and also after test - to insure that device still operational.
        """
        pass

    def close(self):
        """Method required by boardfarm.

        Purpose is to close connection to device's consoles.
        """
        pass

    def isalive(self):
        """Method required by boardfarm.

        States that device is operational and its consoles are accessible.
        """
        pass

    def touch(self):
        """Method required by boardfarm.

        Purpose is to keep consoles active, so they don't disconnect for long running activities.
        """
        pass
