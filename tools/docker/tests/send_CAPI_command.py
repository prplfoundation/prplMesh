#!/usr/bin/env python3
import argparse
import logging
import socket
from enum import Enum
from typing import Union

logger = logging.getLogger(__name__)


class CAPIReply(Enum):
    """An enum representing the possible CAPI replies."""
    RUNNING = "status,RUNNING"
    COMPLETE = "status,COMPLETE"  # parameters may follow
    INVALID = "status,INVALID"
    ERROR = "status,ERROR"


class UCCSocket:
    """Abstraction of the target listening socket.

    It connects to the listener and it sends and receives
    CAPI commands from it.
    """

    def __init__(self, host: str, port: int):
        """Constructor for UCCSocket

        Parameters
        ----------
        host: str
            The host to connect to. Can either be an ip or a hostname.
        port: str
            The port to connect to.
        """
        self.host = host
        self.port = port
        self.conn: socket.socket

    def __enter__(self):
        self.conn = socket.create_connection((self.host, self.port))
        self.conn.settimeout(None)
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.conn.close()

    def send_cmd(self, command: str) -> None:
        """Send a new CAPI command to the device.
        If previous replies from the server were available, they will be discarded.

        Parameters
        ----------
        command : str
            The command to send. If it does not end with a newline, a new line will be appended.
        """
        if command[-1] != "\n":
            command += "\n"
        self.conn.send(command.encode("utf-8"))

    def get_reply(self) -> str:
        """Wait until the server replies with a `CAPIReply` message other than `CAPIReply.RUNNING`.

        The replies from the server will be printed as they are received.
        Note that this method only returns once a `CAPIReply.COMPLETE`, `CAPIReply.INVALID`,
        or `CAPIReply.ERROR` message has been received from the server.

        Returns
        -------
        str
            The reply from the device.
        """
        data = bytearray()
        while True:
            # resetting data to the next line:
            data = data[data.find(b"\n") + 1:]
            while b"\n" not in data:
                # reading until there is a newline
                data.extend(self.conn.recv(256))
            replies = data.decode("utf-8").split("\n")
            for r in replies:
                if not r:
                    pass  # server replied with an empty line
                elif CAPIReply.RUNNING.value in r:
                    print(r)
                elif CAPIReply.COMPLETE.value in r:
                    print(r)
                    return r
                elif CAPIReply.INVALID.value in r or CAPIReply.ERROR.value in r:
                    raise ValueError("Server replied with {}".format(r))
                else:
                    raise ValueError("Received an unknown reply from the server:\n {}".format(r))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Simulated UCC")
    parser.add_argument("host", help="The device hostname or IP.", type=str)
    parser.add_argument("port", help="The listening port on the device.", type=int)
    parser.add_argument("command", help="The CAPI command to send.")
    args = parser.parse_args()
    socket.gethostbyname(args.host)

    with UCCSocket(args.host, args.port) as sock:
        sock.send_cmd(args.command)
        sock.get_reply()
