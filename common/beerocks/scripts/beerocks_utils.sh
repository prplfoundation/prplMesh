#!/bin/sh
#############################################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2016-2019 Intel Corporation
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
#############################################################################
# uncomment below to debug
# set -x

SIG_TERM=-15
SIG_KILL=-9

MULTIAP_DEPLOY_ARCHIVE=/tmp/multiap/deploy/multiap_deploy.tar.gz
BEEROCKS_PATH=/opt/beerocks
BEEROCKS_LIB_PATH=/usr/lib

if [ -d "/opt/lantiq/wave" ] # if UGW
then
    BEEROCKS_WAVE_PATH=/opt/lantiq/wave
else
    BEEROCKS_WAVE_PATH=/etc/wave
fi

BEEROCKS_TMP_PATH=/tmp/beerocks

BEEROCKS_LOGS_PATH=/rdklogs/logs

BEEROCKS_ERRORS_FILE=$BEEROCKS_LOGS_PATH/beerocks_errors.log
UGW_DB_BEEROCKS="object:Device.X_INTEL_COM_BEEROCKS"
UGW_DB_BEEROCKS_WIFI="object:Device.X_INTEL_COM_BEEROCKS.WiFi"
# Default values
DEFAULT_SSID="beerocks_demo"
DEAFULT_SECURITY="WPA2-Personal"
DEFAULT_PASS="test1234"
DEFAULT_IP="192.168.1.1"
DEFAULT_DFS="0"

killall_program()
{
    PROGRAM_NAME=$1
    if [ "$#" -eq 2 ]; then
        KILL_SIG=$2
    else
        KILL_SIG=$SIG_TERM
    fi
    for PID in $(ps -ef | grep $PROGRAM_NAME | grep -v grep | grep -v beerocks_utils.sh | grep -v vi | grep -v tail | awk '{print $1}'); do
        echo "kill $KILL_SIG $PID $PROGRAM_NAME";
        kill $KILL_SIG $PID > /dev/null 2>&1;
    done
}


get_hostap_iface_list()
{
    IFACE_LIST=`grep -e "^hostap_iface=" $BEEROCKS_PATH/beerocks_agent.conf | awk -F'[/=]' '{print $2}'`
    IFACE_LIST=`echo $IFACE_LIST | sed -e "s|\n| |g"`
    for IFACE in $IFACE_LIST ; do
        IFACE_EXISTS=`ifconfig -a | grep $IFACE`
        if [ ! -z "$IFACE_EXISTS" ]; then
            printf $IFACE" "
        fi
    done
}

get_wpa_supplicant_iface_list()
{
    IFACE_LIST=`grep -e "^sta_iface=" $BEEROCKS_PATH/beerocks_agent.conf | awk -F'[/=]' '{print $2}'`
    IFACE_LIST=`echo $IFACE_LIST | sed -e "s|\n| |g"`
    for IFACE in $IFACE_LIST ; do
        IFACE_EXISTS=`ifconfig -a | grep $IFACE`
        if [ ! -z "$IFACE_EXISTS" ]; then
            printf $IFACE" "
        fi
    done
}

start_beerocks_watchdog()
{
    echo "Start beerocks_watchdog"
    ./beerocks_watchdog.sh&
}

clear_4addr_list()
{
    if [ "$#" -eq 1 ]; then
        IFACE_LIST=$1
    else
        IFACE_LIST=$( get_hostap_iface_list )
    fi

    echo "Clear hostapd's 4 address list"
    {
        for IFACE in $IFACE_LIST ; do
            iwpriv $IFACE gFourAddrStas | while read -r line; do iwpriv $IFACE sDelFourAddrSta "$line"; done
        done

    } &> /dev/null
}

reset_hostapd()
{
    if [ "$#" -eq 1 ]; then
        IFACE_LIST=$1
    else
        IFACE_LIST=$( get_hostap_iface_list )
    fi

    for IFACE in $IFACE_LIST ; do
        killall_program hostapd_$IFACE $SIG
    done

    clear_4addr_list $IFACE_LIST

    for IFACE in $IFACE_LIST ; do
        echo "Restart /tmp/hostapd_$IFACE"
        iwpriv $IFACE sEnableRadio 0
        /tmp/hostapd_$IFACE /tmp/wlan_wave/hostapd_$IFACE.conf -t 2>&1 > $BEEROCKS_LOGS_PATH/logs/hostapd_$IFACE.log &
    done
}

kill_hostapd()
{
    if [ "$#" -eq 1 ]; then
        IFACE_LIST=$1
    else
        IFACE_LIST=$( get_hostap_iface_list )
    fi

    for IFACE in $IFACE_LIST ; do
        killall_program hostapd_$IFACE $SIG
    done

    clear_4addr_list $IFACE_LIST   
}


reset_wpa_supplicant()
{
    if [ $BEEROCKS_LOCAL_GATEWAY == "1" ]; then
        return
    fi

    if [ "$#" -eq 1 ]; then
        IFACE_LIST=$1
    else
        IFACE_LIST=$( get_wpa_supplicant_iface_list )
    fi

    for IFACE in $IFACE_LIST ; do
        killall_program wpa_supplicant_$IFACE $SIG
    done

    for IFACE in $IFACE_LIST ; do
        echo "Restart /tmp/wpa_supplicant_$IFACE"
        ifconfig $IFACE up
        iw $IFACE set 4addr off
        /tmp/wpa_supplicant_$IFACE -Dnl80211 -i $IFACE -c /tmp/wlan_wave/wpa_supplicant_$IFACE.conf -e /tmp/wpa_supplicant_ent_$IFACE -B root
    done
}

