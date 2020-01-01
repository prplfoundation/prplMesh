#!/bin/sh
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 Gal Wiener (Intel), Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

ALL_TESTS="topology initial_ap_config ap_config_renew ap_config_bss_tear_down channel_selection
           ap_capability_query client_capability_query combined_infra_metrics
           client_steering_mandate client_steering_dummy client_association_dummy client_steering_policy client_association
           higher_layer_data_payload_trigger higher_layer_data_payload"

scriptdir="$(cd "${0%/*}"; pwd)"
rootdir="${scriptdir%/*/*/*}"
. ${rootdir}/tools/docker/functions.sh

redirect="> /dev/null 2>&1"
error=0

bridge_name=br-lan

container_ip() {
    docker exec -t "$1" ip -f inet addr show "$bridge_name" | grep -Po 'inet \K[\d.]+'
}

container_CAPI_port() {
    # get the CAPI port based on the container name.
    docker exec -t "$1" grep ucc_listener_port ${installdir}/config/beerocks_controller.conf  | cut -f2 -d= | cut -f1 -d' '
}

send_bml_command() {
    docker exec gateway ${installdir}/bin/beerocks_cli -c "$*"
}

send_CAPI_command() {
    # send a CAPI command to a container.
    ip="$(container_ip "$1")"
    port="$(container_CAPI_port "$1")"
    dbg "Sending to $ip:$port. Command: $2"
    "${rootdir}/tools/docker/tests/send_CAPI_command.py" "$ip" "$port" "$2"
}

test_initial_ap_config() {
    status "test initial autoconfig"

    check_error=0
    check docker exec repeater1 sh -c \
        'grep -i -q "WSC Global authentication success" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec repeater1 sh -c \
        'grep -i -q "WSC Global authentication success" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'
    check docker exec repeater1 sh -c \
        'grep -i -q "KWA (Key Wrap Auth) success" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec repeater1 sh -c \
        'grep -i -q "KWA (Key Wrap Auth) success" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'
    check docker exec repeater1 sh -c \
        'grep -i -q "Controller configuration (WSC M2 Encrypted Settings)" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec repeater1 sh -c \
        'grep -i -q "Controller configuration (WSC M2 Encrypted Settings)" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'

    return $check_error
}

test_ap_config_renew() {
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

    return $check_error
}

test_ap_config_bss_tear_down() {
    status "test ap config bss tear down"
    # Regression test: MAC address should be case insensitive
    MAC_AGENT1=$(echo $mac_agent1 | tr a-z A-Z)
    # Configure the controller and send renew
    send_CAPI_command gateway "DEV_RESET_DEFAULT" $redirect
    send_CAPI_command gateway "DEV_SET_CONFIG,bss_info1,$mac_agent1 8x Multi-AP-24G-1 0x0020 0x0008 maprocks1 0 1" $redirect

    gw_mac_without_colons="$(printf $mac_gateway | tr -d :)"
    send_CAPI_command gateway "DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x000A,tlv_type1,0x01,tlv_length1,0x0006,tlv_value1,0x${gw_mac_without_colons},tlv_type2,0x0F,tlv_length2,0x0001,tlv_value2,{0x00},tlv_type3,0x10,tlv_length3,0x0001,tlv_value3,{0x00}}" $redirect

    sleep 3
    check docker exec repeater1 sh -c \
        'grep -i -q "ssid: Multi-AP-24G-1, .* fronthaul" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec repeater1 sh -c \
        'grep -i -q "ssid: .* teardown" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'

    # SSIDs have been removed for the CTT Agent1's front radio
    send_CAPI_command gateway "DEV_SET_CONFIG,bss_info1,$MAC_AGENT1 8x" $redirect
    # Send renew message
    send_CAPI_command gateway "DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x000A,tlv_type1,0x01,tlv_length1,0x0006,tlv_value1,0x${gw_mac_without_colons},tlv_type2,0x0F,tlv_length2,0x0001,tlv_value2,{0x00},tlv_type3,0x10,tlv_length3,0x0001,tlv_value3,{0x00}}" $redirect

    sleep 3
    check docker exec repeater1 sh -c \
        'grep -i -q "ssid: .* teardown" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    
    return $check_error
}

test_channel_selection() {
    status "test channel selection"
    
    check_error=0
    dbg "Send channel preference query"
    send_CAPI_command gateway "DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8004" $redirect
    sleep 1
    dbg "Confirming channel preference query has been received on agent"
    check docker exec repeater1 sh -c \
        'grep -i -q "CHANNEL_PREFERENCE_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec repeater1 sh -c \
        'grep -i -q "CHANNEL_PREFERENCE_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'
    
    dbg "Send channel selection request"
    send_CAPI_command gateway "DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8006" $redirect
    sleep 1
    dbg "Confirming channel selection request has been received on agent"
    check docker exec repeater1 sh -c \
        'grep -i -q "CHANNEL_SELECTION_REQUEST_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec repeater1 sh -c \
        'grep -i -q "CHANNEL_SELECTION_REQUEST_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'
    
    check docker exec gateway sh -c \
        'grep -i -q "certification buffer with CHANNEL_SELECTION_REQUEST_MESSAGE CMDU was loaded successfully!" /tmp/$USER/beerocks/logs/beerocks_controller.log'

    dbg "Confirming 1905.1 Ack Message request was received on agent"
    # TODO: When creating handler for the ACK message on the agent, replace lookup of this string
    check docker exec repeater1 sh -c \
        'grep -i -q "ACK_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec repeater1 sh -c \
        'grep -i -q "ACK_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'

    return $check_error
}
test_client_capability_query() { 
    status "test client capability"

    check_error=0
    send_CAPI_command gateway "DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8009,tlv_type,0x90,tlv_length,\
0x000C,tlv_value,{$mac_agent1_wlan0 0x000000110022}" $redirect
    sleep 1
    dbg "Confirming client capability query has been received on agent"
    # check that both radio agents received it,in the future we'll add a check to verify which radio the query was intended for.
    check docker exec repeater1 sh -c \
        'grep -i -q "CLIENT_CAPABILITY_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec repeater1 sh -c \
        'grep -i -q "CLIENT_CAPABILITY_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'
}
test_ap_capability_query() {
    status "test ap capability query"
    check_error=0
    check send_CAPI_command gateway "DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8001" $redirect
    sleep 1
    dbg "Confirming ap capability query has been received on agent"
    check docker exec repeater1 sh -c 'grep -i -q "AP_CAPABILITY_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    return $check_error
}
test_combined_infra_metrics() {
    err "combined_infra_metrics not implemented yet."
    return 0
}
test_client_steering_mandate() {
    status "test client steering"
    check_error=0

    dbg "Send topology request to agent 1"
    check send_CAPI_command gateway "DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x0002" $redirect
    sleep 1
    dbg "Confirming topology query was received"
    check docker exec repeater1 sh -c \
        'grep -i -q "TOPOLOGY_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent.log'

    dbg "Send topology request to agent 2"
    check send_CAPI_command gateway "DEV_SEND_1905,DestALid,$mac_agent2,MessageTypeValue,0x0002" $redirect
    sleep 1
    dbg "Confirming topology query was received"
    check docker exec repeater2 sh -c \
        'grep -i -q "TOPOLOGY_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent.log'

    dbg "Send Client Steering Request message for Steering Mandate to CTT Agent1"
    check send_CAPI_command gateway "DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8014,tlv_type,0x9B,tlv_length,\
0x001b,tlv_value,{$mac_agent1_wlan0 0xe0 0x0000 0x1388 0x01 {0x000000110022} 0x01 {$mac_agent2_wlan0 0x73 0x24}}" $redirect
    sleep 1
    dbg "Confirming Client Steering Request message was received - mandate"
    check docker exec repeater1 sh -c \
        'grep -i -q "Got steer request" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    
    dbg "Confirming BTM Report message was received"
    check docker exec gateway sh -c \
        'grep -i -q "CLIENT_STEERING_BTM_REPORT_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_controller.log'

    dbg "Confirming ACK message was received"
    check docker exec repeater1 sh -c \
        'grep -i -q "ACK_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'

    check send_CAPI_command gateway "DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8014,tlv_type,0x9B,tlv_length,\
0x000C,tlv_value,{$mac_agent1_wlan0 0x00 0x000A 0x0000 0x00}" $redirect
    sleep 1
    dbg "Confirming Client Steering Request message was received - Opportunity"
    check docker exec repeater1 sh -c \
        'grep -i -q "CLIENT_STEERING_REQUEST_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'

    dbg "Confirming ACK message was received"
    check docker exec gateway sh -c \
        'grep -i -q "ACK_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_controller.log'

    dbg "Confirming steering completed message was received"
    check docker exec gateway sh -c \
        'grep -i -q "STEERING_COMPLETED_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_controller.log'

    dbg "Confirming ACK message was received"
    check docker exec repeater1 sh -c \
        'grep -i -q "ACK_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    return $check_error
}

test_client_association_dummy(){
    status "test client association dummy"
    check_error=0
    sta_mac=11:11:33:44:55:66

    dbg "Connect dummy STA to wlan0"
    check docker exec repeater1 sh -c \
        "echo 'EVENT AP-STA-CONNECTED ${sta_mac}' >> /tmp/$USER/beerocks/wlan0/EVENT"
    
    dbg "Send client association control request to the chosen BSSID to steer the client (UNBLOCK) "
    eval send_bml_command "client_allow \"${sta_mac} ${mac_agent1_wlan2}\"" $redirect
    sleep 1

    dbg "Confirming Client Association Control Request message was received (UNBLOCK)"
    check docker exec repeater1 sh -c \
        "grep -i -q 'Got client allow request for ${sta_mac}' /tmp/\$USER/beerocks/logs/beerocks_agent_wlan2.log"

    dbg "Send client association control request to all other (BLOCK) "
    eval send_bml_command "client_disallow \"${sta_mac} ${mac_agent1_wlan0}\"" $redirect
    sleep 1

    dbg "Confirming Client Association Control Request message was received (BLOCK)"
    check docker exec repeater1 sh -c \
        "grep -i -q 'Got client disallow request for ${sta_mac}' /tmp/\$USER/beerocks/logs/beerocks_agent_wlan0.log"
    return $check_error
}

wait_for_message() {
    found=0
    timeout=$1
    container=$2
    file=$3
    msg=$4
    for wait in $(seq 1 $timeout); do
        sleep 1
        command="docker exec -it $container grep -i -q "$msg" /tmp/$USER/beerocks/logs/$file"
        if docker exec -it $container grep -i -q "$msg" "/tmp/$USER/beerocks/logs/"$file; then
            dbg "OK after $wait $command"
            found=1
            break
        fi
    done
    if [ $found = 0 ]; then
        err "FAIL after $wait $command" 
        return 1
    fi
}

test_optimal_path_dummy() {
    status "test optimal path dummy"
    check_error=0

    dbg "Connect dummy STA to wlan0"
    check docker exec -it repeater1 sh -c \
        'echo "EVENT AP-STA-CONNECTED 11:22:33:44:55:66" > /tmp/$USER/beerocks/wlan0/EVENT'

    dbg "Pre-prepare RRM Beacon Response for association handling task"
    check docker exec -it repeater1 sh -c \
        'echo "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-40 rsni=40 bssid=aa:bb:cc:00:00:10" > /tmp/$USER/beerocks/wlan0/EVENT'

    dbg "Confirming 11k request is done by association handling task"
    wait_for_message 2 repeater1 "beerocks_monitor_wlan0.log" "Beacon 11k request to sta 11:22:33:44:55:66 on bssid aa:bb:cc:00:00:10 channel 1"

    dbg "Update Stats"
    check docker exec -it repeater1 sh -c \
        'echo "DATA STA-UPDATE-STATS 11:22:33:44:55:66 rssi=-38,-39,-40,-41 snr=38,39,40,41 uplink=1000 downlink=800" > /tmp/$USER/beerocks/wlan0/EVENT'

    dbg "Pre-prepare RRM Beacon Responses for optimal path task"
    #Response for IRE1, BSSID of wlan0.0
    check docker exec -it repeater1 sh -c \
        'echo "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=aa:bb:cc:11:00:10" > /tmp/$USER/beerocks/wlan0/EVENT'

    #Response for IRE1, BSSID of wlan2.0
    check docker exec -it repeater1 sh -c \
        'echo "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=aa:bb:cc:11:00:20" > /tmp/$USER/beerocks/wlan0/EVENT'

    #Response for IRE2, BSSID of wlan0.0
    check docker exec -it repeater1 sh -c \
        'echo "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-40 rsni=40 bssid=aa:bb:cc:00:00:10" > /tmp/$USER/beerocks/wlan0/EVENT'

    #Response for IRE2, BSSID of wlan2.0
    check docker exec -it repeater1 sh -c \
        'echo "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=aa:bb:cc:00:00:20" > /tmp/$USER/beerocks/wlan0/EVENT'

    #Response for GW, BSSID of wlan0.0
    check docker exec -it repeater1 sh -c \
        'echo "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=00:11:22:33:00:10" > /tmp/$USER/beerocks/wlan0/EVENT'

    #Response for GW, BSSID of wlan2.0
    check docker exec -it repeater1 sh -c \
        'echo "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=00:11:22:33:00:20" > /tmp/$USER/beerocks/wlan0/EVENT'

    dbg "Confirming 11k request is done by optimal path task"
    wait_for_message 20 repeater1 "beerocks_monitor_wlan0.log" "Beacon 11k request to sta 11:22:33:44:55:66 on bssid aa:bb:cc:11:00:20 channel 149"

    dbg "Confirming 11k request is done by optimal path task"
    wait_for_message 20 repeater1 "beerocks_monitor_wlan0.log" "Beacon 11k request to sta 11:22:33:44:55:66 on bssid aa:bb:cc:00:00:20 channel 149"

    dbg "Confirming 11k request is done by optimal path task"
    wait_for_message 20 repeater1 "beerocks_monitor_wlan0.log" "Beacon 11k request to sta 11:22:33:44:55:66 on bssid aa:bb:cc:11:00:10 channel 1"

    dbg "Confirming 11k request is done by optimal path task"
    wait_for_message 20 repeater1 "beerocks_monitor_wlan0.log" "Beacon 11k request to sta 11:22:33:44:55:66 on bssid 00:11:22:33:00:20 channel 149"

    dbg "Confirming 11k request is done by optimal path task"
    wait_for_message 20 repeater1 "beerocks_monitor_wlan0.log" "Beacon 11k request to sta 11:22:33:44:55:66 on bssid 00:11:22:33:00:10 channel 1"

    dbg "Confirming no steer is done"
    wait_for_message 20 gateway "beerocks_controller.log" "could not find a better path for sta 11:22:33:44:55:66"

    # Steer scenario
    dbg "Update Stats"
    check docker exec -it repeater1 sh -c \
        'echo "DATA STA-UPDATE-STATS 11:22:33:44:55:66 rssi=-58,-59,-60,-61 snr=18,19,20,21 uplink=100 downlink=80" > /tmp/$USER/beerocks/wlan0/EVENT'

    dbg "Pre-prepare RRM Beacon Responses for optimal path task"
    #Response for IRE1, BSSID of wlan0.0
    check docker exec -it repeater1 sh -c \
        'echo "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=aa:bb:cc:11:00:10" > /tmp/$USER/beerocks/wlan0/EVENT'

    #Response for IRE1, BSSID of wlan2.0
    check docker exec -it repeater1 sh -c \
        'echo "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=aa:bb:cc:11:00:20" > /tmp/$USER/beerocks/wlan0/EVENT'

    #Response for IRE2, BSSID of wlan0.0
    check docker exec -it repeater1 sh -c \
        'echo "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-60 rsni=20 bssid=aa:bb:cc:00:00:10" > /tmp/$USER/beerocks/wlan0/EVENT'

    #Response for IRE2, BSSID of wlan2.0
    check docker exec -it repeater1 sh -c \
        'echo "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=aa:bb:cc:00:00:20" > /tmp/$USER/beerocks/wlan0/EVENT'

    #Response for GW, BSSID of wlan0.0
    check docker exec -it repeater1 sh -c \
        'echo "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=00:11:22:33:00:10" > /tmp/$USER/beerocks/wlan0/EVENT'

    #Response for GW, BSSID of wlan2.0
    check docker exec -it repeater1 sh -c \
        'echo "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=00:11:22:33:00:20" > /tmp/$USER/beerocks/wlan0/EVENT'

    dbg "Confirming steering is requested by optimal path task"
    wait_for_message 20 gateway "beerocks_controller.log" "optimal_path_task: steering"

    # Error scenario, sta doesn't support 11k
    dbg "Connect dummy STA to wlan0"
    check docker exec -it repeater1 sh -c \
       'echo "EVENT AP-STA-CONNECTED 11:22:33:44:55:77" > /tmp/$USER/beerocks/wlan0/EVENT'

    dbg "Pre-prepare RRM Beacon Response with error for association handling task"
    check docker exec -it repeater1 sh -c \
        'echo "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:77 channel=0 dialog_token=0 measurement_rep_mode=4 op_class=0 duration=0 rcpi=0 rsni=0 bssid=aa:bb:cc:00:00:10" > /tmp/$USER/beerocks/wlan0/EVENT'

    dbg "Confirming 11k request is done by association handling task"
    wait_for_message 20 repeater1 "beerocks_monitor_wlan0.log" "Beacon 11k request to sta 11:22:33:44:55:77 on bssid aa:bb:cc:00:00:10 channel 1"

    dbg "Confirming STA doesn't support beacon measurement"
    wait_for_message 20 gateway "beerocks_controller.log" "setting sta 11:22:33:44:55:77 as beacon measurement unsupported"

    dbg "Confirming optimal path falls back to RSSI measurements"
    wait_for_message 20 gateway "beerocks_controller.log" "requesting rssi measurements for 11:22:33:44:55:77"
}

test_client_steering_dummy() {
    status "test client steering dummy"
    check_error=0
    sta_mac=11:22:33:44:55:66

    dbg "Connect dummy STA to wlan0"
    check docker exec repeater1 sh -c \
        "echo 'EVENT AP-STA-CONNECTED ${sta_mac}' >> /tmp/$USER/beerocks/wlan0/EVENT"

    dbg "Send steer request "
    eval send_bml_command "steer_client \"${sta_mac} ${mac_agent1_wlan2}\"" $redirect
    sleep 1

    dbg "Confirming Client Association Control Request message was received (UNBLOCK)"
    check docker exec repeater1 sh -c \
        'grep -i -q "Got client allow request" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'

    dbg "Confirming Client Association Control Request message was received (BLOCK)"
    check docker exec repeater1 sh -c \
        'grep -i -q "Got client disallow request" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'

    dbg "Confirming Client Association Control Request message was received (BLOCK)"
    check docker exec repeater2 sh -c \
        'grep -i -q "Got client disallow request" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'

    dbg "Confirming Client Association Control Request message was received (BLOCK)"
    check docker exec repeater2 sh -c \
        'grep -i -q "Got client disallow request" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'

    dbg "Confirming Client Steering Request message was received - mandate"
    check docker exec repeater1 sh -c \
        'grep -i -q "Got steer request" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'

    dbg "Confirming BTM Report message was received"
    check docker exec gateway sh -c \
        'grep -i -q "CLIENT_STEERING_BTM_REPORT_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_controller.log'

    dbg "Confirming ACK message was received"
    check docker exec repeater1 sh -c \
        'grep -i -q "ACK_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'

    dbg "Disconnect dummy STA from wlan0"
    check docker exec repeater1 sh -c \
        "echo 'EVENT AP-STA-DISCONNECTED ${sta_mac}' >> /tmp/$USER/beerocks/wlan0/EVENT"

    #TODO// check for "disconnected after successful steering, proceeding to unblock" message 

    dbg "Connect dummy STA to wlan2"
    check docker exec repeater1 sh -c \
        "echo 'EVENT AP-STA-CONNECTED ${sta_mac}' >> /tmp/$USER/beerocks/wlan2/EVENT"

    dbg "Confirm steering success by client connected"
    check docker exec gateway sh -c \
        "grep -i -q 'steering successful for sta ${sta_mac}' /tmp/\$USER/beerocks/logs/beerocks_controller.log"
    return $check_error
}

test_client_steering_policy() {
    status "test client steering policy"
    check_error=0
    rm /tmp/catch

    dbg "Send client steering policy to agent 1"
    check send_CAPI_command gateway "DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8003,tlv_type,0x89,tlv_length\
,0x000C,tlv_value,{0x00 0x00 0x01 {0x112233445566 0x01 0xFF 0x14}}" > /tmp/catch
    sleep 1
    MID1_STR=$(grep -Po "(?<=mid,0x).*[^\s]" /tmp/catch)
    dbg "Confirming client steering policy has been received on agent"
    
    check docker exec repeater1 sh -c 'grep -i -q "MULTI_AP_POLICY_CONFIG_REQUEST_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log > /tmp/catch'
    sleep 1
    dbg "Confirming client steering policy ack message has been received on the controller"
    TMP="$(docker exec gateway sh -c 'grep -i "ACK_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_controller.log')"
    MID2_STR="$(echo "$TMP" | tr ' ' '\n' | grep -Po "(?<=mid=).*[^\s]" | tail -n 1)"

    check [ "$MID1_STR" = "$MID2_STR" ]
    return $check_error
}

test_client_association() {
    status "test client association"
    check_error=0
    dbg "Send topology request to agent 1"
    check send_CAPI_command gateway "DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x0002" $redirect
    dbg "Confirming topology query was received"
    check docker exec repeater1 sh -c \
        'grep -i -q "TOPOLOGY_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent.log'

    dbg "Send client association control message"
    check send_CAPI_command gateway "DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8016,tlv_type,0x9D,tlv_length,\
0x000f,tlv_value,{$mac_agent1_wlan0 0x00 0x1E 0x01 {0x000000110022}}" $redirect

    dbg "Confirming client association control message has been received on agent"
    # check that both radio agents received it,in the future we'll add a check to verify which radio the query was intended for.
    check docker exec repeater1 sh -c \
        'grep -i -q "CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec repeater1 sh -c \
        'grep -i -q "CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'

    dbg "Confirming ACK message was received on controller"
    check docker exec gateway sh -c \
        'grep -i -q "ACK_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_controller.log'
    return $check_error
}

test_higher_layer_data_payload_trigger() {
    status "test higher layer data payload"
    check_error=0
    mac_gateway_hex=$(mac_to_hex $mac_gateway)
    dbg "mac_gateway_hex = ${mac_gateway_hex}"
    copies=200
    payload="$mac_gateway_hex"
    for i in `seq 1 $(($copies - 1))`
    do
        payload="$payload $mac_gateway_hex"
    done

    dbg "Send Higher Layer Data message"
    # MCUT sends Higher Layer Data message to CTT Agent1 by providing:
    # Higher layer protocol = "0x00"
    # Higher layer payload = 200 concatenated copies of the ALID of the MCUT (1200 octets)
    check send_CAPI_command gateway "DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8018,tlv_type,0xA0,tlv_length,\
0x04b1,tlv_value,{0x00 $payload}" $redirect

    dbg "Confirming higher layer data message was received in the agent" 
    
    check docker exec repeater1 sh -c \
        'grep -i -q "HIGHER_LAYER_DATA_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent.log'

    dbg "Confirming matching protocol and payload length"

    check docker exec repeater1 sh -c \
        'grep -i -q "protocol: 0" /tmp/$USER/beerocks/logs/beerocks_agent.log'
    check docker exec repeater1 sh -c \
        'grep -i -q "payload_length: 4b0" /tmp/$USER/beerocks/logs/beerocks_agent.log'

    dbg "Confirming ACK message was received in the controller"
    check docker exec gateway sh -c \
        'grep -i -q "ACK_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_controller.log'
    return $check_error
}

test_higher_layer_data_payload() {
    err "higher_layer_data_payload not implemented yet."
    return 0
}
test_topology() {
    status "test topology query"
    check_error=0
    check send_CAPI_command gateway "DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x0002" $redirect
    dbg "Confirming topology query was received"
    docker exec repeater1 sh -c 'grep -i -q "TOPOLOGY_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent.log'
    return $check_error
}
test_init() {
    status "test initialization"    
    eval ${scriptdir}/test_gw_repeater.sh -f -r "repeater1" -r "repeater2" -d 7 $redirect || {
        err "start GW+Repeater failed, abort"
        exit 1
    }
    #save bml_conn_map output into a file.
    connmap=$(mktemp)
    [ -z "$connmap" ] && { err "Failed to create temp file"; exit 1; }
    trap "rm -f $connmap" EXIT
    docker exec gateway ${installdir}/bin/beerocks_cli -c bml_conn_map > "$connmap"

    mac_gateway=$(grep "GW_BRIDGE" "$connmap" | head -1 | awk '{print $5}' | cut -d ',' -f 1)
    dbg "mac_gateway = ${mac_gateway}"

    mac_agent1=$(grep "IRE_BRIDGE" "$connmap" | head -1 | awk '{print $5}' | cut -d ',' -f 1)
    dbg "mac_agent1 = ${mac_agent1}"
    mac_agent2=$(grep "IRE_BRIDGE" "$connmap" | sed -n 2p | awk '{print $5}' | cut -d ',' -f 1)
    dbg "mac_agent2 = ${mac_agent2}"

    mac_agent1_wlan0=$(grep "RADIO: wlan0" "$connmap" | head -1 | awk '{print $4}' | cut -d ',' -f 1)
    dbg "mac_agent1_wlan0 = ${mac_agent1_wlan0}"

    mac_agent2_wlan0=$(grep "RADIO: wlan0" "$connmap" | sed -n 2p | awk '{print $4}' | cut -d ',' -f 1)
    dbg "mac_agent2_wlan0 = ${mac_agent2_wlan0}"

    mac_agent1_wlan2=$(grep "RADIO: wlan2" "$connmap" | head -1 | awk '{print $4}' | cut -d ',' -f 1)
    dbg "mac_agent1_wlan2 = ${mac_agent1_wlan2}"

    mac_agent2_wlan2=$(grep "RADIO: wlan2" "$connmap" | sed -n 2p | awk '{print $4}' | cut -d ',' -f 1)
    dbg "mac_agent2_wlan2 = ${mac_agent2_wlan2}"

}
usage() {
    echo "usage: $(basename $0) [-hv]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -v|--verbose - verbosity on"
    echo ""
    echo "  positional params:"
    echo "      topology - Topology discovery test"
    echo "      initial_ap_config - Initial AP configuration test"
    echo "      ap_config_renew - AP configuration renew test"
    echo "      ap_config_bss_tear_down - AP configuration BSS Tear Down test"
    echo "      channel_selection - Channel Selection test"
    echo "      client_steering_mandate - Client Steering for Steering Mandate and Steering Opportunity test"
    echo "      client_steering_dummy - Client Steering using dummy bwl"
    echo "      optimal_path_dummy - Optimal Path using dummy bwl"
    echo "      client_association_dummy - Client Association Control Message using dummy bwl"
    echo "      client_steering_policy - Setting Client Steering Policy test"
    echo "      client_association - Client Association Control Message test"
    echo "      ap_capability_query - AP Capability query test"
    echo "      client_capability_query - Client Capability info reporting test"
    echo "      combined_infra_metrics - Combined Infrastructure Metrics test"
    echo "      higher_layer_data_payload_trigger - Higher layer data payload over 1905 trigger test"
    echo "      higher_layer_data_payload - Higher layer data payload over 1905 test"
}
main() {
    OPTS=`getopt -o 'hv' --long help,verbose -n 'parse-options' -- "$@"`
    if [ $? != 0 ] ; then err "Failed parsing options." >&2 ; usage; exit 1 ; fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -v | --verbose)     VERBOSE=true; redirect=; shift ;;
            -h | --help)        usage; exit 0; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done
    [ -z "$*" ] && set ${ALL_TESTS}
    info "Tests to run: $*"
    test_init
    for test in "$@"; do
       report "test_${test}" test_${test}
       count=$((count+1))
    done

    if [ $error -gt 0 ]; then
        err "$error / $count tests failed"
        if [ "$VERBOSE" = "true" ]; then
            for dockerimage in gateway repeater1 repeater2; do
                info "*** Dumping logs from $dockerimage ***"
                docker exec $dockerimage sh -c '
                    for logfile in controller agent agent_wlan0 agent_wlan2; do
                        logpath=/tmp/$USER/beerocks/logs/beerocks_$logfile.log
                        [ -e $logpath ] && {
                            printf "\n\n==> %s <==\n" $logfile
                            cat $logpath
                        }
                    done'
            done
        fi
    else
        success "$count / $count tests passed"
    fi

    exit $error
}

VERBOSE=false

main "$@"
