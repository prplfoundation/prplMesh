#!/bin/sh

# Test script for Wi-Fi multiap Control API for APs
# Commands based on version 8.1.0
#
# On target board run:
# $ cd /tmp
# $ mknod wfa_ca-pipe p
# $ nc -l -p 9000 < sigma-pipe | ./sigma-ap.sh > sigma-pipe &
# or as persistent
# while true; do nc -l -p 9000 < sigma-pipe  | "./sigma-ap.sh" > ./sigma-pipe; done &
# On PC run:
# $ nc <board-ip> 8989
# or
# connect with telnet client in raw mode to <board-ip>:8989
# Then enter commands and send them with <ENTER>

# First digit - Major: Changes incompatible with previous version
# Second digit - Minor: Extension features
# Third digit - Fix: Fixes
# TODO: Store md5 checksum inside the script
#FW_VERSION=`version.sh | grep "Wave wlan version" | awk -F ": " '{print $2}'`

CA_VER_NUM="1.0.0"
MODEL='GRX350'
VENDOR='INTEL'
DEBUGGING=0 # 1=enabled 0=disabled
# TIMESTAMP="cat /proc/uptime"
TIMESTAMP=
DEFAULT_IFS=$IFS
COMMANDS="ca_get_version,device_get_info,dev_reset_default,dev_set_config,dev_get_parameter,dev_send_1905,start_wps_registration"
list_capi_controller_api="dev_reset_default,dev_set_config,dev_get_parameter,dev_send_1905,start_wps_registration"
list_capi_agent_api="dev_set_config"
controller_resp="wfa_ca_controller: return value is: BML_RET_OK,"
agent_resp="wfa_ca_agent: return value is: BML_RET_OK,"
USE_BML=1 #1=direct call to bml 0=use mapfcli
CLI_PATH="/opt/beerocks/beerocks_cli"
CA_VERSION="multiap-capi-$CA_VER_NUM"
FW_VERSION=`version.sh | grep "beerocks_slave_version" | awk -F ": " '{print $2}'`
BML_RESP="0"
TEST_IP="192.168.1.1"
DUTInfo_PATH="/opt/beerocks/DUTInfo_INTELGRX350.txt"
dirname() {
	full=$1
	file=`basename $full`
	path=${full%%$file}
	echo $path
	[ -z "$path" ] && path=./
	echo $path
}
thispath=`dirname $0`

debug_print()
{
   if [ "$DEBUGGING" = "1" ] ; then
      echo "$*" > /dev/console
   fi
}

error_print()
{
   echo "$*" > /dev/console
}

#--- Helper Functions ------------------------------------------------------------------------------
# check if tr command is available
# lower/uppercase conversion is faster with external tr
TR=`command -v tr`
if [ "$TR" = "" ]; then
   debug_print "tr not available"
      alias lower=lc_int
         alias upper=uc_int
         else
            debug_print "tr available at $TR"
               alias lower=lc_ext
                  alias upper=uc_ext
                  fi
                  

#alias send_running="echo status,RUNNING"

send_running()
{
   IFS=,
   echo "status,RUNNING " `eval $TIMESTAMP`
}

# First char for these function needs to be a "," to be able to also send replies
# without parameters
send_complete()
{
   IFS=,
   echo "status,COMPLETE$*" `eval $TIMESTAMP`
}

send_error()
{
   IFS=,
   echo "status,ERROR$*" `eval $TIMESTAMP`
}

send_invalid()
{
   IFS=,
   echo "status,INVALID$*" `eval $TIMESTAMP`
}

#
# TODO: Check whether sed works faster when all the variables are changed at once
#       Maybe the file read only once in this case.
#

UPPERCHARS=ABCDEFGHIJKLMNOPQRSTUVWXYZ
LOWERCHARS=abcdefghijklmnopqrstuvwxyz

