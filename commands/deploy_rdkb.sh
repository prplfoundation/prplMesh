#!/bin/sh

archive=${1-/tmp/multiap_deploy.tar.gz}
path=${2-/}

echo "$0: RDKB Deploy APM"
echo "$0: Stop APM"
/opt/beerocks/apm_utils.sh stop

echo "$0: WiFi Restart"
wifi down #stops hostapd
systemctl stop systemd-wave_init.service #rmmods ko’s
systemctl start systemd-wave_init.service # insmod ko’s, runs recovery daemon
wifi

echo "$0: Deploy $archive to $path..."
tar -C $path -xzvf $archive
rm -rf $archive
echo "$0: Start APM"
/opt/beerocks/apm_utils.sh start

echo "$0: Done"