kill_wpa_supplicant()
{
    if [ $BEEROCKS_LOCAL_GATEWAY == "1" ]; then
        return
    fi

    if [ "$#" -eq 0 ]; then
        IFACE_LIST=$( get_wpa_supplicant_iface_list )
    elif [ "$#" -eq 1 ]; then
        IFACE_LIST=$1
    elif [ "$#" -eq 2 ]; then
        IFACE_LIST=$1
        SIG=$2
    fi

    for IFACE in $IFACE_LIST ; do
        killall_program wpa_supplicant_$IFACE $SIG
    done
}

get_beerocks_config()
{
    echo ${UGW_DB_BEEROCKS} > ${BEEROCKS_TMP_PATH}/tmp_cal_config
    caltest -g ${BEEROCKS_TMP_PATH}/tmp_cal_config -c WEB
}

enable_wlan_radios()
{
    echo > ${BEEROCKS_TMP_PATH}/tmp_cal_config
    IFACE_LIST=$( get_hostap_iface_list )

    echo "*** Enable radio's Tx: " $IFACE_LIST " en=$1 ***" 
    for IFACE in $IFACE_LIST
    do
        ./beerocks_agent -t $IFACE $1
    done
}

config_set_acs_scan_mode()
{
    echo "config_set_acs_scan_mode() start"
    echo > ${BEEROCKS_TMP_PATH}/tmp_cal_config
    IFACE_LIST=$( get_hostap_iface_list )
    for IFACE in $IFACE_LIST
    do
        INDEX=$( expr `echo "$IFACE" | tr -dc '0-9'` / 2 + 1 )
        echo "object:Device.WiFi.Radio.$INDEX.X_LANTIQ_COM_Vendor: :MODIFY" >> ${BEEROCKS_TMP_PATH}/tmp_cal_config
        echo "param:WaveAcsScanMode: :true" >> ${BEEROCKS_TMP_PATH}/tmp_cal_config
    done

    caltest -s ${BEEROCKS_TMP_PATH}/tmp_cal_config -c SERVD

    CONF_LINE=`grep -e "&& acs_scan_mode=" $BEEROCKS_WAVE_PATH/scripts/fapi_wlan_wave_radio_set`
    CONF_LINE_COMMENT=`grep -e "&& acs_scan_mode=" $BEEROCKS_WAVE_PATH/scripts/fapi_wlan_wave_radio_set | grep -e "^#"`
    if [ -z "$CONF_LINE_COMMENT" ]; then
        echo "Patch --> $BEEROCKS_WAVE_PATH/scripts/fapi_wlan_wave_radio_set"
        sed -i '/&& acs_scan_mode=/c\[ "$channel" = "acs_smart" ] && acs_scan_mode=1' $BEEROCKS_WAVE_PATH/scripts/fapi_wlan_wave_radio_set
    fi

    echo "config_set_acs_scan_mode() end"
}

enable_wlan_ifaces()
{
    ENABLE=$1 

    if [ $ENABLE == "0" ] || [ $BEEROCKS_LOCAL_GATEWAY == "0" ]; then
        IFACE_LIST=$( get_wpa_supplicant_iface_list )
        echo "*** Enable STA: " $IFACE_LIST  " en=$ENABLE ***"
        {
            for IFACE in $IFACE_LIST ; do
                ./beerocks_agent -w $IFACE $1
            done

        }
    fi

    IFACE_LIST=$( get_hostap_iface_list )
    echo "*** Enable AP's: " $IFACE_LIST " en=$ENABLE ***" 
    {
        for IFACE in $IFACE_LIST ; do
            ./beerocks_agent -w $IFACE $1
        done
    }
}

config_enable_beerocks()
{
    FLAG="false"
    if [ "$1" == "1" ]; then
        FLAG="true"
    fi

    echo "set beerocks_enable to ${FLAG}"

    echo ${UGW_DB_BEEROCKS}": : MODIFY" > ${BEEROCKS_TMP_PATH}/tmp_cal_config
    echo "param:Enable: :"${FLAG} >> ${BEEROCKS_TMP_PATH}/tmp_cal_config
    caltest -s ${BEEROCKS_TMP_PATH}/tmp_cal_config -c SERVD

}

config_onboarding_flag()
{
    FLAG="false"
    if [ "$1" == "1" ]; then
        FLAG="true"
    fi

    echo "set onboarding ${FLAG}"

    echo ${UGW_DB_BEEROCKS}": : MODIFY" > ${BEEROCKS_TMP_PATH}/tmp_cal_config
    echo "param:Onboarding: :"${FLAG} >> ${BEEROCKS_TMP_PATH}/tmp_cal_config
    caltest -s ${BEEROCKS_TMP_PATH}/tmp_cal_config -c WEB
}