lc_int()
{
   #usage: lc "SOME STRING" "destination variable name"
   i=0
   OUTPUT=""
   while ([ $i -lt ${#1} ]) do
      CUR=${1:$i:1}
      case $UPPERCHARS in
         *$CUR*)
            CUR=${UPPERCHARS%$CUR*}
            OUTPUT="${OUTPUT}${LOWERCHARS:${#CUR}:1}"
         ;;
         *)
            OUTPUT="${OUTPUT}$CUR"
         ;;
      esac
      i=$((i+1))
   done
   debug_print "lower-${OUTPUT}"
   export ${2}="${OUTPUT}"
}

lc_ext()
{
   export ${2}=`echo $1 | tr '[:upper:]' '[:lower:]'`
}

uc_int()
{
   #usage: uc "some string" -> "SOME STRING"
   i=0
   OUTPUT=""
   while ([ $i -lt ${#1} ]) do
      CUR=${1:$i:1}
      case $LOWERCHARS in
         *$CUR*)
            CUR=${LOWERCHARS%$CUR*}
            OUTPUT="${OUTPUT}${UPPERCHARS:${#CUR}:1}"
         ;;
         *)
            OUTPUT="${OUTPUT}$CUR"
         ;;
      esac
      i=$((i+1))
   done
   debug_print "upper-${OUTPUT}"
   export ${2}="${OUTPUT}"
}

uc_ext()
{
   export ${2}=`echo $1 | tr '[:lower:]' '[:upper:]'`
}

#--- Command Functions ------------------------------------------------------------------------------
capi_controller_api()
{
    send_running
	if [ $USE_BML == "1" ];
	then
    	bmlResult=`$CLI_PATH -c "bml_wfa_ca_controller $*"`
        status=`echo "$bmlResult"|tr '\n' ' '|grep "$controller_resp"`
    	if [[ -n "$status" ]];
	    then
    		send_complete ""
			BML_RESP="1"
	    else
    		send_invalid ",errorCode,38"
			BML_RESP="0"
   		 fi
	else
		echo "TODO: MAPFCLI"
	fi
}
capi_agent_api()
{
    send_running
	if [ $USE_BML == "1" ];
	then
    	bmlResult=`$CLI_PATH -c "bml_wfa_ca_agent $*"`
	    status=`echo "$bmlResult"|tr '\n' ' '|grep "$agent_resp"`
    	if [[ -n "$status" ]];
	    then
    		send_complete ""
			BML_RESP="1"
	    else
    		send_invalid ",errorCode,38"
			BML_RESP="0"
   		 fi
	else
		echo "TODO: MAPFCLI"
	fi
}
ca_get_version()
{
   send_running
   send_complete ",version,$CA_VERSION"
}
device_get_info()
{
   send_running
   #configure following values - vendor , model number , FW version
   if [ -e $DUTInfo_PATH ];then
   	while IFS= read line
	do
	   key=`echo "$line" | cut -d"=" -f1`
           value=`echo "$line" | cut -d"=" -f2`
           if [ "$key" == "vendor" ];then
           	VENDOR=$value
           fi
           if [ "$key" == "model" ];then
            	MODEL=$value
           fi
	done <"$DUTInfo_PATH"
   fi
   send_complete ",vendor,$VENDOR,model,$MODEL,version,$FW_VERSION"
}
dev_reset_default()
{

	capi_controller_api dev_reset_default $*
	wifi_interface="wlan0,wlan2"
	eth_interface="eth0_1,eth0_2,eth0_3,eth0_4"
	if [ "$BML_RESP" == "1" ];then
#Disable all WIFI Interface
		for intf in $wifi_interface
  		do
			ifconfig $intf down
	        done
	        
#Check CAPI interface
	    capi_ip=`netstat -t | grep 9000 |grep ESTABLISHED | awk '{ print $5 }' | cut -d: -f4 | cut -d"%" -f1`
    	capi_mac=`arp | grep $capi_ip | awk '{ print $4 }'`
    	
#seach capi_mac related eth port
		capi_intf=`brctl showmacs br-lan | grep $capi_mac | awk '{ print $5 }'`
		for intf in $eth_interface
  		do
			if [ $capi_intf != $intf ]; then
				ifconfig $intf down
			fi
	    done
#Do network restart
	#ubus call network restart	
#Give Static IP	
		ifconfig br-lan $TEST_IP
	fi
}
dev_set_config()
{
	#TODO: based on input call controller/agent
	check_agent_cmd=`echo $* | grep -i "backhaul"` #Assume backhaul for agent
	if [[ -n "$check_agent_cmd" ]];
	then
			capi_agent_api dev_set_config $*
	else
			capi_controller_api dev_set_config $*
	fi
}
send_macaddr()
{
	file="/sys/class/net/$1/address"
	if [[ -e $file ]];then
		mac=`cat $file`
		send_complete ",id,$mac"
	else
		send_error ""
	fi
}

check_plat_param()
{
	send_running
	found="0"	
	for index in "$@"	
	do
		if [ "$found" == "1" ];then
			case $index in
				"ALid")
					paramval=`cat /sys/class/net/br-lan/address`
					if [[ -n "$paramval" ]]; then
						send_complete ",$index,$paramval"
					else
						send_error ""
					fi
				;;			
				"backhaultype")
#					send_complete ",backhaultype,wifi" #TODO: how to know?
					send_complete ",backhaultype,ethernet"
				;;
				"fronthaulradio")
					radio0=`echo "$*" | grep "24g"`
					radio1=`echo "$*" | grep "5g"`
					if [[ -n "$radio0" ]];then
						send_macaddr wlan0
					fi
					if [[ -n "$radio1" ]];then
						send_macaddr wlan2
					fi
					if [[ -z "$radio0" ]] && [[ -z "$radio1" ]];then	
						send_invalid ""
					fi
					return
				;;
				"bssid")
				;;
				"macaddr")
				;;
				"backhaulband")
				;;
				"HTCap_5G1_Supt")
				;;
				"HTCap_5G2_Supt")
				;;
				"VHTCap_5G1_Supt")
				;;
				"VHTCap_5G2_Supt")
				;;
				"HECap_5G1_Supt")
				;;
				"HECap_5G1_Supt")
				;;
				"HECap_24G_Supt")
				;;
				"HTCap_24G_Supt")
				;;
				"FronthaulRadio_24G_Supt")
				;;
				"FronthaulRadio_5G1_Supt")
				;;
				"FronthaulRadio_5G2_Supt")
				;;
				"BackhaulSTA_Supt")
				;;
				"FronthaulRadio_24G_BSSID")
						send_macaddr wlan0
				;;
				"FronthaulRadio_5G1_BSSID")
						send_macaddr wlan2
				;;
				"FronthaulRadio_5G2_BSSID")
						send_macaddr wlan4
				;;
				*)
					send_invalid ""
				;;		
			esac
		fi		
		if [ "$index" == "parameter" ];then
			found="1"
		fi
	done
}
dev_get_parameter()
{
	if [ $USE_BML == "1" ];
	then
	    send_running
	    #Change paramlist to comma seperated insync with bml
	    paramlist=`echo $* | tr " " ","` 
	    bmlResult=`$CLI_PATH -c "bml_wfa_ca_controller dev_get_parameter,$paramlist"`
	    status=`echo "$bmlResult"|tr '\n' ' '|grep "$controller_resp"`
	    if [[ -n "$status" ]];
	        then
		paramvalue=`echo "$bmlResult"|tr '\n' ' '|cut -d"=" -f2`
		found="0"	
		for param in "$@"	
		do
		if [ "$found" == "1" ];then
			if [ "$param" == "fronthaulradio" ];then
				send_complete ",id,$paramvalue"		
			else
				send_complete ",$param,$paramvalue"		
			fi
			return;
		fi		
		if [ "$param" == "parameter" ];then
			found="1"
		fi
		done
	    else
			send_error ""
	    fi	
	else
			echo "TODO: MAPFCLI"
	fi
}

