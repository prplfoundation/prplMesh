#!/bin/sh
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 Gal Wiener (Intel), Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

# NOTE optimal_path_dummy must be done at the beginning. A shortcoming of our current test_flows
# approach is that no AP Auto(re)configuration is done at the beginning of each test, so it inherits
# the configuration from previous tests. Since the initial_ap_config, ap_config_renew and
# ap_config_bss_tear_down tests tear down some radios, they make the optimal_path_dummy test fail,
# since it uses all radios. A better solution would of course be to properly configure all APs,
# but just doing the optimal_path_dummy test first is simpler for the time being.
# FIXME optimal_path_dummy temporarily disabled since it's broken
ALL_TESTS="topology initial_ap_config ap_config_renew ap_config_bss_tear_down channel_selection
           ap_capability_query client_capability_query combined_infra_metrics
           client_steering_mandate client_steering_dummy client_association_dummy client_steering_policy client_association
           higher_layer_data_payload_trigger"

scriptdir="$(cd "${0%/*}"; pwd)"
rootdir="${scriptdir%/*}"
. ${rootdir}/tools/docker/functions.sh

redirect="> /dev/null 2>&1"
error=0

bridge_name=br-lan

get_host_bridge_name() {
    docker network inspect prplMesh-net-${UNIQUE_ID} --format="br-{{ printf \"%.12s\" .Id }}"
}

start_tcpdump() {
    test="$1"
    if [ "$TCPDUMP" = "true" ]; then
        # -i: interface to capture on; -w: output file
        tcpdump -i $(get_host_bridge_name) -w test_${test}.pcap &
        TCPDUMP_PROC=$!
        status "started tcpdump: pid=${TCPDUMP_PROC}, output file=test_${test}.pcap"
    fi
}

kill_tcpdump() {
    if [ -n "$TCPDUMP_PROC" ]; then
        status "Terminating tcpdump"
        kill $TCPDUMP_PROC;
    fi
    unset TCPDUMP_PROC
}

container_ip() {
    docker exec -t "$1" ip -f inet addr show "$bridge_name" | grep -Po 'inet \K[\d.]+'
}

container_CAPI_port() {
    # get the CAPI port based on the container name.
    # For test_flows, we always consider the gateway to be a controller-only
    # device (i.e. the agent on the gateway is not addressed by CAPI).
    if [ "$1" = "${GATEWAY}" ]; then
        docker exec -t "${GATEWAY}" grep ucc_listener_port ${installdir}/config/beerocks_controller.conf  | cut -f2 -d= | cut -f1 -d' '
    else
        docker exec -t "${GATEWAY}" grep ucc_listener_port ${installdir}/config/beerocks_agent.conf  | cut -f2 -d= | cut -f1 -d' '
    fi
}

send_bml_command() {
    docker exec ${GATEWAY} ${installdir}/bin/beerocks_cli -c "$*"
}

send_CAPI_command() {
    # send a CAPI command to a container.
    ip="$(container_ip "$1")"
    [ -z "$ip" ] && return 1
    port="$(container_CAPI_port "$1")"
    [ -z "$port" ] && return 1
    dbg "Sending to $ip:$port. Command: $2"
    capi_command_result=$(mktemp)
    [ -z "$capi_command_result" ] && { err "Failed to create temp file"; return 1; }
    trap "rm -f $capi_command_result" EXIT
    "${rootdir}/tests/send_CAPI_command.py" "$ip" "$port" "$2" > "$capi_command_result" || return $?
    dbg "Result:"
    [ "$VERBOSE" = true ] && cat "$capi_command_result"
    capi_command_reply=$(tr  -d '\r' < "$capi_command_result" | awk 'NR==2')
}

send_CAPI_1905() {
    container="$1"; shift
    dest="$1"; shift
    messagetype="$1"; shift
    command="DEV_SEND_1905,DestALid,${dest},MessageTypeValue,${messagetype}"
    for arg; do
        command="$command,$arg"
    done
    capi_command_mid=""
    send_CAPI_command "$container" "$command" || return $?
    capi_command_mid=$(echo "$capi_command_reply" | grep -Po "(?<=mid,0x).*[^\s]")
}

check_log() {
    # Check for a certain message in a log file
    # $1: device to check (${GATEWAY}, ${REPEATER1}, ${REPEATER2})
    # $2: program to check (controller, agent, agent_wlan0, ...)
    # rest: arguments to grep. -q is always added.
    device="$1"; shift
    program="$1"; shift
    check grep -q "$@" "${rootdir}/logs/${device}/beerocks_${program}.log"
}

send_bwl_event() {
    # Send an event to a dummy bwl
    # $1: device to send to (${GATEWAY}, ${REPEATER1}, ${REPEATER2})
    # $2: radio to send to (wlan0, wlan2)
    # $3: event string
    check docker exec $1 sh -c 'echo '"$3"' > /tmp/$USER/beerocks/'$2'/EVENT'
}

test_initial_ap_config() {
    status "test initial autoconfig"

    check_error=0
    check_log ${REPEATER1} agent_wlan0 "WSC Global authentication success"
    check_log ${REPEATER1} agent_wlan2 "WSC Global authentication success"
    check_log ${REPEATER1} agent_wlan0 "KWA (Key Wrap Auth) success"
    check_log ${REPEATER1} agent_wlan2 "KWA (Key Wrap Auth) success"
    check_log ${REPEATER1} agent_wlan0 "Controller configuration (WSC M2 Encrypted Settings)"
    check_log ${REPEATER1} agent_wlan2 "Controller configuration (WSC M2 Encrypted Settings)"

    return $check_error
}

test_ap_config_renew() {
    status "test initial autoconfig"

    # Regression test: MAC address should be case insensitive
    MAC_AGENT1=$(echo $mac_agent1 | tr a-z A-Z)
    # Configure the controller and send renew
    send_CAPI_command ${GATEWAY} "DEV_RESET_DEFAULT" $redirect
    send_CAPI_command ${GATEWAY} "DEV_SET_CONFIG,bss_info1,$MAC_AGENT1 8x Multi-AP-24G-1 0x0020 0x0008 maprocks1 0 1,bss_info2,$mac_agent1 8x Multi-AP-24G-2 0x0020 0x0008 maprocks2 1 0" $redirect
    gw_mac_without_colons="$(printf $mac_gateway | tr -d :)"
    send_CAPI_1905 ${GATEWAY} $mac_agent1 0x000A "tlv_type1,0x01,tlv_length1,0x0006,tlv_value1,0x${gw_mac_without_colons},tlv_type2,0x0F,tlv_length2,0x0001,tlv_value2,{0x00},tlv_type3,0x10,tlv_length3,0x0001,tlv_value3,{0x00}}"

    # Wait a bit for the renew to complete
    sleep 3

    check_log ${REPEATER1} agent_wlan0 "Received credentials for ssid: Multi-AP-24G-1 .* bss_type: 2"
    check_log ${REPEATER1} agent_wlan0 "Received credentials for ssid: Multi-AP-24G-2 .* bss_type: 1"
    check_log ${REPEATER1} agent_wlan2 "ssid: .* teardown"

    mac_agent1_wlan0_hex=0x$(echo $mac_agent1_wlan0 | tr -d :)
    send_CAPI_command ${REPEATER1} "dev_get_parameter,program,map,ruid,${mac_agent1_wlan0_hex},ssid,Multi-AP-24G-1,parameter,macaddr"
    check [ "$capi_command_reply" = "status,COMPLETE,macaddr,$mac_agent1_wlan0" ];

    return $check_error
}

test_ap_config_bss_tear_down() {
    status "test ap config bss tear down"
    # Regression test: MAC address should be case insensitive
    MAC_AGENT1=$(echo $mac_agent1 | tr a-z A-Z)
    # Configure the controller and send renew
    send_CAPI_command ${GATEWAY} "DEV_RESET_DEFAULT" $redirect
    send_CAPI_command ${GATEWAY} "DEV_SET_CONFIG,bss_info1,$mac_agent1 8x Multi-AP-24G-1 0x0020 0x0008 maprocks1 0 1" $redirect

    gw_mac_without_colons="$(printf $mac_gateway | tr -d :)"
    send_CAPI_1905 ${GATEWAY} $mac_agent1 0x000A "tlv_type1,0x01,tlv_length1,0x0006,tlv_value1,0x${gw_mac_without_colons},tlv_type2,0x0F,tlv_length2,0x0001,tlv_value2,{0x00},tlv_type3,0x10,tlv_length3,0x0001,tlv_value3,{0x00}}"

    sleep 3
    check_log ${REPEATER1} agent_wlan0 "ssid: Multi-AP-24G-1, .* fronthaul"
    check_log ${REPEATER1} agent_wlan2 "ssid: .* teardown"
    agent1_wlan0_ssid="$(send_bml_command bml_conn_map | sed -n "/fVAP.*$mac_agent1_wlan0/s/.*ssid: //p")"
    check [ "$agent1_wlan0_ssid" = "Multi-AP-24G-1" ]

    # SSIDs have been removed for the CTT Agent1's front radio
    send_CAPI_command ${GATEWAY} "DEV_SET_CONFIG,bss_info1,$MAC_AGENT1 8x" $redirect
    # Send renew message
    send_CAPI_1905 ${GATEWAY} $mac_agent1 0x000A "tlv_type1,0x01,tlv_length1,0x0006,tlv_value1,0x${gw_mac_without_colons},tlv_type2,0x0F,tlv_length2,0x0001,tlv_value2,{0x00},tlv_type3,0x10,tlv_length3,0x0001,tlv_value3,{0x00}}"

    sleep 3
    check_log ${REPEATER1} agent_wlan0 "ssid: .* teardown"
    agent1_wlan0_ssid="$(send_bml_command bml_conn_map | sed -n "/fVAP.*$mac_agent1_wlan0/s/.*ssid: //p")"
    check [ "$agent1_wlan0_ssid" = "N/A" ]

    return $check_error
}

test_channel_selection() {
    status "test channel selection"
    
    check_error=0
    dbg "Send channel preference query"
    send_CAPI_1905 ${GATEWAY} $mac_agent1 0x8004
    sleep 1
    dbg "Confirming channel preference query has been received on agent"
    check_log ${REPEATER1} agent_wlan0 "CHANNEL_PREFERENCE_QUERY_MESSAGE"
    check_log ${REPEATER1} agent_wlan2 "CHANNEL_PREFERENCE_QUERY_MESSAGE"
    
    dbg "Send channel selection request"
    send_CAPI_1905 ${GATEWAY} $mac_agent1 0x8006
    sleep 1
    dbg "Confirming channel selection request has been received on agent"
    check_log ${REPEATER1} agent_wlan0 "CHANNEL_SELECTION_REQUEST_MESSAGE"
    check_log ${REPEATER1} agent_wlan2 "CHANNEL_SELECTION_REQUEST_MESSAGE"
    
    dbg "Confirming 1905.1 Ack Message request was received on agent"
    # TODO: When creating handler for the ACK message on the agent, replace lookup of this string
    check_log ${REPEATER1} agent_wlan0 "ACK_MESSAGE"
    check_log ${REPEATER1} agent_wlan2 "ACK_MESSAGE"

    return $check_error
}
test_client_capability_query() { 
    status "test client capability"

    check_error=0
    send_CAPI_1905 ${GATEWAY} $mac_agent1 0x8009 "tlv_type,0x90,tlv_length,\
0x000C,tlv_value,{$mac_agent1_wlan0 0x000000110022}"
    sleep 1
    dbg "Confirming client capability query has been received on agent"
    # check that both radio agents received it,in the future we'll add a check to verify which radio the query was intended for.
    check_log ${REPEATER1} agent_wlan0 "CLIENT_CAPABILITY_QUERY_MESSAGE"
    check_log ${REPEATER1} agent_wlan2 "CLIENT_CAPABILITY_QUERY_MESSAGE"
}
test_ap_capability_query() {
    status "test ap capability query"
    check_error=0
    check send_CAPI_1905 ${GATEWAY} $mac_agent1 0x8001
    sleep 1
    
    dbg "Confirming ap capability query has been received on agent"
    check_log ${REPEATER1} agent "AP_CAPABILITY_QUERY_MESSAGE"
    
    dbg "Confirming ap capability report has been received on controller"
    check_log ${GATEWAY} controller "AP_CAPABILITY_REPORT_MESSAGE"
    
    return $check_error
}

test_combined_infra_metrics() {
    status "test combined infrastructure metrics"
    check_error=0
    dbg "Send AP Metrics query message to agent 1"
    check send_CAPI_1905 ${GATEWAY} $mac_agent1 0x800B "tlv_type,0x93,tlv_length,0x0007,tlv_value,0x01 {$mac_agent1_wlan0}"
    check_log ${REPEATER1} agent_wlan0 "Received AP_METRICS_QUERY_MESSAGE"
    # TODO agent should send response autonomously, with same MID.
    # tlv1 == AP metrics TLV
    # tlv2 == STA metrics TLV with no metrics
    # tlv3 == STA metrics TLV for STA connected to this BSS
    # tlv4 == STA traffic stats TLV for same STA
    check send_CAPI_1905 ${REPEATER1} $mac_gateway 0x800C "tlv_type1,0x94,tlv_length1,0x000d,tlv_value1,{$mac_agent1_wlan0} 0x01 0x0002 0x01 0x1f2f3f,\
tlv_type2,0x96,tlv_length2,0x0007,tlv_value2,{55:44:33:22:11:00} 0x00,tlv_type3,0x96,tlv_length3,0x001a,\
tlv_value3,{66:44:33:22:11:00} 0x01 {$mac_agent1_wlan0} 0x11223344 0x1a2a3a4a 0x1b2b3b4b 0x55,\
tlv_type4,0xa2,tlv_length4,0x0022,tlv_value4,{55:44:33:22:11:00} 0x10203040 0x11213141 0x12223242 0x13233343 0x14243444 0x15253545 0x16263646"
    check_log ${GATEWAY} controller "Received AP_METRICS_RESPONSE_MESSAGE"

    dbg "Send AP Metrics query message to agent 2"
    check send_CAPI_1905 ${GATEWAY} $mac_agent2 0x800B "tlv_type,0x93,tlv_length,0x0007,tlv_value,0x01 {$mac_agent2_wlan2}"
    check_log ${REPEATER2} agent_wlan2 "Received AP_METRICS_QUERY_MESSAGE"
    # TODO agent should send response autonomously
    # Same as above but with different STA MAC addresses, different values and skipping the empty one
    check send_CAPI_1905 ${REPEATER2} $mac_gateway 0x800C "tlv_type1,0x94,tlv_length1,0x0010,tlv_value1,{$mac_agent2_wlan2} 0x11 0x1002 0x90 0x1c2c3c 0x1d2d3d,\
tlv_type2,0x96,tlv_length2,0x001a,tlv_value2,{77:44:33:22:11:00} 0x01 {$mac_agent2_wlan2} 0x19293949 0x10203040 0x11213141 0x99,\
tlv_type3,0xa2,tlv_length3,0x0022,tlv_value3,{77:44:33:22:11:00} 0xa0203040 0xa1213141 0xa2223242 0xa3233343 0xa4243444 0xa5253545 0xa6263646"
    check_log ${GATEWAY} controller "Received AP_METRICS_RESPONSE_MESSAGE"

    dbg "Send 1905 Link metric query to agent 1 (neighbor agent 2)"
    check send_CAPI_1905 ${GATEWAY} $mac_agent1 0x0005 "tlv_type,0x08,tlv_length,0x0008,tlv_value,0x01 {$mac_agent2} 0x02"
    check_log ${REPEATER1} agent_wlan0 "Received LINK_METRIC_QUERY_MESSAGE"
    # TODO agent should send response autonomously
    check send_CAPI_1905 ${REPEATER1} $mac_gateway 0x6 "tlv_type1,0x09,tlv_length1,0x0029,\
tlv_value1,{$mac_agent1} {$mac_agent2} {$mac_agent1_wlan0} {$mac_agent2_wlan2} 0x0100 0x01 0x00000000 0x0000e300 0x4230 0x0064 0x0300,\
tlv_type2,0x0a,tlv_length2,0x0023,tlv_value2,{$mac_agent1} {$mac_agent2} {$mac_agent1} {$mac_agent2} 0x0100 0x00000007 0x00020000 0x31"
    check_log ${GATEWAY} controller "Received LINK_METRIC_RESPONSE_MESSAGE"

    # Trigger combined infra metrics
    dbg "Send Combined infrastructure metrics message to agent 1"
    check send_CAPI_1905 ${GATEWAY} $mac_agent1 0x8013
    check_log ${REPEATER1} agent "Received COMBINED_INFRASTRUCTURE_METRICS"
    check_log ${REPEATER1} agent "Received TLV_TRANSMITTER_LINK_METRIC"
    check_log ${REPEATER1} agent "Received TLV_RECEIVER_LINK_METRIC"

    return $check_error
}

test_client_steering_mandate() {
    status "test client steering"
    check_error=0

    dbg "Send topology request to agent 1"
    check send_CAPI_1905 ${GATEWAY} $mac_agent1 0x0002
    sleep 1
    dbg "Confirming topology query was received"
    check_log ${REPEATER1} agent "TOPOLOGY_QUERY_MESSAGE"

    dbg "Send topology request to agent 2"
    check send_CAPI_1905 ${GATEWAY} $mac_agent2 0x0002
    sleep 1
    dbg "Confirming topology query was received"
    check_log ${REPEATER2} agent "TOPOLOGY_QUERY_MESSAGE"

    dbg "Send Client Steering Request message for Steering Mandate to CTT Agent1"
    check send_CAPI_1905 ${GATEWAY} $mac_agent1 0x8014 "tlv_type,0x9B,tlv_length,\
0x001b,tlv_value,{$mac_agent1_wlan0 0xe0 0x0000 0x1388 0x01 {0x000000110022} 0x01 {$mac_agent2_wlan0 0x73 0x24}}"
    sleep 1
    dbg "Confirming Client Steering Request message was received - mandate"
    check_log ${REPEATER1} agent_wlan0 "Got steer request"
    
    dbg "Confirming BTM Report message was received"
    check_log ${GATEWAY} controller "CLIENT_STEERING_BTM_REPORT_MESSAGE"

    dbg "Confirming ACK message was received"
    check_log ${REPEATER1} agent_wlan0 "ACK_MESSAGE"

    check send_CAPI_1905 ${GATEWAY} $mac_agent1 0x8014 "tlv_type,0x9B,tlv_length,\
0x000C,tlv_value,{$mac_agent1_wlan0 0x00 0x000A 0x0000 0x00}"
    sleep 1
    dbg "Confirming Client Steering Request message was received - Opportunity"
    check_log ${REPEATER1} agent_wlan0 "CLIENT_STEERING_REQUEST_MESSAGE"

    dbg "Confirming ACK message was received"
    check_log ${GATEWAY} controller "ACK_MESSAGE"

    dbg "Confirming steering completed message was received"
    check_log ${GATEWAY} controller "STEERING_COMPLETED_MESSAGE"

    dbg "Confirming ACK message was received"
    check_log ${REPEATER1} agent_wlan0 "ACK_MESSAGE"
    return $check_error
}

test_client_association_dummy(){
    status "test client association dummy"
    check_error=0
    sta_mac=11:11:33:44:55:66

    dbg "Connect dummy STA to wlan0"
    send_bwl_event ${REPEATER1} wlan0 "EVENT AP-STA-CONNECTED ${sta_mac}"
    dbg "Send client association control request to the chosen BSSID to steer the client (UNBLOCK) "
    eval send_bml_command "client_allow \"${sta_mac} ${mac_agent1_wlan2}\"" $redirect
    sleep 1

    dbg "Confirming Client Association Control Request message was received (UNBLOCK)"
    check_log ${REPEATER1} agent_wlan2 "Got client allow request for ${sta_mac}"

    dbg "Send client association control request to all other (BLOCK) "
    eval send_bml_command "client_disallow \"${sta_mac} ${mac_agent1_wlan0}\"" $redirect
    sleep 1

    dbg "Confirming Client Association Control Request message was received (BLOCK)"
    check_log ${REPEATER1} agent_wlan0 "Got client disallow request for ${sta_mac}"
    return $check_error
}

wait_for_message() {
    found=0
    timeout=$1
    container=$2
    file=$3
    msg=$4
    command="grep -q '$msg' '$rootdir/logs/$container/$file'"
    for wait in $(seq 1 $timeout); do
        sleep 1
        if eval $command; then
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
    send_bwl_event ${REPEATER1} wlan0 "EVENT AP-STA-CONNECTED 11:22:33:44:55:66"
    dbg "Pre-prepare RRM Beacon Response for association handling task"
    send_bwl_event ${REPEATER1} wlan0 "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-40 rsni=40 bssid=aa:bb:cc:00:00:10"
    dbg "Confirming 11k request is done by association handling task"
    wait_for_message 2 ${REPEATER1} "beerocks_monitor_wlan0.log" "Beacon 11k request to sta 11:22:33:44:55:66 on bssid aa:bb:cc:00:00:10 channel 1"

    dbg "Update Stats"
    send_bwl_event ${REPEATER1} wlan0 "DATA STA-UPDATE-STATS 11:22:33:44:55:66 rssi=-38,-39,-40,-41 snr=38,39,40,41 uplink=1000 downlink=800"
    dbg "Pre-prepare RRM Beacon Responses for optimal path task"
    #Response for IRE1, BSSID of wlan0.0
    send_bwl_event ${REPEATER1} wlan0 "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=aa:bb:cc:11:00:10"
    #Response for IRE1, BSSID of wlan2.0
    send_bwl_event ${REPEATER1} wlan0 "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=aa:bb:cc:11:00:20"
    #Response for IRE2, BSSID of wlan0.0
    send_bwl_event ${REPEATER1} wlan0 "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-40 rsni=40 bssid=aa:bb:cc:00:00:10"
    #Response for IRE2, BSSID of wlan2.0
    send_bwl_event ${REPEATER1} wlan0 "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=aa:bb:cc:00:00:20"
    #Response for GW, BSSID of wlan0.0
    send_bwl_event ${REPEATER1} wlan0 "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=00:11:22:33:00:10"
    #Response for GW, BSSID of wlan2.0
    send_bwl_event ${REPEATER1} wlan0 "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-80 rsni=10 bssid=00:11:22:33:00:20"
    dbg "Confirming 11k request is done by optimal path task"
    wait_for_message 20 ${REPEATER1} "beerocks_monitor_wlan0.log" "Beacon 11k request to sta 11:22:33:44:55:66 on bssid aa:bb:cc:11:00:20 channel 149"

    dbg "Confirming 11k request is done by optimal path task"
    wait_for_message 20 ${REPEATER1} "beerocks_monitor_wlan0.log" "Beacon 11k request to sta 11:22:33:44:55:66 on bssid aa:bb:cc:00:00:20 channel 149"

    dbg "Confirming 11k request is done by optimal path task"
    wait_for_message 20 ${REPEATER1} "beerocks_monitor_wlan0.log" "Beacon 11k request to sta 11:22:33:44:55:66 on bssid aa:bb:cc:11:00:10 channel 1"

    dbg "Confirming 11k request is done by optimal path task"
    wait_for_message 20 ${REPEATER1} "beerocks_monitor_wlan0.log" "Beacon 11k request to sta 11:22:33:44:55:66 on bssid 00:11:22:33:00:20 channel 149"

    dbg "Confirming 11k request is done by optimal path task"
    wait_for_message 20 ${REPEATER1} "beerocks_monitor_wlan0.log" "Beacon 11k request to sta 11:22:33:44:55:66 on bssid 00:11:22:33:00:10 channel 1"

    dbg "Confirming no steer is done"
    wait_for_message 20 ${GATEWAY} "beerocks_controller.log" "could not find a better path for sta 11:22:33:44:55:66"

    # Steer scenario
    dbg "Update Stats"
    send_bwl_event ${REPEATER1} wlan0 "DATA STA-UPDATE-STATS 11:22:33:44:55:66 rssi=-58,-59,-60,-61 snr=18,19,20,21 uplink=100 downlink=80"
    dbg "Pre-prepare RRM Beacon Responses for optimal path task"
    #Response for IRE1, BSSID of wlan0.0
    send_bwl_event ${REPEATER1} wlan0 "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=aa:bb:cc:11:00:10"
    #Response for IRE1, BSSID of wlan2.0
    send_bwl_event ${REPEATER1} wlan0 "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=aa:bb:cc:11:00:20"
    #Response for IRE2, BSSID of wlan0.0
    send_bwl_event ${REPEATER1} wlan0 "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-60 rsni=20 bssid=aa:bb:cc:00:00:10"
    #Response for IRE2, BSSID of wlan2.0
    send_bwl_event ${REPEATER1} wlan0 "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=aa:bb:cc:00:00:20"
    #Response for GW, BSSID of wlan0.0
    send_bwl_event ${REPEATER1} wlan0 "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=1 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=00:11:22:33:00:10"
    #Response for GW, BSSID of wlan2.0
    send_bwl_event ${REPEATER1} wlan0 "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:66 channel=149 dialog_token=0 measurement_rep_mode=0 op_class=0 duration=50 rcpi=-30 rsni=50 bssid=00:11:22:33:00:20"
    dbg "Confirming steering is requested by optimal path task"
    wait_for_message 20 ${GATEWAY} "beerocks_controller.log" "optimal_path_task: steering"

    # Error scenario, sta doesn't support 11k
    dbg "Connect dummy STA to wlan0"
    send_bwl_event ${REPEATER1} wlan0 "EVENT AP-STA-CONNECTED 11:22:33:44:55:77"
    dbg "Pre-prepare RRM Beacon Response with error for association handling task"
    send_bwl_event ${REPEATER1} wlan0 "DATA RRM-BEACON-REP-RECEIVED 11:22:33:44:55:77 channel=0 dialog_token=0 measurement_rep_mode=4 op_class=0 duration=0 rcpi=0 rsni=0 bssid=aa:bb:cc:00:00:10"
    dbg "Confirming 11k request is done by association handling task"
    wait_for_message 20 ${REPEATER1} "beerocks_monitor_wlan0.log" "Beacon 11k request to sta 11:22:33:44:55:77 on bssid aa:bb:cc:00:00:10 channel 1"

    dbg "Confirming STA doesn't support beacon measurement"
    wait_for_message 20 ${GATEWAY} "beerocks_controller.log" "setting sta 11:22:33:44:55:77 as beacon measurement unsupported"

    dbg "Confirming optimal path falls back to RSSI measurements"
    wait_for_message 20 ${GATEWAY} "beerocks_controller.log" "requesting rssi measurements for 11:22:33:44:55:77"
}

test_client_steering_dummy() {
    status "test client steering dummy"
    check_error=0
    sta_mac=11:22:33:44:55:66

    dbg "Connect dummy STA to wlan0"
    send_bwl_event ${REPEATER1} wlan0 "EVENT AP-STA-CONNECTED ${sta_mac}"
    dbg "Send steer request "
    eval send_bml_command "steer_client \"${sta_mac} ${mac_agent1_wlan2}\"" $redirect
    sleep 1

    dbg "Confirming Client Association Control Request message was received (UNBLOCK)"
    check_log ${REPEATER1} agent_wlan2 "Got client allow request"

    dbg "Confirming Client Association Control Request message was received (BLOCK)"
    check_log ${REPEATER1} agent_wlan0 "Got client disallow request"

    dbg "Confirming Client Association Control Request message was received (BLOCK)"
    check_log ${REPEATER2} agent_wlan0 "Got client disallow request"

    dbg "Confirming Client Association Control Request message was received (BLOCK)"
    check_log ${REPEATER2} agent_wlan2 "Got client disallow request"

    dbg "Confirming Client Steering Request message was received - mandate"
    check_log ${REPEATER1} agent_wlan0 "Got steer request"

    dbg "Confirming BTM Report message was received"
    check_log ${GATEWAY} controller "CLIENT_STEERING_BTM_REPORT_MESSAGE"

    dbg "Confirming ACK message was received"
    check_log ${REPEATER1} agent_wlan0 "ACK_MESSAGE"

    dbg "Disconnect dummy STA from wlan0"
    send_bwl_event ${REPEATER1} wlan0 "EVENT AP-STA-DISCONNECTED ${sta_mac}"
    # Make sure that controller sees disconnect before connect by waiting a little
    sleep 1

    dbg "Connect dummy STA to wlan2"
    send_bwl_event ${REPEATER1} wlan2 "EVENT AP-STA-CONNECTED ${sta_mac}"
    dbg "Confirm steering success by client connected"
    check_log ${GATEWAY} controller "steering successful for sta ${sta_mac}"
    check_log ${GATEWAY} controller "sta ${sta_mac} disconnected after successful steering"
    return $check_error
}

test_client_steering_policy() {
    status "test client steering policy"
    check_error=0

    dbg "Send client steering policy to agent 1"
    check send_CAPI_1905 ${GATEWAY} $mac_agent1 0x8003 \
        "tlv_type,0x89,tlv_length,0x000C,tlv_value,{0x00 0x00 0x01 {0x112233445566 0x01 0xFF 0x14}}"
    MID1_STR=$capi_command_mid
    sleep 1
    dbg "Confirming client steering policy has been received on agent"
    
    check_log ${REPEATER1} agent_wlan0 "MULTI_AP_POLICY_CONFIG_REQUEST_MESSAGE"
    sleep 1
    dbg "Confirming client steering policy ack message has been received on the controller"
    check_log ${GATEWAY} controller "ACK_MESSAGE, mid=$MID1_STR"

    return $check_error
}

test_client_association() {
    status "test client association"
    check_error=0
    dbg "Send topology request to agent 1"
    check send_CAPI_1905 ${GATEWAY} $mac_agent1 0x0002
    dbg "Confirming topology query was received"
    check_log ${REPEATER1} agent "TOPOLOGY_QUERY_MESSAGE"

    dbg "Send client association control message"
    check send_CAPI_1905 ${GATEWAY} $mac_agent1 0x8016 "tlv_type,0x9D,tlv_length,\
0x000f,tlv_value,{$mac_agent1_wlan0 0x00 0x1E 0x01 {0x000000110022}}"

    dbg "Confirming client association control message has been received on agent"
    # check that both radio agents received it,in the future we'll add a check to verify which radio the query was intended for.
    check_log ${REPEATER1} agent_wlan0 "CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE"
    check_log ${REPEATER1} agent_wlan2 "CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE"

    dbg "Confirming ACK message was received on controller"
    check_log ${GATEWAY} controller "ACK_MESSAGE"
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
    check send_CAPI_1905 ${GATEWAY} $mac_agent1 0x8018 "tlv_type,0xA0,tlv_length,\
0x04b1,tlv_value,{0x00 $payload}"

    dbg "Confirming higher layer data message was received in the agent" 
    
    check_log ${REPEATER1} agent "HIGHER_LAYER_DATA_MESSAGE"

    dbg "Confirming matching protocol and payload length"

    check_log ${REPEATER1} agent "protocol: 0"
    check_log ${REPEATER1} agent "payload_length: 4b0"

    dbg "Confirming ACK message was received in the controller"
    check_log ${GATEWAY} controller "ACK_MESSAGE"
    return $check_error
}

test_topology() {
    status "test topology query"
    check_error=0
    check send_CAPI_1905 ${GATEWAY} $mac_agent1 0x0002
    dbg "Confirming topology query was received"
    check_log ${REPEATER1} agent "TOPOLOGY_QUERY_MESSAGE"
    return $check_error
}

test_init() {
    status "test initialization"

    start_tcpdump init

    [ "$SKIP_INIT" = "false" ] && {
        eval ${scriptdir}/test_gw_repeater.sh -f -u ${UNIQUE_ID} -g ${GATEWAY} -r "${REPEATER1}" -r "${REPEATER2}" -d 7 $redirect || {
            err "start GW+Repeater failed, abort"
            exit 1
        }
    }
    kill_tcpdump

    #save bml_conn_map output into a file.
    connmap=$(mktemp)
    [ -z "$connmap" ] && { err "Failed to create temp file"; exit 1; }
    trap "rm -f $connmap" EXIT
    docker exec ${GATEWAY} ${installdir}/bin/beerocks_cli -c bml_conn_map > "$connmap"

    mac_gateway=$(grep "GW_BRIDGE" "$connmap" | head -1 | awk '{print $5}' | cut -d ',' -f 1)
    dbg "mac_gateway = ${mac_gateway}"

    mac_agent1=$(grep "IRE_BRIDGE" "$connmap" | head -1 | awk '{print $5}' | cut -d ',' -f 1)
    dbg "mac_agent1 = ${mac_agent1}"
    mac_agent2=$(grep "IRE_BRIDGE" "$connmap" | sed -n 2p | awk '{print $5}' | cut -d ',' -f 1)
    dbg "mac_agent2 = ${mac_agent2}"

    mac_agent1_wlan0=$(docker exec ${REPEATER1} ip -o l list dev wlan0 | sed 's%.*link/ether \([0-9a-f:]*\).*%\1%')
    dbg "mac_agent1_wlan0 = ${mac_agent1_wlan0}"

    mac_agent2_wlan0=$(docker exec ${REPEATER2} ip -o l list dev wlan0 | sed 's%.*link/ether \([0-9a-f:]*\).*%\1%')
    dbg "mac_agent2_wlan0 = ${mac_agent2_wlan0}"

    mac_agent1_wlan2=$(docker exec ${REPEATER1} ip -o l list dev wlan2 | sed 's%.*link/ether \([0-9a-f:]*\).*%\1%')
    dbg "mac_agent1_wlan2 = ${mac_agent1_wlan2}"

    mac_agent2_wlan2=$(docker exec ${REPEATER2} ip -o l list dev wlan2 | sed 's%.*link/ether \([0-9a-f:]*\).*%\1%')
    dbg "mac_agent2_wlan2 = ${mac_agent2_wlan2}"

}
usage() {
    echo "usage: $(basename $0) [-hvds]"
    echo "  options:"
    echo "      -h|--help - show this help menu"
    echo "      -t|--tcpdump - Capture the results with tcpdump (requires root (sudo) or special permission/capability management of interfaces)"
    echo "      -v|--verbose - verbosity on"
    echo "      -s|--stop-on-failure - stop on failure"
    echo "      -u|--unique-id - unique id to add as suffix to container and network names"
    echo "      --skip-init - skip init"
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
}
main() {
    OPTS=`getopt -o 'htvsu:' --long help,tcpdump,verbose,skip-init,stop-on-failure,unique-id: -n 'parse-options' -- "$@"`
    if [ $? != 0 ] ; then err "Failed parsing options." >&2 ; usage; exit 1 ; fi

    eval set -- "$OPTS"

    while true; do
        case "$1" in
            -t | --tcpdump)         TCPDUMP=true; shift ;;
            -v | --verbose)         VERBOSE=true; redirect=; shift ;;
            -h | --help)            usage; exit 0; shift ;;
            -s | --stop-on-failure) STOP_ON_FAILURE=true; shift ;;
            -u | --unique-id)       UNIQUE_ID="$2"; shift; shift ;;
            --skip-init)            SKIP_INIT=true; shift ;;
            -- ) shift; break ;;
            * ) err "unsupported argument $1"; usage; exit 1 ;;
        esac
    done

    REPEATER1=repeater1-${UNIQUE_ID}
    REPEATER2=repeater2-${UNIQUE_ID}
    GATEWAY=gateway-${UNIQUE_ID}

    [ -z "$*" ] && set ${ALL_TESTS}
    info "Tests to run: $*"
    trap kill_tcpdump EXIT
    test_init
    for test in "$@"; do
        start_tcpdump "${test}"
        report "test_${test}" test_${test}
        [ "$STOP_ON_FAILURE" = "true" -a $error -gt 0 ] && exit 1
        kill_tcpdump
        count=$((count+1))
    done

    if [ $error -gt 0 ]; then
        err "$error / $count tests failed"
    else
        success "$count / $count tests passed"
    fi

    exit $error
}

VERBOSE=false
SKIP_INIT=false
STOP_ON_FAILURE=false
TCPDUMP=false
UNIQUE_ID=${SUDO_USER:-${USER}}
main "$@"
