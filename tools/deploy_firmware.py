#!/usr/bin/env python3

import time
import sys
import os
import subprocess
import difflib
import argparse
import shutil
from typing import List

import pexpect
import pexpect.fdpexpect
import pexpect.pxssh
import serial


class PrplwrtDevice:
    """Represents a prplWrt device.

    Offers methods to check if a device needs to be upgraded and to do the actual upgrade.

    It needs to have access to the artifacts of a build job to determine when an upgrade
    is needed (see `artifacts_dir`).
    """

    serial_prompt = "_pexpect_prompt_ "
    """For serial connections we will set this prompt to make it easier to "expect" it."""

    baudrate = 115200
    """The baudrate of the serial connection to the device."""

    def __init__(self, device: str, name: str, image: str, username: str = "root"):
        """

        Parameters
        -----------
        device: str
            The name of the platform (example: netgear-rax40).
        name: str
            The name of the device (it should ne reachable through ssh without a password).
        image: str
            The name of the image that can be used to upgrade the device.
        username: str, optional
            The username to use when connecting to the device over SSH.
        """
        self.device = device
        self.name = name
        self.image = image
        self.username = username

        self.rootdir = os.path.join(os.path.dirname(os.path.realpath(__file__)), "..")
        self.artifacts_dir = os.path.join(self.rootdir, "build/{}".format(self.device))
        """The directory where artifacts are stored. It's expected to contain
        prplwrt-version, and the image file."""

    def set_prompt(self, pexp: pexpect):
        """Set the prompt to `serial_prompt` on a pexpect object."""
        pexp.sendline("export PS1='{}'".format(self.serial_prompt))

    def read_remote_version(self) -> List[str]:
        """Read prplwrt-version on a remote device

        Returns
        -------
        List[str]
            The content of the prplwrt-version file on the device as a list of strings.
        """
        version = None
        with pexpect.pxssh.pxssh(logfile=sys.stdout.buffer) as shell:
            shell.login(self.name, self.username)
            shell.sendline("cat /etc/prplwrt-version")
            shell.expect("cat /etc/prplwrt-version")
            shell.prompt()
            # remove the first (empty) line:
            version = shell.before.decode().splitlines()[1:]
        return version

    def read_artifacts_dir_version(self) -> List[str]:
        """Reads the local prplwrt-version.

        The version file is read from the artifacts (see `artifacts_dir`).

        Returns
        -------
        List[str]
            The content of the local prplwrt-version as a list of strings.
        """
        with open(os.path.join(self.artifacts_dir, "prplwrt-version")) as version_file:
            return version_file.read().splitlines()

    def needs_upgrade(self):
        """Check if a device needs to be updated

        The check is done by comparing prplwrt-version on the target
        with a local one (see `read_artifacts_dir_version).
        """
        artifacts_dir_version = self.read_artifacts_dir_version()
        remote_version = self.read_remote_version()
        diff = difflib.unified_diff(artifacts_dir_version, remote_version,
                                    fromfile='artifacts', tofile='device')
        diff_str = '\n'.join(diff)
        if diff_str:
            print(diff_str)
        return bool(diff_str)

    def reach(self, attempts: int = 5, wait: int = 5):
        """Check if the device is reachable via SSH (and optionally try multiple times).

        Parameters
        ----------
        attempts: int, optional
            How many times we try before concluding that the device is unreachable.
        wait: int, optional
            The number of seconds to wait between attempts.

        Returns
        -------
        bool
            True if the device is reachable via SSH, false otherwise.
        """
        for _ in range(attempts):
            try:
                with pexpect.pxssh.pxssh() as shell:
                    shell.login(self.name, self.username, login_timeout=5)
                    return True
            except pexpect.pxssh.ExceptionPxssh:
                print("Waiting for the device to be reachable")
                time.sleep(wait)
        return False

    def sysupgrade(self):
        """Upgrade the device using sysupgrade."""
        raise NotImplementedError

    def upgrade_uboot(self):
        """Upgrade the device through uboot."""
        raise NotImplementedError


