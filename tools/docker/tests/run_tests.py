import send_CAPI_command
import unittest
import os.path
import os
import sys
import flows
from flows import *
from enum import Enum

class StaticConfig(flows.BaseConfig):
    base_path = "build/install/config"

    def __init__(self, file_name):
        mypath = os.path.dirname(os.path.abspath(__file__))
        self.rootdir = os.path.abspath(os.path.join(mypath, "../../.."))
        configfile = os.path.join(self.rootdir, self.base_path, file_name)
        lines = []
        with open(configfile, "r") as f:
            lines = f.readlines()
        values = [(k.strip(), v.strip()) for k,v in [line.split("=") for line in lines if "=" in line]]
        self.values = dict(values)

class LocalTestSystem(flows.TestSystem):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.gateway_config = StaticConfig("beerocks_controller.conf")
        self.agent_config = StaticConfig("beerocks_controller.conf")

    def get_ip(self, device:DeviceType):
        docker_name = "gateway" if device == DeviceType.gateway else "agent"
        command = "docker exec -t %s ip -f inet addr show br_lan | grep -Po 'inet \K[\d.]+'" % docker_name
        with os.popen(command) as proc:
            return proc.readline().strip()
        raise RuntimeError("Command execution failed! Command=" + command)

    def get_port(self, device:DeviceType):
        return int(get_config(device)['ucc_listener_port'])

    def get_log(self, device:DeviceType, log:LogType):
        log_fn = log.name + ".log"
        log_path = os.path.join(self.gateway_config.rootdir, "logs", device.name, log_fn)
        with open(log_path) as log_file:
            return ''.join(log_file.readlines())

    def cleanup(self):
        pass
  
if __name__ == '__main__':
    flows.testSystem = LocalTestSystem(verbose=True)
    rv = unittest.main()
    flows.testSystem.cleanup()
    sys.exit(rv)
