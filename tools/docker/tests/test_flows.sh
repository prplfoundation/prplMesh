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

start_gw_repeater() {
    dbg "delete running containers before starting test"
    eval docker rm -f gateway $redirect
    eval docker rm -f repeater $redirect
    dbg "start gw and repeater"
    eval ${scriptdir}/test_gw_repeater.sh -d 5 $redirect
}

send_bml_command() {       
    docker exec -it gateway ${installdir}/bin/beerocks_cli -c "$*"
}

test_initial_ap_config() {
    status "test initial autoconfig"

    check_error=0
    check docker exec -it repeater sh -c \
        'grep -i -q "WSC Global authentication success" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec -it repeater sh -c \
        'grep -i -q "WSC Global authentication success" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'
    check docker exec -it repeater sh -c \
        'grep -i -q "KWA (Key Wrap Auth) success" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec -it repeater sh -c \
        'grep -i -q "KWA (Key Wrap Auth) success" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'
    check docker exec -it repeater sh -c \
        'grep -i -q "Controller configuration (WSC M2 Encrypted Settings)" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec -it repeater sh -c \
        'grep -i -q "Controller configuration (WSC M2 Encrypted Settings)" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'

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
    eval send_bml_command "bml_wfa_ca_controller \"DEV_SEND_1905,DestALid,$AL_MAC,MessageTypeValue,0x8004\"" $redirect
    sleep 1
    dbg "Confirming channel preference query has been received on agent"
    check docker exec -it repeater sh -c \
        'grep -i -q "CHANNEL_PREFERENCE_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec -it repeater sh -c \
        'grep -i -q "CHANNEL_PREFERENCE_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'
    
    dbg "Send channel selection request"
    eval send_bml_command "bml_wfa_ca_controller \"DEV_SEND_1905,DestALid,$AL_MAC,MessageTypeValue,0x8006\"" $redirect
    sleep 1
    dbg "Confirming channel selection request has been received on agent"
    check docker exec -it repeater sh -c \
        'grep -i -q "CHANNEL_SELECTION_REQUEST_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec -it repeater sh -c \
        'grep -i -q "CHANNEL_SELECTION_REQUEST_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'
    
    dbg "Confirming 1905.1 Ack Message request was received on agent"
    # TODO: When creating handler for the ACK message on the agent, replace lookup of this string
    check docker exec -it repeater sh -c \
        'grep -i -q "handle_cmdu_control_ieee1905_1_message 8000" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    check docker exec -it repeater sh -c \
        'grep -i -q "handle_cmdu_control_ieee1905_1_message 8000" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'

    return $check_error
}

test_ap_capability_info_reporting() {
    #TODO: Implement
    return 1
}

test_client_capability_query() { 
    status "test client capability"  

eval send_bml_command '"bml_wfa_ca_controller \"DEV_SEND_1905,DestALid,$AL_MAC,MessageTypeValue,0x8009,tlv_type,0x90,tlv_length,\
0x000C,tlv_value,{$RADIO_WLAN0_MAC 0x000000110022}\""'    
    sleep 1
    dbg "Confirming client capability query has been received on agent"
    # check that both radio agents received it,in the future we'll add a check to verify which radio the query was intended for.
    docker exec -it repeater sh -c 'grep -i -q "CLIENT_CAPABILITY_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan0.log'
    docker exec -it repeater sh -c 'grep -i -q "CLIENT_CAPABILITY_QUERY_MESSAGE" /tmp/$USER/beerocks/logs/beerocks_agent_wlan2.log'
}
test_link_metric_collection() {
    #TODO: Implement
    return 1
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
    eval send_bml_command "bml_trigger_topology_discovery $AL_MAC" $redirect
    dbg "Confirming topology query was received"
    docker exec -it repeater sh -c 'grep -i -q "Topology Query" /tmp/$USER/beerocks/logs/beerocks_agent.log'
}
test_init() {
    status "test initialization"

    eval ${scriptdir}/test_gw_repeater.sh -f -d 5 $redirect || {
        err "start GW+Repeater failed, abort"
        exit 1
    }
    AL_MAC=$(docker exec -it gateway ${installdir}/bin/beerocks_cli -c bml_conn_map | grep IRE_BRIDGE | awk '{print $5}' | cut -d ',' -f 1)
    RADIO_WLAN0_MAC=$(docker exec -it gateway /home/cor/work/dev1/build/install/bin/beerocks_cli -c bml_conn_map | grep "RADIO: wlan0" | head -1 | awk '{print $4}' | cut -d ',' -f 1 | tr --delete :)    
    RADIO_WLAN0_MAC="0x${RADIO_WLAN0_MAC}"     

    RADIO_WLAN2_MAC=$(docker exec -it gateway ${installdir}/bin/beerocks_cli -c bml_conn_map | grep "RADIO: wlan2" | head -1 | awk '{print $4}' | cut -d ',' -f 1 | tr --delete :)    
    RADIO_WLAN2_MAC="0x${RADIO_WLAN2_MAC}"

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
    echo "      ap_capability_info_reporting - AP Capability info reporting test"
    echo "      client_capability_info_reporting - Capability info reporting test"
    echo "      client_steering_mandate - Client Steering for Steering Mandate and Steering Opportunity test"
    echo "      client_steering_policy - Setting Client Steering Policy test"
    echo "      client_association - Client Association Control Message test"
    echo "      ap_capability_query - AP Capability query test"
    echo "      client_capability_query - Client Capability info reporting test"
    echo "      combined_infra_metrics - Link metric collection test"
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
    else
        success "$count / $count tests passed"
    fi

    exit $error
}

VERBOSE=false
AL_MAC=
RADIO_WLAN0_MAC=
RADIO_WLAN2_MAC=

main $@