config_ssid_pass()
{
    SSID=$1
    SECURITY=$2
    PASS=$3

    # Hack to remove
    # echo "SSID=${SSID}" > ${BEEROCKS_PATH}/cred_hack
    # if [ ${SECURITY} == "None" ]; then
    #     echo "SEC=None" >> ${BEEROCKS_PATH}/cred_hack
    # else
    #     echo "SEC=${SECURITY}" >> ${BEEROCKS_PATH}/cred_hack
    #     echo "PASS=${PASS}" >> ${BEEROCKS_PATH}/cred_hack
    # fi
    
    # TODO: config front and back in more elegant way

    # Front 
    echo ${UGW_DB_BEEROCKS_WIFI}".Front.Ap: : MODIFY" > ${BEEROCKS_TMP_PATH}/tmp_cal_config
    echo "param:SSID: :"${SSID} >> ${BEEROCKS_TMP_PATH}/tmp_cal_config
    if [ ${SECURITY} == "None" ]; then
        echo "param:ModeEnabled: :None" >> ${BEEROCKS_TMP_PATH}/tmp_cal_config
    else
        echo "param:ModeEnabled: :${SECURITY}" >> ${BEEROCKS_TMP_PATH}/tmp_cal_config
        echo "param:KeyPassphrase: :"${PASS} >> ${BEEROCKS_TMP_PATH}/tmp_cal_config
    fi

    if [ $BEEROCKS_LOCAL_GATEWAY != "1" ]; then
        echo ${UGW_DB_BEEROCKS}": : MODIFY" >> ${BEEROCKS_TMP_PATH}/tmp_cal_config
        echo "param:Onboarding: :false" >> ${BEEROCKS_TMP_PATH}/tmp_cal_config
        
    fi

    caltest -s ${BEEROCKS_TMP_PATH}/tmp_cal_config -c SERVD

    # # Back
    # echo ${UGW_DB_BEEROCKS_WIFI}.Back.Ap": : MODIFY" > ${BEEROCKS_TMP_PATH}/tmp_cal_config
    # echo "param:SSID: :"${SSID} >> ${BEEROCKS_TMP_PATH}/tmp_cal_config
    # if [ ${SECURITY} == "None" ]; then
    #     echo "param:ModeEnabled: :None" >> ${BEEROCKS_TMP_PATH}/tmp_cal_config
    # else
    #     echo "param:ModeEnabled: :${SECURITY}" >> ${BEEROCKS_TMP_PATH}/tmp_cal_config
    #     echo "param:KeyPassphrase: :"${PASS} >> ${BEEROCKS_TMP_PATH}/tmp_cal_config
    # fi

    # if [ $BEEROCKS_LOCAL_GATEWAY != "1" ]; then
    #     echo ${UGW_DB_BEEROCKS}": : MODIFY" >> ${BEEROCKS_TMP_PATH}/tmp_cal_config
    #     echo "param:Onboarding: :false" >> ${BEEROCKS_TMP_PATH}/tmp_cal_config
        
    # fi

    # caltest -s ${BEEROCKS_TMP_PATH}/tmp_cal_config -c WEB
}

config_master_mode()
{
    MASTER_MODE="false"
    if [ "$1" == "1" ]; then
        MASTER_MODE="true"
    fi

    echo ${UGW_DB_BEEROCKS}": :MODIFY" > ${BEEROCKS_TMP_PATH}/tmp_cal_config
    echo "param:Master: :"${MASTER_MODE} >> ${BEEROCKS_TMP_PATH}/tmp_cal_config

    caltest -s ${BEEROCKS_TMP_PATH}/tmp_cal_config -c SERVD
}

get_config_master_mode()
{
    RETURN_VALUE=$( ./beerocks_agent -q is_master | grep -e "is_master=true" )
    if [ ! -z "$RETURN_VALUE" ]; then
        BEEROCKS_LOCAL_MASTER="1"
        echo "1" > $BEEROCKS_TMP_PATH/is_master # used by beerocks_watchdog.sh
    else
        BEEROCKS_LOCAL_MASTER="0"
        if [ -f $BEEROCKS_TMP_PATH/is_master ]; then
            rm $BEEROCKS_TMP_PATH/is_master
        fi
    fi

}

config_gateway_mode()
{
    GATEWAY_MODE="false"
    if [ "$1" == "1" ]; then
        GATEWAY_MODE="true"
    fi

    echo UGW_DB_BEEROCKS": :MODIFY" > ${BEEROCKS_TMP_PATH}/tmp_cal_config
    echo "param:Gateway: :"${GATEWAY_MODE} >> ${BEEROCKS_TMP_PATH}/tmp_cal_config

    caltest -s ${BEEROCKS_TMP_PATH}/tmp_cal_config -c SERVD
}

get_config_gateway_mode()
{
    RETURN_VALUE=$( ./beerocks_agent -q is_gateway | grep -e "is_gateway=true" )
    if [ ! -z "$RETURN_VALUE" ]; then
        BEEROCKS_LOCAL_GATEWAY="1"
    else
        BEEROCKS_LOCAL_GATEWAY="0"
    fi

}

config_br_ip()
{
    IP=$1
    BASE_IP=`echo $1 | sed -e 's/\(.*\)./\1 /'`
    # remove trailing space
    BASE_IP="$(echo -e "${BASE_IP}" | sed -e 's/[[:space:]]*$//')"

    clish -c "configure lan" -c "start" \
        -c "set interface br-lan ip ${IP}" \
        -c "set interface br-lan netmask 255.255.255.0" \
        -c "set dhcp server range minaddress ${BASE_IP}100 maxaddress ${BASE_IP}200" \
        -c "commit"
}

set_debug_ip()
{
    IP=$1
    echo "Removing eth0_4 from br-lan"
    brctl delif br-lan eth0_4
    sleep 1
    echo "Set eth0_4 ip to "$IP
    ifconfig $IP netmask 255.255.255.0
}

config_get_enable_dfs()
{
    if [ "$#" -eq 1 ]; then
        IFACE_LIST=$1
    else
        IFACE_LIST=$( get_hostap_iface_list )
    fi


    for IFACE in $IFACE_LIST
    do
        echo "DFS enable value of $IFACE is:"
	    clish -c "show wlan radio $IFACE" | grep IEEE80211hEnabled
    done

}

