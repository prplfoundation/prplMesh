#!/bin/sh

archive=${1-/tmp/multiap_deploy.tar.gz}
path=${2-/}

echo "$0: RDKB Deploy APM"
echo "$0: Stop APM"
/opt/beerocks/apm_utils.sh stop
echo "$0: Deploy $archive to $path..."
tar -C $path -xzvf $archive
rm -rf $archive
echo "$0: Start APM"
/opt/beerocks/apm_utils.sh start

echo "$0: Done"