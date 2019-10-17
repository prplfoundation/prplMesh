This folder contains a snapshot of debug binaries for Gl.Inet B1300
and config files for setting up the devices as a gateway and an extender.

install-stripped.tgz     - binaries to extract into /opt/beerocks on the device
owrt-cfg-gateway.tar.gz  - confg file to load onto gateway device
owrt-cfg-extender.tar.gz - config file to load onto extender device
fw.bin                   - openwrt firware (from master branch as of mid October 2019)

After uploading the config files make sure to reconfigure MAC addresses on the
device wireless interfaces and the LAN bridge. By default the MAC addresses
on the wireless and br-lan interfaces match the Ethernet ones and confuse
the controller.

Example for gateway configuration:
uci set wireless.default_radio0.macaddr="00:01:02:03:04:FC"
uci set wireless.default_radio1.macaddr="00:01:02:03:04:FD"
uci set network.lan.macaddr='00:01:02:03:04:F0'
uci commit
/etc/init.d/network restart

Example for extender configuration (using old UCI config format):
uci set wireless.wifinet0.macaddr="00:01:02:03:04:56"
uci set wireless.wifinet1.macaddr="00:01:02:03:04:57"
uci set network.lan.macaddr='00:01:02:03:04:50'
uci commit
/etc/init.d/network restart
