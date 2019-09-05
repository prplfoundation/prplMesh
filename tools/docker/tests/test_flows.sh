#!/bin/sh
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 Gal Wiener (Intel), Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

ALL_TESTS="topology initial_ap_config ap_config_renew ap_config_bss_tear_down channel_selection
           ap_capability_query client_capability_query combined_infra_metrics
           client_steering_mandate client_steering_policy client_association
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
        'grep -i -q "handle_cmdu_control_ieee1905_1_message 8000" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec -it repeater1 sh -c \
        'grep -i -q "handle_cmdu_control_ieee1905_1_message 8000" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'

    return $check_error
}
test_client_capability_query() { 
    status "test client capability"  

    eval send_bml_command '"bml_wfa_ca_controller \"DEV_SEND_1905,DestALid,$mac_agent1,MessageTypeValue,0x8009,tlv_type,0x90,tlv_length,\
0x000C,tlv_value,{$mac_agent1_wlan0 0x000000110022}\""' $redirect
    sleep 1
    dbg "Confirming client capability query has been received on agent"
    # check that both radio agents received it,in the future we'll add a check to verify which radio the query was intended for.
    docker exec -it repeater1 sh -c 'grep -i -q "CLIENT_CAPABILITY_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    docker exec -it repeater1 sh -c 'grep -i -q "CLIENT_CAPABILITY_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'
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
    #TODO: Implement
    return 1
}

test_client_steering_policy() {
    #TODO: Implement
    return 1
}

test_client_association() {
    #TODO: Implement
    return 1
}

test_higher_layer_data_payload_trigger() {
    #TODO: Implement
    return 1
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

    mac_agent1_wlan0=$(grep "RADIO: wlan0" "$connmap" | head -1 | awk '{print $4}' | cut -d ',' -f 1 | tr --delete :)
    mac_agent1_wlan0="0x${mac_agent1_wlan0}"
    dbg "mac_agent1_wlan0 = ${mac_agent1_wlan0}"

    mac_agent2_wlan0=$(grep "RADIO: wlan0" "$connmap" | sed -n 2p | awk '{print $4}' | cut -d ',' -f 1 | tr --delete :)
    mac_agent2_wlan0="0x${mac_agent2_wlan0}"
    dbg "mac_agent2_wlan0 = ${mac_agent2_wlan0}"

    mac_agent1_wlan2=$(grep "RADIO: wlan2" "$connmap" | head -1 | awk '{print $4}' | cut -d ',' -f 1 | tr --delete :)
    mac_agent1_wlan2="0x${mac_agent1_wlan2}"
    dbg "mac_agent1_wlan2 = ${mac_agent1_wlan2}"

    mac_agent2_wlan2=$(grep "RADIO: wlan2" "$connmap" | sed -n 2p | awk '{print $4}' | cut -d ',' -f 1 | tr --delete :)
    mac_agent2_wlan2="0x${mac_agent2_wlan2}"
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