config_set_enable_dfs()
{
    MODE="false"
    if [ "$1" == "1" ]; then
        MODE="true"
    fi
    
    # old implementation
    # # loop through interface list instead of single interface
    # IFACE_LIST=$( get_hostap_iface_list )
    # for IFACE in $IFACE_LIST
    # do
	#     clish -c "configure wlan" -c "start" \
	# 	      -c "set radio $IFACE IEEE80211hEnabled ${MODE}" \
	# 	      -c "commit"
    # done

    echo > ${BEEROCKS_TMP_PATH}/tmp_cal_config
    IFACE_LIST=$( get_hostap_iface_list )
    for IFACE in $IFACE_LIST
    do
        INDEX=$( expr `echo "$IFACE" | tr -dc '0-9'` / 2 + 1 )
        echo "object:Device.WiFi.Radio.$INDEX: :MODIFY" >> ${BEEROCKS_TMP_PATH}/tmp_cal_config
        echo "param:IEEE80211hEnabled: :$MODE" >> ${BEEROCKS_TMP_PATH}/tmp_cal_config
    done

    caltest -s ${BEEROCKS_TMP_PATH}/tmp_cal_config
}

config_get_country_code()
{
    if [ "$#" -eq 1 ]; then
        IFACE_LIST=$1
    else
        IFACE_LIST=$( get_hostap_iface_list )
    fi


    for IFACE in $IFACE_LIST
    do
        echo "RegulatoryDomain of $IFACE is:"
	    clish -c "show wlan radio $IFACE" | grep RegulatoryDomain
    done

}

config_set_country_code()
{
    LOCATION=" "
    if [ "$2" == "outside" ]; then
        LOCATION="O"
    elif [ "$2" == "inside" ]; then
        LOCATION="I"
    fi

    if [ "$3" == "all" ]; then
        IFACE_LIST=$( get_hostap_iface_list )
    else
        IFACE_LIST=$3
    fi

    COUNTRY_CODE=$1

    VALUE=$COUNTRY_CODE$LOCATION

    for IFACE in $IFACE_LIST
    do
	    clish -c "configure wlan" -c "start" \
		      -c "set radio $IFACE RegulatoryDomain $VALUE" \
		      -c "commit"
    done
}

config_interactive()
{
    # Read the SSID & Pass
    deploy_ssid=""
    read -p "SSID [${DEFAULT_SSID}]: " deploy_ssid
    deploy_ssid=${deploy_ssid:-${DEFAULT_SSID}}

    deploy_security=""
    read -p "Security ['${DEAFULT_SECURITY}'(default) or 'None']: " deploy_security
    deploy_security=${deploy_security:-${DEAFULT_SECURITY}}

    if [ "$deploy_security" == "${DEAFULT_SECURITY}" ]; then
        deploy_pass=""
        read -p "Pass [${DEFAULT_PASS}]: " deploy_pass
        deploy_pass=${deploy_pass:-${DEFAULT_PASS}}
    fi

    # read IP
    deploy_ip=""
    read -p "IP [${DEFAULT_IP}]: " deploy_ip
    deploy_ip=${deploy_ip:-${DEFAULT_IP}}

    # config dfs
    deploy_dfs=""
    read -p "DFS enable [${DEFAULT_DFS}]: " deploy_dfs
    deploy_dfs=${deploy_dfs:-${DEFAULT_DFS}}

    # Confirmation
    abort_deploy=""
    while true; do
        if [ "$deploy_security" == "${DEAFULT_SECURITY}" ]; then
            read -p "Configuration:: SSID: '${deploy_ssid}', Security: '${deploy_security}', 'Pass: '${deploy_pass}', IP: '${deploy_ip}', DFS enable: '${deploy_dfs}' - Confirm? " deploy_confirm
        else
            read -p "Configuration:: SSID: '${deploy_ssid}', Security: '${deploy_security}', IP: '${deploy_ip}', DFS enable: '${deploy_dfs}' - Confirm? " deploy_confirm
        fi

        case ${deploy_confirm} in
        [yY]* ) abort_deploy="Yes"; break;;
        [nN]* ) echo "Aborting..."; break;;
        * ) echo "Valid inputs are 'y' or 'n'.";;
        esac
    done

    if [ "$abort_deploy" == "Yes" ]; then
        config_set_enable_dfs $deploy_dfs
        config_ssid_pass $deploy_ssid $deploy_security $deploy_pass
        config_br_ip $deploy_ip
        config_enable_beerocks "1"

        rm -rf ${BEEROCKS_TMP_PATH}
        ${BEEROCKS_PATH}/beerocks_utils.sh start_wd
    fi
}

beerocks_enable_ahb()
{
    clish -c "configure wlan" -c "start" \
        -c "set radio wlan0 WaveDisableAHB false" \
        -c "commit"
}

show_beerocks_status()
{
    ps | grep beerocks | grep -v grep | grep -v status | grep -v tail
    ps | grep fapi | grep -v grep | grep -v tail
    ps | grep hostapd | grep -v grep | grep -v tail
    ps | grep wpa_supplicant | grep -v grep | grep -v tail
}

