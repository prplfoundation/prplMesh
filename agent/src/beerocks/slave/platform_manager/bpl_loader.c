/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bpl/bpl.h>
#include <bpl/bpl_arp.h>
#include <bpl/bpl_cfg.h>
#include <bpl/bpl_dhcp.h>

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

/**
 * MMZ:
 * It is not necessary to dynamically load this library during execution since
 * it is available during compile/link phase.
 *
 * Dynamic load is suitable/required for plug-in libraries but I think it adds an
 * unnecessary overhead in this case. Moreover, code readability and
 * maintainability are more difficult.
 */

// The file name of the BPL shared library
#ifndef BPL_MODULE_NAME
#define BPL_MODULE_NAME "libbpl.so"
#endif

// Structore for storing pointers to dynamically loaded symbols
struct SModuleSymbols {
    void *handle;

    // General Symbols
    int (*bpl_init)();
    void (*bpl_close)();

    // BPL Configuration Symbols
    int (*bpl_cfg_is_enabled)();
    int (*bpl_cfg_is_master)();
    int (*bpl_cfg_get_operating_mode)();
    int (*bpl_cfg_is_onboarding)();
    int (*bpl_cfg_get_rdkb_extensions)();
    int (*bpl_cfg_get_band_steering)();
    int (*bpl_cfg_get_dfs_reentry)();
    int (*bpl_cfg_get_client_roaming)();
    int (*bpl_cfg_get_wifi_params)(const char *iface, struct BPL_WLAN_PARAMS *wlan_params);
    int (*bpl_cfg_get_backhaul_params)(int *, int *, int *);
    int (*bpl_cfg_get_backhaul_vaps)(char *, const int);
    int (*bpl_cfg_get_beerocks_credentials)(const int, char *, char *, char *);
    int (*bpl_cfg_get_security_policy)();
    int (*bpl_cfg_set_onboarding)(int);
    int (*bpl_cfg_notify_onboarding_completed)(const char *, const char *, const char *,
                                               const char *, const int);
    int (*bpl_cfg_notify_fw_version_mismatch)();
    int (*bpl_cfg_notify_error)(int, const char *);
    int (*bpl_cfg_notify_iface_status)(const BPL_INTERFACE_STATUS_NOTIFICATION *status_notif);
    int (*bpl_cfg_get_administrator_credentials)(char *);
    int (*bpl_cfg_get_device_info)(struct BPL_DEVICE_INFO *);

    // BPL DHCP Symbols
    int (*bpl_dhcp_mon_start)(bpl_dhcp_mon_cb);
    int (*bpl_dhcp_mon_handle_event)();
    int (*bpl_dhcp_mon_stop)();

    // BPL ARP Symbols
    int (*bpl_arp_mon_start)(BPL_ARP_MON_CTX *, const char *);
    int (*bpl_arp_mon_stop)(BPL_ARP_MON_CTX);
    int (*bpl_arp_mon_get_fd)(BPL_ARP_MON_CTX);
    int (*bpl_arp_mon_get_raw_arp_fd)(BPL_ARP_MON_CTX);
    int (*bpl_arp_mon_process)(BPL_ARP_MON_CTX, struct BPL_ARP_MON_ENTRY *);
    int (*bpl_arp_mon_process_raw_arp)(BPL_ARP_MON_CTX, struct BPL_ARP_MON_ENTRY *);
    int (*bpl_arp_mon_probe)(BPL_ARP_MON_CTX, const uint8_t *, const uint8_t *, int);
    int (*bpl_arp_mon_get_mac_for_ip)(BPL_ARP_MON_CTX, const uint8_t *, uint8_t *);
    int (*bpl_arp_mon_get_ip_for_mac)(BPL_ARP_MON_CTX, const uint8_t *, uint8_t *);
    int (*bpl_arp_get_bridge_iface)(const char *, const uint8_t *, char *);
};

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Variables ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

