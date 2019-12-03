/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bpl/bpl_cfg.h>
#include <bpl/bpl_wlan.h>
#include <dlfcn.h>
#include <mapf/common/logger.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace mapf;
MAPF_INITIALIZE_LOGGER

int wlanReady = -1;

#define SKIP_IF_WLAN_NOT_READY                                                                     \
    {                                                                                              \
        if (wlanReady != 0) {                                                                      \
            wlanReady = bpl_wlan_ready();                                                          \
            if (wlanReady != 0) {                                                                  \
                MAPF_ERR("Can't perform requested operation. WLAN is not initialized. Please fix " \
                         "to continue.\n");                                                        \
                break;                                                                             \
            }                                                                                      \
        }                                                                                          \
    }

void getString(char *s, int size)
{
    if (!fgets(s, size, stdin)) {
        fprintf(stderr, "No input.\n");
        exit(1);
    }
}

int main()
{
    int return_status;
    char inputInterface[32], userInputS[10];
    struct BPL_WLAN_PARAMS wlan_params;

    while (1) {
        memset(inputInterface, 0, sizeof(inputInterface));
        printf("\n1. Query WLAN ready\n2. AP start\n3. AP stop\n4. STA start\n5. STA stop\n6. "
               "Restore at interface level\n7. Restore at full scope\n8. Set WiFI credentials\n9. "
               "Get WiFi credentials\n10. AP post init handling\n11. Exit\n");
        fflush(stdin);
        getString(userInputS, 3);
        switch (atoi(userInputS)) {
        case 1:
            wlanReady = bpl_wlan_ready();
            if (wlanReady != 0) {
                MAPF_ERR("WLAN is not initialized.\n");
                wlanReady = -1;
            } else {
                MAPF_INFO("WLAN has been initialized.\n");
            }
            break;
        case 2:
            SKIP_IF_WLAN_NOT_READY
            printf("Interface : ");
            fflush(stdin);
            getString(inputInterface, sizeof(inputInterface));
            MAPF_DBG("Performing AP start on " << inputInterface << "\n");
            return_status = bpl_wlan_ap_start(inputInterface);
            if (return_status == 1) {
                MAPF_INFO("AP interface " << inputInterface << " is already up.\n");
            } else if (return_status == 0) {
                MAPF_INFO("AP start on interface " << inputInterface << " succeeded.\n");
            } else {
                MAPF_ERR("AP start on interface " << inputInterface << " failed.\n");
            }
            break;
        case 3:
            SKIP_IF_WLAN_NOT_READY
            printf("Interface : ");
            fflush(stdin);
            getString(inputInterface, sizeof(inputInterface));
            MAPF_DBG("Performing AP stop on " << inputInterface << "\n");
            return_status = bpl_wlan_ap_stop(inputInterface);
            if (return_status == 1) {
                MAPF_INFO("AP interface " << inputInterface << " is already down.\n");
            } else if (return_status == 0) {
                MAPF_INFO("AP stop on interface " << inputInterface << " succeeded.\n");
            } else {
                MAPF_ERR("AP stop on interface " << inputInterface << " failed.\n");
            }
            break;
        case 4:
            SKIP_IF_WLAN_NOT_READY
            printf("Interface : ");
            fflush(stdin);
            getString(inputInterface, sizeof(inputInterface));
            MAPF_DBG("Performing STA start on " << inputInterface << "\n");
            if (bpl_wlan_sta_start(inputInterface) != 0) {
                MAPF_ERR("STA start on interface " << inputInterface << " failed.\n");
            } else {
                MAPF_INFO("STA start on interface " << inputInterface << " succeeded.\n");
            }
            break;
        case 5:
            SKIP_IF_WLAN_NOT_READY
            printf("Interface : ");
            fflush(stdin);
            getString(inputInterface, sizeof(inputInterface));
            MAPF_DBG("Performing STA stop on " << inputInterface << "\n");
            if (bpl_wlan_sta_stop(inputInterface) != 0) {
                MAPF_ERR("STA stop on interface " << inputInterface << " failed.\n");
            } else {
                MAPF_INFO("STA stop on interface " << inputInterface << " succeeded.\n");
            }
            break;
        case 6:
            SKIP_IF_WLAN_NOT_READY
            printf("Interface : ");
            fflush(stdin);
            getString(inputInterface, sizeof(inputInterface));
            MAPF_DBG("Performing recovery on " << inputInterface << "\n");
            if (bpl_wlan_restore(inputInterface) != 0) {
                MAPF_ERR("Recovery on interface " << inputInterface << " failed.\n");
            } else {
                MAPF_INFO("Recovery on interface " << inputInterface << " succeeded.\n");
            }
            break;
        case 7:
            SKIP_IF_WLAN_NOT_READY
            MAPF_DBG("Performing WLAN recovery\n");
            if (bpl_wlan_restore(NULL) != 0) {
                MAPF_ERR("WLAN recovery failed.\n");
            } else {
                MAPF_INFO("WLAN recovery succeeded.\n");
            }
            break;
        case 9:
            memset(&wlan_params, 0, sizeof(wlan_params));
            printf("Interface : ");
            fflush(stdin);
            getString(inputInterface, sizeof(inputInterface));
            if (bpl_cfg_get_wifi_params(inputInterface, &wlan_params) != 0) {
                MAPF_ERR("Failed to retrieve WiFi params for " << inputInterface << "\n");
            } else {
                MAPF_INFO("SSID " << wlan_params.ssid << " Security mode " << wlan_params.security
                                  << " Passphrase " << wlan_params.passphrase << "\n");
            }
            break;
        case 10:
            SKIP_IF_WLAN_NOT_READY
            MAPF_DBG("Performing post init handling for AP wlan2\n");
            if (bpl_wlan_ap_postinit("wlan2") != 0) {
                MAPF_ERR("Post init handling on AP wlan2 failed.\n");
            } else {
                MAPF_INFO("Post init handling on AP wlan2 succeeded.\n");
            }
            break;
        case 11:
            return 0;
            break;
        default:
            printf("Invalid selection\n");
            break;
        }
    }
    return 0;
}
