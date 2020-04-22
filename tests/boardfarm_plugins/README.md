<!--
SPDX-License-Identifier: BSD-2-Clause-Patent
Copyright (c) 2020 the prplMesh contributors
This code is subject to the terms of the BSD+Patent license.
See LICENSE file for more details.
-->

### Prerequisites

To obtain prplMesh and boardfarm sources a Google repo tool is used.
Refer to its [README](https://gerrit.googlesource.com/git-repo/+/refs/heads/master/README.md) for detailed info.

### Boardfarm installation

1. To obtain sources using previously mentioned repo tool run:
```
repo init -u https://github.com/prplfoundation/prplMesh-manifest.git
repo sync
repo forall -p -c 'git checkout $REPO_RREV' # (optional)
```

2. Install prplMesh to your host using instruction provided by its [README](https://github.com/prplfoundation/prplMesh/blob/master/README.md). 

3. Install additional python packages:
```
cd boardfarm
pip3 install -r requirements.txt
```

4. Install boardfarm to your host(optional):
```
python3 setup.py install --user
```
If you consider not to install boardfarm to your host and use binary provided in sources, then you may encounter additional warnings in boardfarm log caused by files not included in instalation package.

### Boardfarm plugin dir structure:
```
boardfarm_*     <--- "boardfarm_" prefix is mandatory for boardfarm plugin name
    |
    +- __init__.py
    +- boardfarm_config.json
    +- devices
    |   |
    |   +- __init__.py
    |   +- sample_device_0.py
    |   +- sample_device_1.py
    +- tests
    |   |
    |   +- __init__.py
    |   +- sample_test_0.py
    |   +- sample_test_1.py
    |   +- ...
    +- testsuites.cfg
```

### Custom device description
If the tested device is abcent in `boardfarm/boardfarm/devices` dir we can specify new one by creating `custom_config.json` file sort of:
```json
{
    "prplmesh_dummy": {
        "name": "prplMesh-controller",
        "board_type": "prplmesh_docker",
        "role": "controller",
        "delay": 7,
        "conn_cmd": "",
        "connection_type": "local_cmd",
        "devices": [
            {
                "name": "repeater1",
                "type": "prplMesh_dut",
                "role": "agent",
                "delay": 7,
                "conn_cmd": "",
                "radios": [
                    {
                        "type": "radio_dummy",
                        "iface": "wlan0"
                    },
                    {
                        "type": "radio_dummy",
                        "iface": "wlan2"
                    }
                ]
            },
            {
                "name": "repeater2",
                "type": "prplMesh_dut",
                "role": "agent",
                "delay": 7,
                "conn_cmd": "",
                "radios": [
                    {
                        "type": "radio_dummy",
                        "iface": "wlan0"
                    },
                    {
                        "type": "radio_dummy",
                        "iface": "wlan2"
                    }
                ]
            }
        ]
    }
}

```
Mandatory fields for parent device are:
* `name` is any unique name at all
* `board_type` is a name of existing device class
* `devices` is a list of devices that will be added, must be present even it's empty(i.e. `"devices": []`).
Mandatory fields for child devices(those are listed in **devices** section of parent one):
* `name` is any unique name at all
* `type` is a name of existing device class

When running boardfarm we can specify that custom config with in following manner:
```
-c /path/to/custom_config.json
```

### Devices
There are several devices already existing in `<boardfarm-repo-root>/boardfarm/devices`, but if we created `custom_config.json` which uses previously unknown device, then we must implement its class in plugin dir `boardfarm_*/devices/<device-name>.py`.
As template for the device we can use `<boardfarm-repo-root>/devices/base.py`.

### Test suites
Custom test suite should be present in root of boardfarm plugin dir and its name must match `testsuites*.cfg`.
Suite names are specified in brackets [], test names must match test classes names defined in the files at the "boardfarm_*/tests/" directory.
Sample contents:
```
[suite-name]
TestCaseName0

[another-suite-name]
TestCaseName0
TestCaseName1
TestCaseName2
```

### Test file contents:
Each test must be presented as class, for test file template please refer to:
`boardfarm/tests/bft_base_test.py`
Below you can see mandatory class members:
```
class SampleTest(BftBaseTest):
    def runTest(self):
        '''Main test logic must be implemented here.'''
        print('{}: Running...'.format(self.__class__.__name__))

    def skipTest(self):
        '''Method invoked if DUT doesn't respond.

        If the DUT's consoles don't respond, generally the device is considered as 'dead'.
        Here we can provide info about possible reasons and suggestions.
        '''
        print('{}: Skipping...'.format(self.__class__.__name__))

    def recover(self):
        '''Method invoked if test fails with an unexpected exception type.

        According to boardfarm sources, it is considered as deprecated,
        but it is suggested to leave it as a stub to prevent possible faults.
        '''
        print('{}: Recovering...'.format(self.__class__.__name__)

    @classmethod
    def teardown_class(cls):
        '''Teardown method, optional for boardfarm tests.'''
        print('{}: Teardown...'.format(cls.__class__.__name__))
```
For operational example please refer to **boardfarm_prplmesh** plugin.

### Boardfarm usage

To run boardfarm sample you can use following command:
```
PYTHONPATH=/path/to/module/dir bft -c /path/to/boardfarm_config.json --testsuite <test-suite-name> -n <device-name>
```
or invoke provided script:
```
/bin/bash run_bf.sh
```
It will lauch *boardfarm_prplmesh* plugin.
By default, all test logs will be stored in **results** dir.
