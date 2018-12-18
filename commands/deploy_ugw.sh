#!/bin/sh

archive=${1-/tmp/multiap/deploy/multiap_deploy.tar.gz}

echo "$0: UGW Deploy mAP"
echo "$0: Kill mAP controller & Agent"
/opt/beerocks/beerocks_utils.sh killall_forced
echo "$0: Stop mAP Framework"
/etc/init.d/multiap_init.sh stop

echo "$0: Deploy $archive ..."
tar -C / -xzvf $archive
rm -rf $archive

echo "$0: Start mAP Framework"
/etc/init.d/multiap_init.sh start

echo "$0: Start beerocks"
/opt/beerocks/beerocks_utils.sh start_wd

echo "$0: Done"