dev_send_1905()
{
	if [ $USE_BML == "1" ];
	then
    	bmlResult=`$CLI_PATH -c "bml_wfa_ca_controller $*"`
	    status=`echo "$bmlResult"|tr '\n' ' '|grep "$controller_resp"`
    	if [[ -n "$status" ]];
	    then
#TODO: Based on return parse string and send mid value
			mid=`echo $status | cut -d"=" -f2` #Assume string returns mid=xx
    		send_complete ",mid,$mid"
			BML_RESP="1"
	    else
    		send_invalid ",errorCode,38"
			BML_RESP="0"
   		 fi
	else
		echo "TODO: MAPFCLI"
	fi
}

start_wps_registration()
{
	capi_controller_api start_wps_registration $*
}

#--- Parser ------------------------------------------------------------------------------

parse_command()
{
   #echo parsing,`eval $TIMESTAMP`

   cmd=$1
   lower "$1" cmd
   shift

   debug_print "running command: >>>$cmd<<<"
   #echo $cmd
   found="0"
   for valid in $COMMANDS
   do
	if [ $valid == $cmd ]; then
	   found="1"
	fi
   done
   if [ $found == "1" ]; then
	   	BML_RESP="0"
   		$cmd $*
   else
      send_invalid ",errorCode,38"
      debug_print "Unknown multiap command: >>>$cmd<<<"
      debug_print "Supported commands:"
      debug_print $COMMAND 
   fi
   cmd=""
   return
}

#send_running
#important, set field separator properly
IFS=" "

let ap_line_count=0
# cp -f /etc/rc.conf /tmp
while read line ; do
   debug_print "read: >>>$line<<<"
   let ap_line_count+=1
   # remove special characters except comma, underscore, exclamation mark
#   tline=`echo $line | tr -dc '[:alnum:],_!\n'`
#   debug_print "tline: >>>$tline<<<"
   # For Windows hosts we need to remove carrige returns from the line
   tline=`echo $line | tr -d '\r'`
   # Delete trailing spaces
   tline=`echo $tline | tr -d ' '`
   debug_print "tline: >>>$tline<<<"
   IFS=,
   parse_command $tline
   IFS=" "

   debug_print "lines parsed $ap_line_count"
   debug_print "clearing all temp ap_ variables"
   variables=`set | grep "^ap_" | cut -d= -f1 | xargs echo `
   for var in $variables ; do
      debug_print "clearing $var"
      unset ${var}
   done
   unset variables
   unset token
   unset line
   unset tline
   #set
done
