#!/bin/sh

ap_to_repeater() 
{
    # Kill APM
    /opt/beerocks/apm_utils.sh stop

    uci set beerocks.config.management_mode="Proprietary-Mesh"
    uci set beerocks.config.operating_mode="WDS-Repeater"  
    uci commit beerocks

    uci set wireless.default_radio26.disabled='0'
    uci set wireless.default_radio58.disabled='0'
    uci commit wireless

    # Reset hostapd's
    wifi
}

repeater_to_ap() 
{
    # Kill APM
    /opt/beerocks/apm_utils.sh stop

    # Kill DHCP client
    ps -A | grep [d]hcp | awk '{print $1;}' | xargs kill

    # start DHCP server
    dnsmasq -u nobody --dhcp-authoritative -P 4096 -C /var/dnsmasq.conf

    # Reset hostapd's
    wifi
}

static_init()
{
    # Check if repeater mode
    if [ $(uci get beerocks.config.management_mode) != "Proprietary-Mesh" ]
    then 
        echo "not Repeater"
        exit 1;
    fi
    if [ $(uci get beerocks.config.operating_mode) != "WDS-Repeater" ]
    then 
        echo "not Repeater"
        exit 1;
    fi
    
    echo "I am repeater!"

    # Kill APM
    echo "Kill APM"
    /opt/beerocks/apm_utils.sh stop

    # Kill DHCP server
    echo "Kill DHCP server"
    ps -A | grep [d]ns | awk '{print $1;}' | xargs kill

    # Set bridge ip to 0.0.0.0
    echo "Set bridge ip to 0.0.0.0"
    ifconfig brlan0 0.0.0.0

    # Set dhcpcd conf to timeout 0
    echo "Set dhcpcd conf to timeout 0"
    cp /etc/dhcpcd.conf /var/run/dhcpcd.conf
    echo "timeout 0" >> /var/run/dhcpcd.conf

    # Start dhcp client
    echo "Start dhcp client"
    dhcpcd brlan0 -f /var/run/dhcpcd.conf

    # Start APM
    echo "Start APM"
    /opt/beerocks/apm_utils.sh start
}

case $1 in
        "ap_to_repeater")
                ap_to_repeater
                ;;
        "repeater_to_ap")
                repeater_to_ap
                ;;
        "start")
                static_init
                ;;
        *)
        echo "Usage: $0 {ap_to_repeater|start}"
esac

