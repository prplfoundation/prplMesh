from . import BaseTest, LogType, DeviceType
import time
"""
Original shell test:
    status "test initial autoconfig"

    # Regression test: MAC address should be case insensitive
    MAC_AGENT1=$(echo $mac_agent1 | tr a-z A-Z)
    # Configure the controller and send renew
    send_CAPI_command gateway "DEV_RESET_DEFAULT" $redirect
    send_CAPI_command gateway "DEV_SET_CONFIG,bss_info1,$MAC_AGENT1 8x Multi-AP-24G-1 0x0020 0x0008 maprocks1 0 1,bss_info2,$mac_agent1 8x Multi-AP-24G-2 0x0020 0x0008 maprocks2 1 0" $redirect
    gw_mac_without_colons="$(printf $mac_gateway | tr -d :)"
    send_CAPI_command gateway "DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x000A,tlv_type1,0x01,tlv_length1,0x0006,tlv_value1,0x${gw_mac_without_colons},tlv_type2,0x0F,tlv_length2,0x0001,tlv_value2,{0x00},tlv_type3,0x10,tlv_length3,0x0001,tlv_value3,{0x00}}" $redirect

    # Wait a bit for the renew to complete
    sleep 3

    check docker exec repeater1 sh -c \
        'grep -i -q "Received credentials for ssid: Multi-AP-24G-1 .* bss_type: 2" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec repeater1 sh -c \
        'grep -i -q "Received credentials for ssid: Multi-AP-24G-2 .* bss_type: 1" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec repeater1 sh -c \
        'grep -i -q "ssid: .* teardown" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'

    send_CAPI_command repeater1 "dev_get_parameter,program,map,ruid,0x000000000000,ssid,Multi-AP-24G-1,parameter,macaddr"
    check [ "$capi_command_reply" = "status,COMPLETE,macaddr,$mac_agent1_wlan0" ];
"""


class test_initial_ap_config(BaseTest):
    def test_initial_ap_config(self):
        MAC_AGENT1="00:11:22:33:44:55"  # TODO
        gw_mac_without_colons="".join([c for c in "AA:BB:CC:DD:EE:FF" if c != ":"])
        
        self.status("test initial autoconfig")
        self.send_controller_command("DEV_RESET_DEFAULT")
        self.send_controller_command("DEV_SET_CONFIG,bss_info1,{MAC_AGENT1} 8x Multi-AP-24G-1 " + \
                                     "0x0020 0x0008 maprocks1 0 1,bss_info2,$mac_agent1 8x Multi-AP-24G-2 " + \
                                     "0x0020 0x0008 maprocks2 1 0".format(MAC_AGENT1=MAC_AGENT1))
        self.send_controller_command("DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x000A," + \
                                     "tlv_type1,0x01,tlv_length1,0x0006,tlv_value1,0x{gw_mac_without_colons}," + \
                                     "tlv_type2,0x0F,tlv_length2,0x0001,tlv_value2,{{0x00}}," + \
                                     "tlv_type3,0x10,tlv_length3,0x0001,tlv_value3,{{0x00}}" \
                                    .format(gw_mac_without_colons=gw_mac_without_colons))
        time.sleep(3)
        
        self.assertTrue(self.find_in_logs(DeviceType.repeater1, LogType.beerocks_agent_wlan0,
                                    "Received credentials for ssid: Multi-AP-24G-1 .* bss_type: 2", regexp=True))
        self.assertTrue(self.find_in_logs(DeviceType.repeater1, LogType.beerocks_agent_wlan0,
                                      "Received credentials for ssid: Multi-AP-24G-2 .* bss_type: 1", regexp=True))
        self.assertTrue(self.find_in_logs(DeviceType.repeater1, LogType.beerocks_agent_wlan2,
                                       "ssid: .* teardown", regexp=True))
        reply = self.send_repeater1_command("dev_get_parameter,program,map,ruid,0x000000000000,ssid,Multi-AP-24G-1,parameter,macaddr")

        # Not done with this test yet, so make sure it fails
        self.assertTrue(False)