// Structure for storing dynamically loaded addresses
static struct SModuleSymbols s_sModuleSymbols;

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static void *load_symbol(const char *sym_name)
{
    void *symbol;

    if (!s_sModuleSymbols.handle)
        return NULL;

    if (!(symbol = dlsym(s_sModuleSymbols.handle, sym_name))) {
        char *dl_error = dlerror();
        printf("BPL - Failed loading symbol '%s': %s\n", sym_name,
               dl_error ? dl_error : "No Error");
        if (!dl_error) {
            return NULL;
        }
        return NULL;
    }

    return symbol;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////// Local Module MACROs /////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define BPL_LOAD_SYM(sym_name)                                                                     \
    ({                                                                                             \
        if (s_sModuleSymbols.sym_name == NULL)                                                     \
            s_sModuleSymbols.sym_name = load_symbol(#sym_name);                                    \
        s_sModuleSymbols.sym_name;                                                                 \
    })

#define BPL_CALL_SYM(sym_name, ...) ({ (*(s_sModuleSymbols.sym_name))(__VA_ARGS__); })

#define BPL_EXEC_SYM(sym_name, ...)                                                                \
    ({ ((BPL_LOAD_SYM(sym_name)) ? BPL_CALL_SYM(sym_name, __VA_ARGS__) : -1); })

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int bpl_init()
{
    if (s_sModuleSymbols.handle != NULL) {
        printf("BPL - Module '%s' already loaded!\n", BPL_MODULE_NAME);
        return -1;
    }

    // Clear all the handles
    memset(&s_sModuleSymbols, 0, sizeof(s_sModuleSymbols));

    if ((s_sModuleSymbols.handle = dlopen(BPL_MODULE_NAME, RTLD_LAZY)) == NULL) {
        char *dl_error = dlerror();
        if (!dl_error) {
            printf("BPL - Failed loading module '%s': failed getting error\n", BPL_MODULE_NAME);
            return -1;
        }

        printf("BPL - Failed loading module ('%s'): %s\n", BPL_MODULE_NAME, dl_error);
        return -1;
    }

    return (BPL_EXEC_SYM(bpl_init));
}

void bpl_close()
{
    BPL_EXEC_SYM(bpl_close);

    if (s_sModuleSymbols.handle) {
        dlclose(s_sModuleSymbols.handle);
        s_sModuleSymbols.handle = NULL;
    }

    // Clear all the handles
    memset(&s_sModuleSymbols, 0, sizeof(s_sModuleSymbols));
}

int bpl_cfg_is_enabled() { return (BPL_EXEC_SYM(bpl_cfg_is_enabled)); }

int bpl_cfg_get_operating_mode() { return (BPL_EXEC_SYM(bpl_cfg_get_operating_mode)); }

int bpl_cfg_is_master() { return (BPL_EXEC_SYM(bpl_cfg_is_master)); }

int bpl_cfg_is_onboarding() { return (BPL_EXEC_SYM(bpl_cfg_is_onboarding)); }

int bpl_cfg_get_rdkb_extensions() { return (BPL_EXEC_SYM(bpl_cfg_get_rdkb_extensions)); }

int bpl_cfg_get_band_steering() { return (BPL_EXEC_SYM(bpl_cfg_get_band_steering)); }

int bpl_cfg_get_dfs_reentry() { return (BPL_EXEC_SYM(bpl_cfg_get_dfs_reentry)); }

int bpl_cfg_get_client_roaming() { return (BPL_EXEC_SYM(bpl_cfg_get_client_roaming)); }

int bpl_cfg_get_wifi_params(const char *iface, struct BPL_WLAN_PARAMS *wlan_params)
{
    return (BPL_EXEC_SYM(bpl_cfg_get_wifi_params, iface, wlan_params));
}

int bpl_cfg_get_backhaul_params(int *max_vaps, int *network_enabled, int *prefered_radio_band)
{
    return (
        BPL_EXEC_SYM(bpl_cfg_get_backhaul_params, max_vaps, network_enabled, prefered_radio_band));
}

int bpl_cfg_get_backhaul_vaps(char *backhaul_vaps_buf, const int buf_len)
{
    return (BPL_EXEC_SYM(bpl_cfg_get_backhaul_vaps, backhaul_vaps_buf, buf_len));
}

int bpl_cfg_get_beerocks_credentials(int radio_dir, char *ssid, char *pass, char *sec)
{
    return (BPL_EXEC_SYM(bpl_cfg_get_beerocks_credentials, radio_dir, ssid, pass, sec));
}

int bpl_cfg_get_security_policy() { return (BPL_EXEC_SYM(bpl_cfg_get_security_policy)); }

int bpl_cfg_set_onboarding(int enable) { return (BPL_EXEC_SYM(bpl_cfg_set_onboarding, enable)); }

int bpl_cfg_notify_onboarding_completed(const char *ssid, const char *pass, const char *sec,
                                        const char *iface_name, const int success)
{
    return (
        BPL_EXEC_SYM(bpl_cfg_notify_onboarding_completed, ssid, pass, sec, iface_name, success));
}

int bpl_cfg_notify_fw_version_mismatch()
{
    return (BPL_EXEC_SYM(bpl_cfg_notify_fw_version_mismatch));
}

int bpl_cfg_notify_error(int code, const char *str)
{
    return (BPL_EXEC_SYM(bpl_cfg_notify_error, code, str));
}

int bpl_cfg_notify_iface_status(const BPL_INTERFACE_STATUS_NOTIFICATION *status_notif)
{
    return (BPL_EXEC_SYM(bpl_cfg_notify_iface_status, status_notif));
}

int bpl_cfg_get_administrator_credentials(char *pass)
{
    return (BPL_EXEC_SYM(bpl_cfg_get_administrator_credentials, pass));
}

int bpl_cfg_get_device_info(struct BPL_DEVICE_INFO *device_info)
{
    return (BPL_EXEC_SYM(bpl_cfg_get_device_info, device_info));
}

int bpl_dhcp_mon_start(bpl_dhcp_mon_cb cb) { return (BPL_EXEC_SYM(bpl_dhcp_mon_start, cb)); }

int bpl_dhcp_mon_handle_event() { return (BPL_EXEC_SYM(bpl_dhcp_mon_handle_event)); }

int bpl_dhcp_mon_stop() { return (BPL_EXEC_SYM(bpl_dhcp_mon_stop)); }

int bpl_arp_mon_start(BPL_ARP_MON_CTX *ctx, const char *iface)
{
    return (BPL_EXEC_SYM(bpl_arp_mon_start, ctx, iface));
}

int bpl_arp_mon_stop(BPL_ARP_MON_CTX ctx) { return (BPL_EXEC_SYM(bpl_arp_mon_stop, ctx)); }

int bpl_arp_mon_get_fd(BPL_ARP_MON_CTX ctx) { return (BPL_EXEC_SYM(bpl_arp_mon_get_fd, ctx)); }

int bpl_arp_mon_get_raw_arp_fd(BPL_ARP_MON_CTX ctx)
{
    return (BPL_EXEC_SYM(bpl_arp_mon_get_raw_arp_fd, ctx));
}

int bpl_arp_mon_process(BPL_ARP_MON_CTX ctx, struct BPL_ARP_MON_ENTRY *entry)
{
    return (BPL_EXEC_SYM(bpl_arp_mon_process, ctx, entry));
}

int bpl_arp_mon_process_raw_arp(BPL_ARP_MON_CTX ctx, struct BPL_ARP_MON_ENTRY *entry)
{
    return (BPL_EXEC_SYM(bpl_arp_mon_process_raw_arp, ctx, entry));
}

int bpl_arp_mon_probe(BPL_ARP_MON_CTX ctx, const uint8_t mac[BPL_ARP_MON_MAC_LEN],
                      const uint8_t ip[BPL_ARP_MON_IP_LEN], int task_id)
{
    return (BPL_EXEC_SYM(bpl_arp_mon_probe, ctx, mac, ip, task_id));
}

int bpl_arp_mon_get_mac_for_ip(BPL_ARP_MON_CTX ctx, const uint8_t ip[BPL_ARP_MON_IP_LEN],
                               uint8_t mac[BPL_ARP_MON_MAC_LEN])
{
    return (BPL_EXEC_SYM(bpl_arp_mon_get_mac_for_ip, ctx, ip, mac));
}

int bpl_arp_mon_get_ip_for_mac(BPL_ARP_MON_CTX ctx, const uint8_t mac[BPL_ARP_MON_MAC_LEN],
                               uint8_t ip[BPL_ARP_MON_IP_LEN])
{
    return (BPL_EXEC_SYM(bpl_arp_mon_get_ip_for_mac, ctx, mac, ip));
}

int bpl_arp_get_bridge_iface(const char bridge[BPL_ARP_IFACE_NAME_LEN],
                             const uint8_t mac[BPL_ARP_MON_MAC_LEN],
                             char iface[BPL_ARP_IFACE_NAME_LEN])
{
    return (BPL_EXEC_SYM(bpl_arp_get_bridge_iface, bridge, mac, iface));
}
