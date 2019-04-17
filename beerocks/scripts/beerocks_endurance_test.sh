factcfg() {
        echo factcfg >> /opt/beerocks/factcfg
        factorycfg.sh
}

reboot() {
	echo reboot >> /opt/beerocks/reboot
	/sbin/reboot
}

radio_get_enable() {
		local iface=$1
		local __resvar=$2
		eval $__resvar=$(iwpriv $iface gEnableRadio | cut -d ":" -f 2)
}

hostapd_enabled() {
	hostapd_cli -i$1 stat | grep state=ENABLED &>/dev/null
}

beerocks_operational() {
	caltest -g /tmp/db_read_param_tmp.txt &>/dev/null >/tmp/db_read_tmp.txt
	cat /tmp/db_read_tmp.txt | grep -i "\"paramValue\" : \"1\"," &>/dev/null
	#grep "beerocks is in operational state!" /var/log/servd_log &>/dev/null
}

beerocks_non_operational() {
	caltest -g /tmp/db_read_param_tmp.txt &>/dev/null >/tmp/db_read_tmp2.txt
	cat /tmp/db_read_tmp2.txt | grep -i "\"paramValue\" : \"0\"," &>/dev/null
	#grep "beerocks is in operational state!" /var/log/servd_log &>/dev/null
}

cleanup(){
	rm /tmp/db_read_param_tmp.txt
	rm /tmp/db_read_tmp.txt
	rm /tmp/db_read_tmp2.txt
}

is_running() {
	ps -eaf > /tmp/tmp_ps_result.txt
	grep $0 /tmp/tmp_ps_result.txt | grep -v grep | grep -v $$ &>/dev/null
}

ping_test() {
		local ip=$1
		local __resvar=$2
		ping $ip -c 1
		eval $__resvar=$?
}

if is_running; then
	echo "$0 already running - abort"
	exit 0
fi

TIMEOUT=${1-120}
LAN_CLIENT=${2-"192.168.1.66"}
COUNTER=0
TIMEOUT2=${1-300}
COUNTER2=0


echo "$0 - start, timeout=$TIMEOUT seconds"
echo -en "object:Device.X_INTEL_COM_BEEROCKS\nparam:Operational\n" > /tmp/db_read_param_tmp.txt


while [ $COUNTER -lt $TIMEOUT ]; do
		if hostapd_enabled wlan0 && hostapd_enabled wlan2; then
                echo "both wlan0 and wlan2 are up"
				if beerocks_operational; then
					echo "####### beerocks reached operational state #######"
					echo "####### start test: does beerocks stay operational, timeout=$TIMEOUT2 seconds #######"
					
					while [ $COUNTER2 -lt $TIMEOUT2 ]; do
						if beerocks_non_operational; then
							echo "####### Result: not-operational, issue reproduced #######"
							echo "####### after operational beerocks changed to non-operational --> issue reproduced #######"
							echo "####### end test: does beerocks stay operational #######"
							break;
						else
							if [ $(( $COUNTER2 % 10 )) -eq 0 ]; then
								echo "####### ... beerocks still operational: $COUNTER2 seconds #######"
							fi
						fi
						sleep 1
						let COUNTER2=COUNTER2+1
					done
					
					if [ $COUNTER2 -ge $TIMEOUT2 ]; then
						echo "####### Result: issue not reproduced - reboot #######"
						echo "####### end test: does beerocks stay operational #######"
						rm /tmp/db_read_param_tmp.txt
						cleanup
						reboot
						exit 0
					fi
					cleanup
					pad
					exit 0
				fi
        fi
        sleep 1
        let COUNTER=COUNTER+1
done

echo "####### not all wlan aps are up after timeout $TIMEOUT seconds --> issue reproduced #######"
cleanup
pad