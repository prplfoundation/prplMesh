#!/bin/bash
# Execute this script from the LAN client of DUT
# Assumtion: DUT ip is 192.168.1.1 and already running wfa_ca.sh
# This script will connect wfa_ca running on DUT and send capi commands

cmdlist="ca_get_version device_get_info dev_reset_default dev_send_1905 dev_set_config CA_GET_VERSION DEVICE_GET_INFO DEV_RESET_DEFAULT DEV_SEND_1905 DEV_SET_CONFIG"
ca_get_version_arg=""
CA_GET_VERSION_arg=""
device_get_info=""
DEVICE_GET_INFO_arg=""
dev_reset_default_arg=",name,cntlr1,program,map,devrole,controller,type,testbed"
DEV_RESET_DEFAULT_arg=$dev_reset_default_arg
dev_send_1905_arg=",DestALid,02:9A:96:F5:DB:F0,MessageTypeValue,0x8006,tlv_type1,0x8B,tlv_length1,0x004C,tlv_value1,{0xAC9A96F5DC10 0x14 {0x51 {0x00 0x00}} {0x52 {0x00 0x00}} {0x53 {0x00 0x00}} {0x54 {0x00 0x00}} {0x73 0x03 {0x28 0x2C 0x30} 0x00} {0x74 0x01 {0x2C} 0x00} {0x75 {0x00 0x00}} {0x76 {0x00 0x00}} {0x77 {0x00 0x00}} {0x78 {0x00 0x00}} {0x79 {0x00 0x00}} {0x7A {0x00 0x00}} {0x7B {0x00 0x00}} {0x7C {0x00 0x00}} {0x7D {0x00 0x00}} {0x7E {0x00 0x00}} {0x7F {0x00 0x00}} {0x80 0x05 {0x3A 0x6A 0x7A 0x8A 0x9B} 0x00} {0x81 {0x00 0x00}} {0x82 {0x00 0x00}}},tlv_type2,0x8D,tlv_length2,0x0007,tlv_value2,{0xAC9A96F5DC10 0x14}"
DEV_SEND_1905_arg=$dev_send_1905_arg
dev_set_config_arg=",program,map,bss_info1,02:9A:96:F5:DB:F0 11x Multi-AP-5LGC-1 0x0020 0x0008 maprocks1 0 1,bss_info2,02:9A:96:F5:DB:F0 11x Multi-AP-5LGC-2 0x0020 0x0008 maprocks2 1 0,bss_info3,02:9A:96:F5:14:70 8x Multi-AP-24G-1 0x0020 0x0008 maprocks1 1 1,bss_info4,02:9A:96:F5:14:70 11x Multi-AP-5LG-1 0x0020 0x0008 maprocks1 1 1,bss_info5,02:9A:96:F5:14:70 12x Multi-AP-5HG-1 0x0020 0x0008 maprocks1 1 1"
DEV_SEND_CONFIG_arg=$dev_set_config_arg
parse_result()
{
	pass=`echo $* | grep "status,COMPLETE"`
	if [ "$pass" != "" ];then
		echo "pass,$cmd"
	else
		echo "fail,$cmd"
	fi
}
for cmd in $cmdlist		
do
			cmd_arg="${cmd}_arg"
			result=`echo $cmd${!cmd_arg} | nc -i 1  192.168.1.1 9000`	
#			echo $result
			parse_result $result
done
paramlist="parameter,ALid ruid,1,ssid,some_ssid,parameter,bssid ruid,1,ssid,some_ssid,parameter,macaddr parameter,fronthaulradio,band,24g parameter,fronthaulradio,band,5g"
for param in $paramlist
do
			cmd_arg=",program,map,"
			cmd="dev_get_parameter"
			result=`echo $cmd${cmd_arg}$param | nc -i 1  192.168.1.1 9000`	
			echo $result
			parse_result $result
done
