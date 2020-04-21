# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

from typing import Union

import environment as env


class Utils(object):
    """Collection of common functions used by tests."""

    @staticmethod
    def check_log(entity_or_radio: Union[env.ALEntity, env.Radio], regex: str,
                  start_line: int = 0, timeout: float = 0.3) -> bool:
        result, line, match = entity_or_radio.wait_for_log(regex, start_line, timeout)
        if not result:
            raise Exception
        return result, line, match
