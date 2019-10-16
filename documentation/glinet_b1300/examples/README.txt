This folder contains a snapshot of debug binaries for Gl.Inet B1300
and config files for setting up the devices as a gateway and an extender.

install-stripped.tgz     - binaries to extract into /opt/beerocks on the device
owrt-cfg-gateway.tar.gz  - confg file to load onto gateway device
owrt-cfg-extender.tar.gz - config file to load onto extender device
fw.bin                   - openwrt firware (master as of mid October 2019)

After uploading the config files make sure reconfigure mac addresses on the
device wireless interfaces. By default the MAC addresses on the wireless interfaces
match the Ethernet ones and confuse the controller.

Example for gateway configuration:
uci set wireless.default_radio0.macaddr="00:01:02:03:04:FC"
uci set wireless.default_radio1.macaddr="00:01:02:03:04:FD"
uci commit
wifi reload

Example for extender configuration (it uses old format):
uci set wireless.wifinet0.macaddr="00:01:02:03:04:56"
uci set wireless.wifinet1.macaddr="00:01:02:03:04:57"
uci commit
wifi reload


