#!/bin/sh

ps | awk '/wfa_ca-loop.sh/ {if ($0 !~ "awk") system("kill "$1)}'
ps | awk '/wfa_ca-proxy.sh/ {if ($0 !~ "awk") system("kill "$1)}'
killall nc
pm_util -x 0

dirname() {
	full=$1
	file=`basename $full`
	path=${full%%$file}
	[ -z "$path" ] && path=./
	echo $path
}
thispath=`dirname $0`

cp $thispath/wfa_ca-proxy.sh /tmp/
cd /tmp
[ ! -e wfa_ca-pipe ] && mknod wfa_ca-pipe p


cat > wfa_ca-loop.sh << EOF
while true; do nc -l -p 9000 < wfa_ca-pipe  | "./wfa_ca-proxy.sh" > ./wfa_ca-pipe; done &
EOF
chmod +x wfa_ca-loop.sh

./wfa_ca-loop.sh&
