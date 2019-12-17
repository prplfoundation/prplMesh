#!/bin/sh
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 Adam Dov (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################
ap_to_repeater() 
{
    echo "ap_to_repeater - start"
    #configure prplmesh to repeater mode
    ubus call uci set '{ "config" : "prplmesh" ,"section" : "config", 
        "values" : {"management_mode" : "Proprietary-Mesh",
                    "operating_mode" : "WDS-Repeater",
        }}'
    ubus call uci commit '{"config" : "prplmesh"}'

    # disable DHCP server on br-lan
    ubus call uci set '{ "config" : "dhcp", "section" : "@dnsmasq[0]", "values" : { "listenaddress" : "" }}'
    ubus call uci commit '{"config" : "dhcp"}'

    # Start dhcp client on br-lan
    ubus call uci set '{ "config" : "network", "section" : "lan", "values" : { "proto" : "dhcp" }}'
    ubus call uci commit '{"config" : "network"}'
    
    # FH configuration for client connection (optional)
    ubus call uci set '{ "config" : "wireless" ,"section" : "default_radio13", 
        "values" : {"ssid" : "ssid_RPT_2.4G",
                    "key_mgmt" : "NONE"
        }}'
    
    #enable STAs - for wireless BH connection
    #2.4G
    ubus call uci set '{ "config" : "wireless" ,"section" : "default_radio26",
        "values" : {"wds" : "1",
                    "vendor_elems" : "13 dd050017353001 14 dd050017353001",
                    "full_ch_master_control" : "1",
                    "disabled" : "0"
        }}'
    
    #5G
    ubus call uci set '{ "config" : "wireless" ,"section" : "default_radio58",
        "values" : {"wds" : "1",
                    "vendor_elems" : "13 dd050017353001 14 dd050017353001",
                    "full_ch_master_control" : "1",
                    "disabled" : "0"
        }}'

    #remove ssid configuration to prevent STAs from scanning before prpl request to scan
    #this is required to prevent "FAIL-BUSY" reply from STAs
    ubus call uci delete '{ "config" : "wireless", "section" : "default_radio26", "option" : "ssid" }'
    ubus call uci delete '{ "config" : "wireless", "section" : "default_radio58", "option" : "ssid" }'
    
    #uci commit wireless (currently automatically restarts the hostapd's)
    ubus call uci commit '{"config" : "wireless"}'

     #(The daemon shouldn't perform this operation directly - but via UCI)
    #add interfaces to bridge
    #brctl addif br-lan wlan1
    #brctl addif br-lan wlan3
    echo "ap_to_repeater - success"
}
repeater_to_ap()
{
    echo "repeater_to_ap - done"
    # start DHCP server on br-lan (automatically stops dhcp client as well)
    ubus call uci set '{ "config" : "network", "section" : "lan", "values" : { "proto" : "static" }}'
    ubus call uci commit '{"config" : "network"}'

    #configure prplmesh to GW-only mode
    ubus call uci set '{ "config" : "prplmesh" ,"section" : "config",
        "values" : {"management_mode" : "Multi-AP-Controller-and-Agent",
                    "operating_mode" : "Gateway"
        }}'
    ubus call uci commit '{"config" : "prplmesh"}'
    
    #configure wireless to disable STAs (which are not requried in GW-only mode)
    ubus call uci set '{ "config" : "wireless" ,"section" : "default_radio26", "values" : {"disabled" : "1"}}'
    ubus call uci set '{ "config" : "wireless" ,"section" : "default_radio58", "values" : {"disabled" : "1"}}'
    
    ubus call uci commit '{"config" : "wireless" }'
    echo "repeater_to_ap - success"
}
is_repeater_configured()
{
    # Check if repeater mode (and required functionality is supported)
    if [ $(uci get prplmesh.config.management_mode) != "Proprietary-Mesh" ]
    then 
        echo "not Repeater"
        exit 1;
    fi
    if [ $(uci get prplmesh.config.operating_mode) != "WDS-Repeater" ]
    then 
        echo "not Repeater"
        exit 1;
    fi
    if [ -z $(which wpa_supplicant) ]
    then 
        echo "no wpa_supplicant"
        exit 1;
    fi
    if [ -z $(uci show wireless | grep sta) ]
    then 
        echo "station not configurated, try to run /opt/intel/wave/scripts/wave_factory_reset.sh"
        exit 1;
    fi

    echo "Repeater is configured - use prplmesh_utils.sh start to run Repeater"
}
case $1 in
        "ap_to_repeater")
                ap_to_repeater
                ;;
        "repeater_to_ap")
                repeater_to_ap
                ;;
        "is_repeater_configured")
                is_repeater_configured
                ;;
        *)
        echo "Usage: $0 {ap_to_repeater|repeater_to_ap|is_repeater_configured}"
esac