update_beerocks()
{
    if [ -e $MULTIAP_DEPLOY_ARCHIVE ]; then
        echo "Updating beerocks"
        echo "Backing up beerocks_controller.conf and beerocks_agent.conf"
        cp $BEEROCKS_PATH/beerocks_*.conf $BEEROCKS_TMP_PATH &> /dev/null
        echo "Unpacking $MULTIAP_DEPLOY_ARCHIVE ..."
        tar -C / -xzvf $MULTIAP_DEPLOY_ARCHIVE
        if [ ! "$#" -eq "1" ]; then
            echo "Using backed-up beerocks_controller.conf and beerocks_agent.conf!"
            cp $BEEROCKS_TMP_PATH/beerocks_*.conf $BEEROCKS_PATH/ &> /dev/null
        fi
        rm -f $MULTIAP_DEPLOY_ARCHIVE
        echo "Done!"
    fi
}

start_beerocks()
{
    #only one instance of the following script can run simultaneously
    #this is ensured inside the script
    if [ -f $BEEROCKS_PATH/.beerocks_endurance_test_on ]; then
        /opt/beerocks/beerocks_endurance_test.sh&
    fi

    rm -f $BEEROCKS_LOGS_PATH/*.log
    rm -f $BEEROCKS_LOGS_PATH/*roll_in_progress*

    # if [ -f "/usr/lib/fapi_wlan_daemon" ]; then
    #     echo "*** start fapi daemon ***"
    #     $BEEROCKS_WAVE_PATH/scripts/fapi_wlan_wave_daemon_start &
    # fi

    if [ $BEEROCKS_LOCAL_MASTER == "1" ]; then
        echo "Start beerocks_controller"
        ./beerocks_controller&
    fi    

    echo "Start beerocks_agent"
    ./beerocks_agent&

    if [ -f $BEEROCKS_PATH/webagent-cli ]; then
        if [ $BEEROCKS_LOCAL_MASTER == "1" ] || [ ! -f "$BEEROCKS_TMP_PATH/agent_onboarded" ]; then
            sleep 1
            echo "Start webagent-cli"
            /opt/beerocks/webagent-cli --daemon &
            printf "$!" > $BEEROCKS_TMP_PATH/pid/webagent-cli
        fi
    fi    
}

pack_log_files()
{
    LOGS_DIR_NAME=system_logs
    if [ "${BEEROCKS_TMP_PATH}/${LOGS_DIR_NAME}.1" -nt "${BEEROCKS_TMP_PATH}/${LOGS_DIR_NAME}.0" ] || [ ! -d "${BEEROCKS_TMP_PATH}/${LOGS_DIR_NAME}.0" ]; then
        echo "saving logs to ${BEEROCKS_TMP_PATH}/${LOGS_DIR_NAME}.0"
        pad -x -r 1 -f ${BEEROCKS_TMP_PATH}/${LOGS_DIR_NAME}.0 -u -s beerocks -H "watchdog restart"
        rm -f  ${BEEROCKS_TMP_PATH}/${LOGS_DIR_NAME}
        ln -s ${BEEROCKS_TMP_PATH}/${LOGS_DIR_NAME}.0 ${BEEROCKS_TMP_PATH}/${LOGS_DIR_NAME}
    else
        echo "saving logs to ${BEEROCKS_TMP_PATH}/${LOGS_DIR_NAME}.1"
        pad -x -r 1 -f ${BEEROCKS_TMP_PATH}/${LOGS_DIR_NAME}.1 -u -s beerocks -H "watchdog restart"
        rm -f  ${BEEROCKS_TMP_PATH}/${LOGS_DIR_NAME}
        ln -s ${BEEROCKS_TMP_PATH}/${LOGS_DIR_NAME}.1 ${BEEROCKS_TMP_PATH}/${LOGS_DIR_NAME}
    fi

    if [ ! -z "$1" ]; then
        echo "adding tag ${1}"
        touch ${BEEROCKS_TMP_PATH}/${LOGS_DIR_NAME}/${1}
    fi
}

killall_beerocks()
{
    if [ "$#" -eq 1 ]; then
        USE_SIG_KILL=$SIG_KILL
    else
        USE_SIG_KILL=$SIG_TERM
    fi
    killall_program beerocks_agent $USE_SIG_KILL
    killall_program beerocks_cli $USE_SIG_KILL
    killall_program beerocks_controller $USE_SIG_KILL
    killall_program beerocks_monitor $USE_SIG_KILL
    killall_program webagent-cli $USE_SIG_KILL   
}

wait_for_killall_beerocks()
{
    SEC_TIMER=0
    while [ $SEC_TIMER -lt 45 ] 
    do
        STATUS=`ps -ef | awk '{print $5}' | grep -e "^\.\/beerocks"`
        if [ -z "$STATUS" ]; then
            echo "killall done"
            return
        fi
        echo $SEC_TIMER" killall -> "$STATUS
        sleep 2
        SEC_TIMER=`expr $SEC_TIMER + 1`
    done
    echo -e "\nkillall timeout!"
}

kill_fapi_daemon()
{
    if [ -f "/usr/lib/fapi_wlan_daemon" ]; then
        echo "*** kill fapi daemon ***"
        killall fapi_wlan_wave_daemon_start
        killall fapi_wlan_daemon
        rm /var/run/fapi_command_socket
        rm /var/run/fapi_command_ended_socket
        rm /var/run/fapi_event_socket
    fi
}

start_fapi_daemon()
{
    if [ -f "/usr/lib/fapi_wlan_daemon" ]; then
        echo "*** start fapi daemon ***"
        $BEEROCKS_WAVE_PATH/scripts/fapi_wlan_wave_daemon_start &
    fi
}

reset_fapi()
{
    if [ $BEEROCKS_INIT == "0" ]; then
        echo "*** FAPI RESET - start***"
        kill_fapi_daemon
        start_fapi_daemon
        echo "*** FAPI RESET - end***"
    fi
}

reset_framework()
{
    if [ $BEEROCKS_INIT == "0" ]; then
        echo "*** MULTIAP FRAMEWORK RESET - start***"
        /etc/init.d/multiap_init.sh restart
        echo "*** MULTIAP FRAMEWORK RESET - end***"
    fi
}

update_ire()
{
    DEFAULT_GW=`ip route | grep default | awk '$3{print $3}'`
    echo "updating ire from $DEFAULT_GW"
    scp admin@$DEFAULT_GW:/opt/beerocks/beerocks_agent ./beerocks_agent_
    scp admin@$DEFAULT_GW:/opt/beerocks/beerocks_monitor ./beerocks_monitor_
    touch /opt/beerocks/restart_beerocks
}

roll_logs()
{
    ROLL_PROGRESS_DIR="roll_in_progress.lock"
    BACKUP_NAME=beerocks_logs_`date "+%Y%m%d_%H%M%S"`.tar.gz

    # Switch into the beerocks logs folder
    cd $BEEROCKS_LOGS_PATH

    # Check if log roll already in progress
    if ! mkdir "$ROLL_PROGRESS_DIR"
    then
        echo "Roll already in progress... Exiting..."
        exit 0
    fi

    logFiles=$(ls *.*.log)
    logFilesCountPreRoll=$(ls *.*.log | wc -l)
        
    # Send USR1 signals to the beerocks processes to trigger log rolling
    killall_program beerocks_controller  -USR1
    killall_program beerocks_agent   -USR1
    killall_program beerocks_monitor -USR1

    # Wait for all the modules to create a new file
    rollTimeout=0
    while true; do
        logFilesCountPostRoll=$(ls *.*.log | wc -l)

        # All modules rolled
        if [ $logFilesCountPostRoll -ge $(($logFilesCountPreRoll * 2)) ]; then
            break
        fi

        # Timeout after 5 seconds
        if [ $rollTimeout -ge 5 ]; then
            break
        fi
        
        rollTimeout=$(($rollTimeout + 1))
        sleep 1
    done

    # Pack and remove the log files
    tar czf $BACKUP_NAME $logFiles
    rm $logFiles

    # Keep last 3 archives
    rm `ls -t *.tar.gz | awk 'NR>3'` > /dev/null 2>&1

    # Cleanup
    rm -r $ROLL_PROGRESS_DIR
}

add_bridge_ip()
{
    # Create a script to make sure the ip will stay after dhcp
    echo "#!/bin/sh" > $BEEROCKS_PATH/beerocks_additional_bridge_ip.sh
    echo "while true; do \
          ip addr add $1/24 dev br-lan &>/dev/null; \
          ping 192.168.2.2 -c 1 &>/dev/null; \
          sleep 5; \
          done" >> "$BEEROCKS_PATH/beerocks_additional_bridge_ip.sh"

    chmod +x $BEEROCKS_PATH/beerocks_additional_bridge_ip.sh

    # Apply now
    ip addr add $1/24 dev br-lan &>/dev/null
    ping 192.168.2.2 -c 1 &>/dev/null
}

ftest()
{
    : # Write here test code
}

print_banner()
{

  echo "
   ______    _______  _______  ______     ______     ______  __  ___     _______     __       _  _     
  |   _  \  |   ____||   ____||   _  \   /  __  \   /      ||  |/  /    /       |   /_ |     | || |   
  |  |_)  | |  |__   |  |__   |  |_)  | |  |  |  | |  ,----'|  '  /    |   (----'    | |     | || |_  
  |   _  <  |   __|  |   __|  |      /  |  |  |  | |  |     |    <      \   \        | |     |__   _|  
  |  |_)  | |  |____ |  |____ |  |\  \-.|  '--'  | |  '----.|  .  \ .----)   |       | |  __    | |    
  |______/  |_______||_______|| _| \._ | \______/   \______||__|\__\|_______/        |_| (__)   |_|   
                                                                                                         "                                                                                                     
}

############################################################################
# Main
############################################################################
cd $BEEROCKS_PATH

BEEROCKS_INIT="0"
BEEROCKS_START="0"
BEEROCKS_LOCAL_MASTER="0"
BEEROCKS_LOCAL_GATEWAY="0"

if [ ! -d $BEEROCKS_TMP_PATH/pid ]; then
    BEEROCKS_INIT="1"
    mkdir -p $BEEROCKS_TMP_PATH
fi

# updates master mode ($BEEROCKS_LOCAL_MASTER)
get_config_master_mode

# updates gateway mode ($BEEROCKS_LOCAL_GATEWAY)
get_config_gateway_mode

# One time setup on boot
if [ "$1" == "start" ] || [ "$1" == "start_wd" ] || [ "$1" == "start_wd_forced" ]; then

    # Banner
    print_banner

    # HACK to support RDKB
    if [ "$2" == "tmp" ]; then
        BEEROCKS_PATH=/tmp/multiap/beerocks/
        BEEROCKS_LIB_PATH=/tmp/multiap/framework/lib
    fi

    update_beerocks

    BEEROCKS_START="1"

    if [ $BEEROCKS_INIT == "1" ]; then
        echo "******* beerocks_utils.sh init started *******"
        mkdir -p $BEEROCKS_TMP_PATH/pid
        mkdir -p $BEEROCKS_LOGS_PATH
      
        chmod +x /opt/beerocks/udhcpc.script.alt


        # echo "******* HACK!!! --> One time WLAN config_set_acs_scan_mode *******"
        # kill_fapi_daemon
        # enable_wlan_ifaces 0
        # enable_wlan_radios 0
        # config_set_acs_scan_mode
        # enable_wlan_radios 1
        # start_fapi_daemon
        # enable_wlan_ifaces 1

        echo "******* beerocks_utils.sh init done *******"
    fi
    
    if [ -f $BEEROCKS_PATH/beerocks_additional_bridge_ip.sh ]; then
        PID=$( ps -ef | grep beerocks_additional_bridge_ip | grep -v grep | awk '{print $1}' )    
        if [ -z $PID ]
        then
            echo "Starting beerocks_additional_bridge_ip.sh"
            $BEEROCKS_PATH/beerocks_additional_bridge_ip.sh&
        fi
    fi

fi

# Check command line args
if [ "$1" == "help" ]; then
    echo "Usage: beerocks_utils.sh <command> [options]"
    echo "  available commands: "
    echo "    start                                                 : start/restart beerocks and hostapd, watchdog disabled"
    echo "    start_wd                                              : start/restart beerocks and hostapd, watchdog enabled"
    echo "    start_wd_forced                                       : start/restart beerocks and hostapd, watchdog enabled (force kill existing instances)"
    echo "    stop                                                  : stop beerocks"
    echo "    killall                                               : kill all: beerocks and watchdog, reset hostapd"
    echo "    reset_hostapd [iface]                                 : restart all hostapd's, or [iface] if given"
    echo "    kill_hostapd [iface]                                  : kill all hostapd's, or [iface] if given"
    echo "    reset_wpa_supplicant [iface]                          : restart all hostapd's, or [iface] if given"
    echo "    kill_wpa_supplicant [iface] [signal]                  : kill wpa_supplicant, or [iface] if given, using [signal]"
    echo "    reset_framework                                       : restart multiap framework"
    echo "    reset_fapi                                            : restart fapi"
    echo "    set_enable_beerocks <0/1>                             : 0 - Disable , 1 - Enable"
    echo "    set_master_mode <0/1>                                 : 0 - Disable , 1 - Enable"
    echo "    set_gateway_mode <0/1>                                : 0 - Disable , 1 - Enable"
    echo "    set_onboarding <0/1>                                  : set the onboarding flag to the given value"
    echo "    set_ssid_key <ssid> [key]                             : Update platform ssid & key for all hostap wlan iface, if key is not given, the networks is unsecure"
    echo "    set_br_ip <ip address>                                : For GW, set the IP address of br-lan iface + DHCP range (100-200)"
    echo "    set_debug_ip <ip address>                             : remove eth0_4 from br-lan and set IP for debug"
    echo "    set_enable_dfs <0/1>                                  : 0 - Disable , 1 - Enable"
    echo "    get_enable_dfs [iface]                                : get dfs_enable value on all radios, or [iface] if given"
    echo "    set_country_code <state_county_code> <env> [iface]    : set county code (2 capital letters) with enviorment ('all'/'outside'/inside)"
    echo "                                                            all radios, or [iface] if given"
    echo "    get_country_code [iface]                              : get county code on all radios, or [iface] if given"
    echo "    enable_wlan_ifaces <0/1>                              : enable / disable all configured AP/STA interfaces"
    echo "    enable_wlan_radios <0/1>                              : enable / disable all configured radios Tx"
    echo "    enable_ahb                                            : enable AHB for wlan0"
    echo "    update                                                : update beerocks binaries"
    echo "    config                                                : interactive configure device"
    echo "    get_config                                            : prints beerocks database configuration"
    echo "    status                                                : show beerocks status"
    echo "    update_ire                                            : update ire agent and monitor executables"
    echo "    pack_logs                                             : Pack beerocks logs to an archive"
    echo "    roll_logs                                             : Rool beerocks logs and pack to an archive"
    echo "    clear_4addr_list                                      : clear all entries in FourAddr STA list on all ifaces, or [iface] if given"
    echo "    config_set_acs_scan_mode                              : set hostapd acs_scan_mode to 1"
    echo "    add_bridge_ip <bridge_ip> <eth_remote_pc_ip>          : set secondary bridge ip to allow connectivity with lab PC"

elif [ "$1" == "stop" ]; then
    killall_beerocks
    wait_for_killall_beerocks
    killall_beerocks 1

elif [ "$1" == "killall" ]; then
    killall_program beerocks_watchdog.sh
    killall_beerocks
    wait_for_killall_beerocks
    killall_beerocks 1

elif [ "$1" == "start" ]; then
    echo "Starting without watchdog..."
    killall_program beerocks_watchdog.sh
    killall_beerocks
    wait_for_killall_beerocks
    killall_beerocks 1
    rm -f $BEEROCKS_ERRORS_FILE # Delete the beerocks errors file
    reset_fapi
    reset_framework
    start_beerocks

elif [ "$1" == "start_wd" ]; then
    killall_program beerocks_watchdog.sh
    killall_beerocks
    wait_for_killall_beerocks
    killall_beerocks 1
    reset_fapi
    reset_framework
    start_beerocks
    start_beerocks_watchdog

elif [ "$1" == "start_wd_forced" ]; then
    killall_program beerocks_watchdog.sh -9
    killall_beerocks 1
    reset_fapi
    reset_framework
    start_beerocks
    start_beerocks_watchdog

elif [ "$1" == "killall_forced" ]; then
    killall_program beerocks_watchdog.sh -9
    killall_beerocks 1

elif [ "$1" == "reset_hostapd" ]; then
    if [ "$#" -eq 1 ]; then
        reset_hostapd
    else
        reset_hostapd $2
    fi

elif [ "$1" == "kill_hostapd" ]; then
    if [ "$#" -eq 1 ]; then
        kill_hostapd
    else
        kill_hostapd $2
    fi

elif [ "$1" == "reset_wpa_supplicant" ]; then
    if [ "$#" -eq 1 ]; then
        reset_wpa_supplicant
    else
        reset_wpa_supplicant $2
    fi

elif [ "$1" == "kill_wpa_supplicant" ]; then
    if [ "$#" -eq 1 ]; then
        kill_wpa_supplicant
    elif [ "$#" -eq 2 ]; then
        kill_wpa_supplicant $2
    else
        kill_wpa_supplicant $2 $3
    fi

elif [ "$1" == "reset_fapi" ]; then
    reset_fapi

elif [ "$1" == "reset_framework" ]; then
    reset_framework

elif [ "$1" == "set_enable_beerocks" ]; then
    config_enable_beerocks $2

elif [ "$1" == "set_onboarding" ]; then
    if [ "$#" -eq 2 ]; then
        rm ${BEEROCKS_TMP_PATH}/tmp_cal_config
        config_onboarding_flag $2
    else
        echo "Error, value not given."
    fi

elif [ "$1" == "set_ssid_key" ]; then
    if [ "$#" -eq 3 ]; then
        config_ssid_pass "$2" ${DEAFULT_SECURITY} "$3"
    elif [ "$#" -eq 2 ]; then
        config_ssid_pass "$2" "None"
    else
        echo "Error, ssid / key not given."
    fi

elif [ "$1" == "set_master_mode" ]; then
    if [ "$#" -eq 2 ]; then
        config_master_mode $2
    else
        echo "Error, value not given."
    fi

elif [ "$1" == "set_gateway_mode" ]; then
    if [ "$#" -eq 2 ]; then
        config_gateway_mode $2
    else
        echo "Error, value not given."
    fi

elif [ "$1" == "set_br_ip" ]; then
    if [ "$#" -eq 2 ]; then
        config_br_ip $2
    else
        echo "Error, value not given."
    fi

elif [ "$1" == "set_debug_ip" ]; then
    if [ "$#" -eq 2 ]; then
        set_debug_ip $2
    else
        echo "Error, value not given."
    fi

elif [ "$1" == "enable_ahb" ]; then
    beerocks_enable_ahb

elif [ "$1" == "set_enable_dfs" ]; then
    if [ "$#" -eq 2 ]; then
        config_set_enable_dfs $2
    else
        echo "Error, value not given."
    fi

elif [ "$1" == "get_enable_dfs" ]; then
    if [ "$#" -eq 1 ]; then
        config_get_enable_dfs
    else
        config_get_enable_dfs $2
    fi

elif [ "$1" == "set_country_code" ]; then
    if [ "$#" -eq 3 ]; then
        config_set_country_code $2 $3 "all"
    elif [ "$#" -eq 4 ]; then
        config_set_country_code $2 $3 $4
    else
        echo "Error, missing input args"
    fi 

elif [ "$1" == "get_country_code" ]; then
    if [ "$#" -eq 1 ]; then
        config_get_country_code
    else
        config_get_country_code $2
    fi

elif [ "$1" == "config" ]; then
    echo "Device configuration:"

    config_interactive

elif [ "$1" == "get_config" ]; then
    get_beerocks_config

elif [ "$1" == "status" ]; then
    echo "Beerocks status:"
    show_beerocks_status

elif [ "$1" == "update_ire" ]; then
    update_ire

elif [ "$1" == "pack_logs" ]; then
    pack_log_files $2

elif [ "$1" == "roll_logs" ]; then
    roll_logs $2

elif [ "$1" == "clear_4addr_list" ]; then
    if [ "$#" -eq 1 ]; then
        clear_4addr_list
    else
        clear_4addr_list $2
    fi

elif [ "$1" == "ftest" ]; then
    ftest

elif [ "$1" == "update" ]; then
    if [ "$#" -eq 1 ]; then
        update_beerocks
    else
        update_beerocks $2
    fi

elif [ "$1" == "banner" ]; then
    print_banner

elif [ "$1" == "kill_fapi" ]; then
    kill_fapi_daemon

elif [ "$1" == "start_fapi" ]; then
    start_fapi_daemon

elif [ "$1" == "enable_wlan_ifaces" ]; then
    if [ "$#" -eq 2 ]; then
        enable_wlan_ifaces $2
    else
        echo "Error, value not given."
    fi

elif [ "$1" == "enable_wlan_radios" ]; then
    if [ "$#" -eq 2 ]; then
        enable_wlan_radios $2
    else
        echo "Error, value not given."
    fi

elif [ "$1" == "config_set_acs_scan_mode" ]; then
    config_set_acs_scan_mode
    
elif [ "$1" == "add_bridge_ip" ]; then
    if [ "$#" -eq 3 ]; then
        add_bridge_ip $2 $3
    else
        echo "Error, value not given."
    fi

else
    echo "command unknown!, run with 'help' for more info"
fi

# delete beerocks tmp dir if its init run without starting beerocks
if [ $BEEROCKS_START == "0" ] && [ $BEEROCKS_INIT == "1" ]; then
    rm -rf $BEEROCKS_TMP_PATH
fi
