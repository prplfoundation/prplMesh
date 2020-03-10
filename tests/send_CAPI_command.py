#!/usr/bin/env python3
import argparse
import logging
import socket
from enum import Enum
from typing import Dict, Tuple

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

    def __init__(self, host: str, port: int, timeout: int = 10):
        """Constructor for UCCSocket

        Parameters
        ----------
        host: str
            The host to connect to. Can either be an ip or a hostname.
        port: str
            The port to connect to.
        timeout: int
            (optional) The timeout for both creating a connection,
            and receiving or sending data.
        """
        self.host = host
        self.port = port
        self.timeout = timeout

    def __enter__(self):
        self.conn = socket.create_connection((self.host, self.port), self.timeout)
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

    def get_reply(self, verbose: bool = False) -> Dict[str, str]:
        """Wait until the server replies with a `CAPIReply` message other than `CAPIReply.RUNNING`.

        The replies from the server will be printed as they are received.
        Note that this method only returns once a `CAPIReply.COMPLETE`, `CAPIReply.INVALID`,
        or `CAPIReply.ERROR` message has been received from the server.

        Parameters
        ----------
        verbose : bool
            If True, print out the valid replies (RUNNING and COMPLETE) as they arrive.

        Returns
        -------
        Dict[str, str]
            A mapping of parameter -> value. The CAPI COMPLETE message is followed by
            parameter,value pairs. These are converted to a dict and returned. If the COMPLETE
            message has no parameters, an empty dict is returned.
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
                    if verbose:
                        print(r)
                elif CAPIReply.COMPLETE.value in r:
                    if verbose:
                        print(r)
                    reply_value_str = r[len(CAPIReply.COMPLETE.value) + 1:].strip()
                    reply_values = reply_value_str.split(',')
                    return {k: v for k, v in zip(reply_values[::2], reply_values[1::2])}
                elif CAPIReply.INVALID.value in r or CAPIReply.ERROR.value in r:
                    raise ValueError("Server replied with {}".format(r))
                else:
                    raise ValueError("Received an unknown reply from the server:\n {}".format(r))

    def cmd_reply(self, command: str, verbose: bool = False) -> Dict[str, str]:
        """Open the connection, send a command and wait for the reply."""
        with self:
            self.send_cmd(command)
            return self.get_reply(verbose)

    def dev_get_parameter(self, parameter: str) -> str:
        """Call dev_get_parameter and return the parameter, or raise KeyError if it is missing."""
        reply = self.cmd_reply("dev_get_parameter,program,map,parameter,{}".format(parameter))
        return reply[parameter]

    def dev_send_1905(self, dest: str, message_type: int, *tlvs: Tuple[int, int, str]) -> int:
        """Call dev_send_1905 to `dest` with CMDU type `message_type` and additional `tlvs`.

        Parameters
        ----------
        dest : str
            The AL-MAC address of the recipient, as a string.

        message_type : int
            The message type of the 1905.1 message to be sent, as an integer.

        tlvs : Tuple[int, int, str]
            A list of TLV descriptions. Each TLV is a 3-tuple (type, length, value). The type and
            length are given as integers. The value is given as a string, formatted according to the
            UCC rules, i.e. with curly braces and hexadecimal numbers.

        Returns
        -------
        The MID of the message, as an integer.
        """
        def format_tlv(tlv_num, tlv_type, tlv_length, tlv_value):
            if tlv_num:
                tlv_num = str(tlv_num)
            else:
                tlv_num = ''
            return "tlv_type{tlv_num},0x{tlv_type:02x}," \
                   "tlv_length{tlv_num},0x{tlv_length:04x}," \
                   "tlv_value{tlv_num},{tlv_value}".format(**locals())

        cmd = "DEV_SEND_1905,DestALid,{dest:s},MessageTypeValue,0x{message_type:04x}"\
            .format(**locals())
        if len(tlvs) > 1:
            formatted_tlvs = [format_tlv(tlv_num + 1, *tlv) for (tlv_num, tlv) in enumerate(tlvs)]
            cmd += ',' + ','.join(formatted_tlvs)
        elif tlvs:
            cmd += ',' + format_tlv('', *tlvs[0])
        return int(self.cmd_reply(cmd)["mid"], base=0)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Simulated UCC")
    parser.add_argument("host", help="The device hostname or IP.", type=str)
    parser.add_argument("port", help="The listening port on the device.", type=int)
    parser.add_argument("command", help="The CAPI command to send.")
    args = parser.parse_args()
    socket.gethostbyname(args.host)

    UCCSocket(args.host, args.port).cmd_reply(args.command, True)