class NetgearRax40(PrplwrtDevice):
    """A Netgear rax40.

    At the moment, the rax40 can only be updated through uboot using a tftp server.
    """

    tftp_dir = "/srv/tftp"
    """The root directory of the tftp server. The image will be copied there"""

    uboot_prompt = "GRX500 #"
    """The u-boot prompt on the target."""

    def upgrade_uboot(self):
        """Upgrade the device through u-boot.

        It requires a running tftp server listenning on the IP the
        target device will expect (see `ipaddr` in the uboot environment).
        """
        serial_path = "/dev/{}".format(self.name)
        if not os.path.exists(serial_path):
            raise ValueError("The serial device {} does not exist!\n".format(serial_path)
                             + "Please make sure you have an appropriate udev rule for it.")
        print("Copying image '{}' to '{}'".format(self.image, self.tftp_dir))
        shutil.copy(os.path.join(self.artifacts_dir, self.image), self.tftp_dir)
        print("Image copied to {}.".format(self.tftp_dir))
        with serial.Serial(serial_path, self.baudrate) as ser:
            print("Connecting to serial")
            shell = pexpect.fdpexpect.fdspawn(ser, logfile=sys.stdout.buffer)
            if not shell.isalive():
                raise ValueError("Unable to connect to the serial device!")
            print("Connected")
            # The console might not be active yet:
            shell.sendline("")
            # make the shell prompt appear:
            self.set_prompt(shell)
            shell.expect(self.serial_prompt)
            # reboot:
            shell.sendline("reboot")
            shell.expect(["Hit any key to stop autoboot:",
                          pexpect.EOF, pexpect.TIMEOUT], timeout=120)
            # stop autoboot:
            shell.sendline("")
            shell.expect(self.uboot_prompt)
            # do the actual upgrade:
            shell.sendline("run update_fullimage")
            shell.expect(self.uboot_prompt, timeout=600)
            # reboot:
            shell.sendline("reset")
            shell.expect("Please press Enter to activate this console", timeout=180)
            # activate the console:
            shell.sendline("")
            self.set_prompt(shell)
            shell.expect(self.serial_prompt)
            shell.sendline("exit")
        if not self.reach(attempts=10):
            raise ValueError("The device was not reachable after the upgrade!")


class Generic(PrplwrtDevice):
    """A generic PrplwrtDevice.

    Since upgrading through uboot is generally target-specific, it
    only offers the sysupgrade option.
    """

    def sysupgrade(self):
        serial_path = "/dev/{}".format(self.name)
        if not os.path.exists(serial_path):
            raise ValueError("The serial device {} does not exist!\n".format(serial_path)
                             + "Please make sure you have an appropriate udev rule for it.")
        print("Copying image '{}' to the target".format(self.image))
        try:
            subprocess.check_output(["scp",
                                     "{}/{}".format(self.artifacts_dir, self.image),
                                     "{}:/tmp/{}".format(self.name, self.image)])
        except subprocess.CalledProcessError as exc:
            print("Failed to copy the image to the target:\n{}".format(exc.output))
            raise exc
        with serial.Serial(serial_path, self.baudrate) as ser:
            print("Connecting to serial")
            shell = pexpect.fdpexpect.fdspawn(ser, logfile=sys.stdout.buffer)
            if not shell.isalive():
                raise ValueError("Unable to connect to the serial device!")
            print("Connected")
            # The console might not be active yet:
            shell.sendline("")
            # make the shell prompt appear:
            self.set_prompt(shell)
            shell.expect(self.serial_prompt)
            # Do the upgrade
            shell.sendline("sysupgrade -v /tmp/{}".format(self.image))
            # first give it 30 seconds, and fail early if the upgrade didn't start:
            shell.expect(r"Performing system upgrade\.\.\.", timeout=30)
            # now give it more time to apply the upgrade and reboot:
            shell.expect(r"Rebooting system\.\.\.", timeout=300)
            shell.expect("Please press Enter to activate this console", timeout=180)
            # activate the console:
            shell.sendline("")
            self.set_prompt(shell)
            shell.expect(self.serial_prompt)
            shell.sendline("exit")
        if not self.reach(attempts=10):
            raise ValueError("The device was not reachable after the upgrade!")


def main():
    parser = argparse.ArgumentParser(prog=sys.argv[0],
                                     description="""Update a prplWrt device, either through u-boot
                                     or using sysupgrade, depending on the target device.""")
    parser.add_argument('-d', '--device',
                        help="""Device to upgrade. Currently supported targets are: netgear-rax40
                        glinet-b1300 turris-omnia""", required=True)
    parser.add_argument(
        '-t',
        '--target-name',
        help="Name of the target to upgrade (make sure it's reachable through ssh).", required=True)

    parser.add_argument(
        '-i',
        '--image',
        help="Name of the image to use for the upgrade (should exist in the artifacts folder).",
        required=True)

    args = parser.parse_args()

    if args.device == "netgear-rax40":
        dev = NetgearRax40(args.device, args.target_name, args.image)
    else:
        dev = Generic(args.device, args.target_name, args.image)

    print("Checking if the device is reachable over ssh")
    if not dev.reach():
        raise ValueError("The device {} is not reachable over ssh! check your ssh configuration."
                         .format(dev.name))
    print("Checking if the device needs to be upgraded")
    if dev.needs_upgrade():
        print("The device {} will be upgraded".format(dev.name))
        try:
            dev.sysupgrade()
        except NotImplementedError:
            dev.upgrade_uboot()
        print("Done")
    else:
        print("The device is already using the same version, no upgrade will be done.")


if __name__ == '__main__':
    main()
