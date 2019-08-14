###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 Gal Wiener (Intel), Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

ALL_TESTS="topology initial_ap_config ap_config_renew ap_config_bss_tear_down channel_selection
           ap_capability_info_reporting client_capability_info_reporting link_metric_collection
           steering_mandate_opportunity client_steering client_association backhaul_optimization
           layer_data_payload_trigger layer_data_payload"

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
    #TODO: Implement
    return 1
}

test_ap_capability_info_reporting() {
    #TODO: Implement
    return 1
}

test_client_capability_info_reporting() {
    #TODO: Implement
    return 1
}

test_link_metric_collection() {
    #TODO: Implement
    return 1
}

test_steering_mandate_opportunity() {
    #TODO: Implement
    return 1
}

test_client_steering() {
    #TODO: Implement
    return 1
}

test_client_association() {
    #TODO: Implement
    return 1
}

test_backhaul_optimization() {
    #TODO: Implement
    return 1
}

test_layer_data_payload_trigger() {
    #TODO: Implement
    return 1
}

test_layer_data_payload() {
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
    start_gw_repeater || {
        err "start GW+Repeater failed, abort"
        exit 1
    }
    AL_MAC=$(docker exec -it gateway ${installdir}/bin/beerocks_cli -c bml_conn_map | grep IRE_BRIDGE | awk '{print $5}' | cut -d ',' -f 1)
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
    echo "      link_metric_collection - Link metric collection test"
    echo "      steering_mandate_opportunity - Client Steering for Steering Mandate and Steering Opportunity test"
    echo "      client_steering - Setting Client Steering Policy test"
    echo "      client_association - Client Association Control Message test"
    echo "      backhaul_optimization - Backhaul optimization test"
    echo "      layer_data_payload_trigger - Higher layer data payload over 1905 trigger test"
    echo "      layer_data_payload - Higher layer data payload over 1905 test"
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

main $@
