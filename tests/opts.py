###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################


class opts:
    '''Static class that encodes the global options.'''
    verbose = False
    tcpdump = False
    tcpdump_dir = ''
    stop_on_failure = False


def message(msg: str, color: int = 0):
    '''Print a message, optionally in a color.'''
    if color:
        print('\x1b[1;{}m{}\x1b[0m'.format(color, msg))
    else:
        print(msg)


def debug(msg: str):
    '''Print a debug message if verbose is enabled.'''
    if opts.verbose:
        message(msg)


def status(msg: str):
    '''Print a purple status message.'''
    message(msg, 35)


def err(msg: str):
    '''Print a red error message.'''
    message(msg, 31)
