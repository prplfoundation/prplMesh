#!/bin/sh
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 Gal Wiener (Intel), Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

ALL_TESTS="topology initial_ap_config ap_config_renew ap_config_bss_tear_down channel_selection
           ap_capability_query client_capability_query combined_infra_metrics
           client_steering_mandate client_steering_dummy client_steering_policy client_association
           higher_layer_data_payload_trigger higher_layer_data_payload"

scriptdir="$(cd "${0%/*}"; pwd)"
topdir="${scriptdir%/*/*/*/*}"
. ${topdir}/prplMesh/tools/docker/functions.sh

redirect="> /dev/null 2>&1"
error=0

send_bml_command() {       
    docker exec -it gateway ${installdir}/bin/beerocks_cli -c "$*"
}

test_initial_ap_config() {
    status "test initial autoconfig"

    check_error=0
    check docker exec -it repeater1 sh -c \
        'grep -i -q "WSC Global authentication success" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec -it repeater1 sh -c \
        'grep -i -q "WSC Global authentication success" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'
    check docker exec -it repeater1 sh -c \
        'grep -i -q "KWA (Key Wrap Auth) success" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec -it repeater1 sh -c \
        'grep -i -q "KWA (Key Wrap Auth) success" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'
    check docker exec -it repeater1 sh -c \
        'grep -i -q "Controller configuration (WSC M2 Encrypted Settings)" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec -it repeater1 sh -c \
        'grep -i -q "Controller configuration (WSC M2 Encrypted Settings)" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'

    # Configure the controller and send renew
    eval send_bml_command "bml_wfa_ca_controller \"DEV_RESET_DEFAULT\"" $redirect
    eval send_bml_command "bml_wfa_ca_controller \\\"DEV_SET_CONFIG,bss_info1,$mac_agent1 8x Multi-AP-24G-1 0x0020 0x0008 maprocks1 0 1,bss_info2,$mac_agent1 8x Multi-AP-24G-2 0x0020 0x0008 maprocks2 1 0\\\"" $redirect
    gw_mac_without_colons="$(printf $mac_gateway | tr -d :)"
    eval send_bml_command "bml_wfa_ca_controller \"DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x000A,tlv_type1,0x01,tlv_length1,0x0006,tlv_value1,0x${gw_mac_without_colons},tlv_type2,0x0F,tlv_length2,0x0001,tlv_value2,{0x00},tlv_type3,0x10,tlv_length3,0x0001,tlv_value3,{0x00}}\"" $redirect

    # Wait a bit for the renew to complete
    sleep 3

    check docker exec -it repeater1 sh -c \
        'grep -i -q "ssid: Multi-AP-24G-1, .* fronthaul" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec -it repeater1 sh -c \
        'grep -i -q "ssid: Multi-AP-24G-2, .* backhaul" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec -it repeater1 sh -c \
        'grep -i -q "ssid: .* teardown" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'

    return $check_error
}

test_ap_config_renew() {
    #TODO: Implement
    return 1
}

test_ap_config_bss_tear_down() {
    #TODO: Implement
    return 1
}

test_channel_selection() {
    status "test channel selection"
    
    check_error=0
    dbg "Send channel preference query"
    eval send_bml_command "bml_wfa_ca_controller \"DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8004\"" $redirect
    sleep 1
    dbg "Confirming channel preference query has been received on agent"
    check docker exec -it repeater1 sh -c \
        'grep -i -q "CHANNEL_PREFERENCE_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec -it repeater1 sh -c \
        'grep -i -q "CHANNEL_PREFERENCE_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'
    
    dbg "Send channel selection request"
    eval send_bml_command "bml_wfa_ca_controller \"DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8006\"" $redirect
    sleep 1
    dbg "Confirming channel selection request has been received on agent"
    check docker exec -it repeater1 sh -c \
        'grep -i -q "CHANNEL_SELECTION_REQUEST_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec -it repeater1 sh -c \
        'grep -i -q "CHANNEL_SELECTION_REQUEST_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'
    
    dbg "Confirming 1905.1 Ack Message request was received on agent"
    # TODO: When creating handler for the ACK message on the agent, replace lookup of this string
    check docker exec -it repeater1 sh -c \
        'grep -i -q "ACK_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec -it repeater1 sh -c \
        'grep -i -q "ACK_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'

    return $check_error
}
test_client_capability_query() { 
    status "test client capability"

    check_error=0
    eval send_bml_command '"bml_wfa_ca_controller \"DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8009,tlv_type,0x90,tlv_length,\
0x000C,tlv_value,{$mac_agent1_wlan0 0x000000110022}\""' $redirect
    sleep 1
    dbg "Confirming client capability query has been received on agent"
    # check that both radio agents received it,in the future we'll add a check to verify which radio the query was intended for.
    check docker exec -it repeater1 sh -c \
        'grep -i -q "CLIENT_CAPABILITY_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec -it repeater1 sh -c \
        'grep -i -q "CLIENT_CAPABILITY_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'
}
test_ap_capability_query() {
    status "test ap capability query"
    eval send_bml_command "bml_wfa_ca_controller \"DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8001\"" $redirect
    sleep 1
    dbg "Confirming ap capability query has been received on agent"
    docker exec -it repeater1 sh -c 'grep -i -q "AP_CAPABILITY_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
}
test_combined_infra_metrics() {
    #TODO: Implement
    return 1
}
test_client_steering_mandate() {
    status "test client steering"
    check_error=0

    dbg "Send topology request to agent 1"
    eval send_bml_command "bml_wfa_ca_controller \"DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x0002\"" $redirect
    sleep 1
    dbg "Confirming topology query was received"
    check docker exec -it repeater1 sh -c \
        'grep -i -q "TOPOLOGY_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent.log'

    dbg "Send topology request to agent 2"
    eval send_bml_command "bml_wfa_ca_controller \"DEV_SEND_1905,DestALid,$mac_agent2,MessageTypeValue,0x0002\"" $redirect
    sleep 1
    dbg "Confirming topology query was received"
    check docker exec -it repeater2 sh -c \
        'grep -i -q "TOPOLOGY_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent.log'

    dbg "Send Client Steering Request message for Steering Mandate to CTT Agent1"
    eval send_bml_command '"bml_wfa_ca_controller \"DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8014,tlv_type,0x9B,tlv_length,\
0x001b,tlv_value,{$mac_agent1_wlan0 0xe0 0x0000 0x1388 0x01 {0x000000110022} 0x01 {$mac_agent2_wlan0 0x73 0x24}}\""' $redirect
    sleep 1
    dbg "Confirming Client Steering Request message was received - mandate"
    check docker exec -it repeater1 sh -c \
        'grep -i -q "Got steer request" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    
    dbg "Confirming BTM Report message was received"
    check docker exec -it gateway sh -c \
        'grep -i -q "CLIENT_STEERING_BTM_REPORT_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_controller.log'

    dbg "Confirming ACK message was received"
    check docker exec -it repeater1 sh -c \
        'grep -i -q "ACK_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'

    eval send_bml_command '"bml_wfa_ca_controller \"DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8014,tlv_type,0x9B,tlv_length,\
0x000C,tlv_value,{$mac_agent1_wlan0 0x00 0x000A 0x0000 0x00}\""' $redirect   
    sleep 1
    dbg "Confirming Client Steering Request message was received - Opportunity"
    check docker exec -it repeater1 sh -c \
        'grep -i -q "CLIENT_STEERING_REQUEST_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'

    dbg "Confirming ACK message was received"
    check docker exec -it gateway sh -c \
        'grep -i -q "ACK_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_controller.log'

    dbg "Confirming steering completed message was received"
    check docker exec -it gateway sh -c \
        'grep -i -q "STEERING_COMPLETED_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_controller.log'

    dbg "Confirming ACK message was received"
    check docker exec -it repeater1 sh -c \
        'grep -i -q "ACK_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
}

test_client_steering_dummy() {
    status "test client steering dummy"
    check_error=0

    dbg "Connect dummy STA to wlan0"
    check docker exec -it repeater1 sh -c \
        'echo "AP-STA-CONNECTED 11:22:33:44:55:66" > /tmp/$USER/beerocks/wlan0/EVENT'

    dbg "Send steer request "
    eval send_bml_command "steer_client \"11:22:33:44:55:66 aa:bb:cc:00:00:20\"" $redirect
    sleep 1

    dbg "Confirming Client Steering Request message was received - mandate"
    check docker exec -it repeater1 sh -c \
        'grep -i -q "Got steer request" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'

    dbg "Confirming BTM Report message was received"
    check docker exec -it gateway sh -c \
        'grep -i -q "CLIENT_STEERING_BTM_REPORT_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_controller.log'

    dbg "Confirming ACK message was received"
    check docker exec -it repeater1 sh -c \
        'grep -i -q "ACK_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'

    dbg "Disconnect dummy STA from wlan0"
    check docker exec -it repeater1 sh -c \
        'echo "AP-STA-DISCONNECTED 11:22:33:44:55:66" > /tmp/$USER/beerocks/wlan0/EVENT'

    #TODO// check for "disconnected after successful steering, proceeding to unblock" message 

    dbg "Connect dummy STA to wlan2"
    check docker exec -it repeater1 sh -c \
        'echo "AP-STA-CONNECTED 11:22:33:44:55:66" > /tmp/$USER/beerocks/wlan2/EVENT'

    dbg "Confirm steering success by client connected"
    check docker exec -it gateway sh -c \
        'grep -i -q "steering successful for sta 11:22:33:44:55:66" /tmp/$USER/beerocks/logs/beerocks_controller.log'
}

test_client_steering_policy() {
    status "test client steering policy"
    rm /tmp/catch

    dbg "Send client steering policy to agent 1"
    eval send_bml_command '"bml_wfa_ca_controller \"DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8003,tlv_type,0x89,tlv_length\
,0x000C,tlv_value,{0x00 0x00 0x01 {0x112233445566 0x01 0xFF 0x14}}\""' > /tmp/catch
    sleep 1
    MID_STR=$(grep -Po "(?<=mid,0x).*[^\s]" /tmp/catch)
    MID1=$(echo "ibase=16; $MID_STR" | bc)
    dbg "Confirming client steering policy has been received on agent"
    
    docker exec -it repeater1 sh -c 'grep -i -q "MULTI_AP_POLICY_CONFIG_REQUEST_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log > /tmp/catch'
    sleep 1
    dbg "Confirming client steering policy ack message has been received on the controller"
    TMP="$(docker exec -it gateway sh -c 'grep -i "ACK_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_controller.log')"
    MID_STR=$(echo "$TMP" | tr ' ' '\n' | grep -Po "(?<=mid=).*[^\s]")
    MID2=$(echo "ibase=16; $MID_STR" | bc)

    if [ "$MID1" -ne "$MID2" ]; then
        return 1
    fi
}

test_client_association() {
    status "test client association"
    check_error=0
    dbg "Send topology request to agent 1"
    eval send_bml_command "bml_wfa_ca_controller \"DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x0002\"" $redirect
    dbg "Confirming topology query was received"
    check docker exec -it repeater1 sh -c \
        'grep -i -q "TOPOLOGY_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent.log'

    dbg "Send client association control message"
    eval send_bml_command '"bml_wfa_ca_controller \"DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8016,tlv_type,0x9D,tlv_length,\
0x000f,tlv_value,{$mac_agent1_wlan0 0x00 0x1E 0x01 {0x000000110022}}\""' $redirect

    dbg "Confirming client association control message has been received on agent"
    # check that both radio agents received it,in the future we'll add a check to verify which radio the query was intended for.
    check docker exec -it repeater1 sh -c \
        'grep -i -q "CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec -it repeater1 sh -c \
        'grep -i -q "CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'

    dbg "Confirming ACK message was received on controller"
    check docker exec -it gateway sh -c \
        'grep -i -q "ACK_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_controller.log'
}

test_higher_layer_data_payload_trigger() {
    status "test higher layer data payload"
    check_error=0
    mac_gateway_hex=$(mac_to_hex $mac_gateway)
    dbg "mac_gateway_hex = ${mac_gateway_hex}"
    copies=200
    
    for i in `seq 1 $copies`
    do
        payload="$payload $mac_gateway_hex"
    done

    dbg "Send Higher Layer Data message"
    # MCUT sends Higher Layer Data message to CTT Agent1 by providing:
    # Higher layer protocol = "0x00"
    # Higher layer payload = 200 concatenated copies of the ALID of the MCUT (1200 octets)
    eval send_bml_command '"bml_wfa_ca_controller \"DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8018,tlv_type,0xA0,tlv_length,\
0x04b1,tlv_value,{0x00 $payload}\""' $redirect

    dbg "Confirming higher layer data message was received in the agent" 
    
    check docker exec -it repeater1 sh -c \
        'grep -i -q "HIGHER_LAYER_DATA_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent.log'

    dbg "Confirming matching protocol and payload length"

    check docker exec -it repeater1 sh -c \
        'grep -i -q "protocol: 0" /tmp/$USER/beerocks/logs/beerocks_agent.log'
    check docker exec -it repeater1 sh -c \
        'grep -i -q "payload_length: 4b0" /tmp/$USER/beerocks/logs/beerocks_agent.log'

    dbg "Confirming ACK message was received in the controller"
    check docker exec -it gateway sh -c \
        'grep -i -q "ACK_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_controller.log'
}

test_higher_layer_data_payload() {
    #TODO: Implement
    return 1
}
test_topology() {
    status "test topology query"
    eval send_bml_command "bml_wfa_ca_controller \"DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x0002\"" $redirect
    dbg "Confirming topology query was received"
    docker exec -it repeater1 sh -c 'grep -i -q "TOPOLOGY_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent.log'
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
    docker exec -it gateway ${installdir}/bin/beerocks_cli -c bml_conn_map > "$connmap"

    mac_gateway=$(grep "GW_BRIDGE" "$connmap" | head -1 | awk '{print $5}' | cut -d ',' -f 1)
    dbg "mac_gateway = ${mac_gateway}"

    mac_agent1=$(grep "IRE_BRIDGE" "$connmap" | head -1 | awk '{print $5}' | cut -d ',' -f 1)
    dbg "mac_agent1 = ${mac_agent1}"
    mac_agent2=$(grep "IRE_BRIDGE" "$connmap" | sed -n 2p | awk '{print $5}' | cut -d ',' -f 1)
    dbg "mac_agent2 = ${mac_agent2}"

    mac_agent1_wlan0=$(mac_to_hex $(grep "RADIO: wlan0" "$connmap" | head -1 | awk '{print $4}' | cut -d ',' -f 1))
    dbg "mac_agent1_wlan0 = ${mac_agent1_wlan0}"

    mac_agent2_wlan0=$(mac_to_hex $(grep "RADIO: wlan0" "$connmap" | sed -n 2p | awk '{print $4}' | cut -d ',' -f 1))
    dbg "mac_agent2_wlan0 = ${mac_agent2_wlan0}"

    mac_agent1_wlan2=$(mac_to_hex $(grep "RADIO: wlan2" "$connmap" | head -1 | awk '{print $4}' | cut -d ',' -f 1))
    dbg "mac_agent1_wlan2 = ${mac_agent1_wlan2}"

    mac_agent2_wlan2=$(mac_to_hex $(grep "RADIO: wlan2" "$connmap" | sed -n 2p | awk '{print $4}' | cut -d ',' -f 1))
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
    [ -z "$@" ] && set ${ALL_TESTS}
    echo $@
    test_init
    for test in $@; do
       report "test_${test}" test_${test}
       count=$((count+1))
    done

    if [ $error -gt 0 ]; then
        err "$error / $count tests failed"
        if [ "$VERBOSE" == "true" ]; then
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

main $@
