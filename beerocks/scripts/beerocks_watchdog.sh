#!/bin/sh
#############################################################################
# INTEL CONFIDENTIAL
# Copyright 2016 Intel Corporation All Rights Reserved.
#
# The source code contained or described herein and all documents related to
# the source code ("Material") are owned by Intel Corporation or its
# suppliers or licensors.  Title to the Material remains with Intel
# Corporation or its suppliers and licensors.  The Material contains trade
# secrets and proprietary and confidential information of Intel or its
# suppliers and licensors.  The Material is protected by worldwide copyright
# and trade secret laws and treaty provisions. No part of the Material may
# be used, copied, reproduced, modified, published, uploaded, posted,
# transmitted, distributed, or disclosed in any way without Intel's prior
# express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or
# delivery of the Materials,  either expressly, by implication, inducement,
# estoppel or otherwise.  Any license under such intellectual property
# rights must be express and approved by Intel in writing.
#############################################################################

if [ -f "/opt/multiap/framework/bpl_err.sh" ] # if UGW
then
. /opt/multiap/framework/bpl_err.sh
fi

BEEROCKS_PATH=/opt/beerocks


BEEROCKS_TMP_PATH=/tmp/beerocks

BEEROCKS_ERROR_HANDLER=$BEEROCKS_PATH/beerocks_error_handler.sh

killall_program(){
    PROGRAM_NAME=$1
    if [ "$#" -eq 2 ]; then
        KILL_SIG=$2
    else
        KILL_SIG=$SIG_TERM
    fi
    for PID in $(ps -ef | grep $PROGRAM_NAME | grep -v grep | awk '{print $1}'); do
        echo "kill $KILL_SIG $PID $PROGRAM_NAME";
        kill $KILL_SIG $PID > /dev/null 2>&1;
    done
}

is_program_running(){
    PROGRAM_NAME=$1
    for PID in $(ps -ef | grep $PROGRAM_NAME | grep -v grep | awk '{print $1}'); do
        echo "true"
        return
    done
    echo "false"
}

PID_STUCK_RETRIES=0
PID_STUCK_RETRIES_LIMIT=15
WATCHDOG_CYCLE_SECONDS=10

# save PID of script to file
echo $$ > $BEEROCKS_TMP_PATH/pid/beerocks_watchdog

while true :
do
    sleep $WATCHDOG_CYCLE_SECONDS
    #check beerocks
    for FILE in beerocks_controller beerocks_agent; do
        BEEROCKS_FILE=$BEEROCKS_PATH/$FILE
        PID_FILE=$BEEROCKS_TMP_PATH/pid/$FILE
        if [ -f $BEEROCKS_FILE ] && [ -f $PID_FILE ]; then
            PID=`cat $PID_FILE`
            PID_ZOMBIE=`ps | grep $PID | awk '$4=="Z"{print "Z"}'`
            if [ ! -f /proc/$PID/exe ] || [ ! -z $PID_ZOMBIE ] ; then
                echo "$0: CRITICAL: $FILE PID $PID error, trigger error handler"
                $BEEROCKS_ERROR_HANDLER 0 $BPL_ERR_WATCHDOG_PROCESS_ZOMBIE
            fi
        fi
    done

    PID_SUM_COUNTER=0
    PID_RUNNING_COUNTER=0
    PID_STUCK_RETRIES_LIMIT=4
    # Check for stuck processes
    NOW_EPOCH=$(date +%s)
    PID_FILES=$(ls $BEEROCKS_TMP_PATH/pid/ | grep -e master -e slave )
    PID_STUCK=false
    
    for PID_FILE in $PID_FILES; do
        FILE_EPOCH_TIMEOUT=$(( $(date -r $BEEROCKS_TMP_PATH/pid/$PID_FILE +%s) + $WATCHDOG_CYCLE_SECONDS ))
        PID=`cat $BEEROCKS_TMP_PATH/pid/$PID_FILE`
        if [ -f /proc/$PID/exe ] && [ $NOW_EPOCH -gt $FILE_EPOCH_TIMEOUT ]; then
            echo "$0: WARNING [`date`]: Process $PID_FILE detected system slow down!"
            echo "$0: WARNING [`date`]: Process $PID_FILE detected system slow down!" >> $BEEROCKS_TMP_PATH/logs/slow_down.log
            top -b -n1 >> $BEEROCKS_TMP_PATH/logs/slow_down.log
            echo " "
            df >> $BEEROCKS_TMP_PATH/logs/slow_down.log
            echo " "
            PID_STUCK=true
        fi
    done

     [ "$PID_STUCK" == "true" ] && {
        let PID_STUCK_RETRIES++
        echo "$0: WARNING [`date`]: Beerocks processes stuck retry count=$PID_STUCK_RETRIES"
        echo "$0: WARNING [`date`]: Beerocks processes stuck retry count=$PID_STUCK_RETRIES"  >> $BEEROCKS_TMP_PATH/logs/slow_down.log
        top -b -n1 >> $BEEROCKS_TMP_PATH/logs/slow_down.log                                                               
        echo " "                                                                                                     
        df >> $BEEROCKS_TMP_PATH/logs/slow_down.log
        echo " "  
    } || PID_STUCK_RETRIES=0

    if [ $PID_STUCK_RETRIES -gt $PID_STUCK_RETRIES_LIMIT ]; then
        echo "$0: CRITICAL: Beerocks processes are stuck (max retries expired), trigger error handler"
        PID_STUCK_RETRIES=0
        $BEEROCKS_ERROR_HANDLER 0 $BPL_ERR_WATCHDOG_PROCESS_STUCK
    fi   
    
done
