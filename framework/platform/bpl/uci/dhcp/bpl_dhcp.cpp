/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bpl/bpl_dhcp.h>

extern "C" {
// Ignore some warnings from libubus
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <libubox/blobmsg_json.h>
#include <libubus.h>
}

#include <string.h>

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace beerocks {
namespace bpl {

static struct ubus_context *s_pUbusCtx = NULL;
static dhcp_mon_cb s_pCallback         = NULL;

enum { DHCP_EVENT_OP, DHCP_EVENT_MAC, DHCP_EVENT_IP, DHCP_EVENT_HOSTNAME, __DHCP_EVENT_MAX };

static const struct blobmsg_policy dhcp_event_policy[] = {
    [DHCP_EVENT_OP]       = {.name = "op", .type = BLOBMSG_TYPE_STRING},
    [DHCP_EVENT_MAC]      = {.name = "mac", .type = BLOBMSG_TYPE_STRING},
    [DHCP_EVENT_IP]       = {.name = "ip", .type = BLOBMSG_TYPE_STRING},
    [DHCP_EVENT_HOSTNAME] = {.name = "hostname", .type = BLOBMSG_TYPE_STRING}};

struct dhcp_event_request {
    struct ubus_request_data req;
    struct uloop_timeout timeout;
    char data[];
};

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static int dhcp_event_handler(struct ubus_context *ctx, struct ubus_object *obj,
                              struct ubus_request_data *req, const char *method,
                              struct blob_attr *msg)
{
    struct blob_attr *tb[__DHCP_EVENT_MAX];

    const char *op       = "(unknown)";
    const char *mac      = "(unknown)";
    const char *ip       = "(unknown)";
    const char *hostname = "(unknown)";

    // Remove "unused variable" warnings
    (void)ctx;
    (void)obj;
    (void)req;
    (void)method;

    blobmsg_parse(dhcp_event_policy, ARRAY_SIZE(dhcp_event_policy), tb, blob_data(msg),
                  blob_len(msg));

    if (tb[DHCP_EVENT_OP]) {
        op = blobmsg_get_string(tb[DHCP_EVENT_OP]);
    }
    if (tb[DHCP_EVENT_MAC]) {
        mac = blobmsg_get_string(tb[DHCP_EVENT_MAC]);
    }
    if (tb[DHCP_EVENT_IP]) {
        ip = blobmsg_get_string(tb[DHCP_EVENT_IP]);
    }
    if (tb[DHCP_EVENT_HOSTNAME]) {
        hostname = blobmsg_get_string(tb[DHCP_EVENT_HOSTNAME]);
    }

    // Execute the callback
    if (s_pCallback)
        s_pCallback(op, mac, ip, hostname);

    // Send response to the caller
    // {
    //     static struct blob_buf b;

    //     blob_buf_init(&b, 0);
    //     blobmsg_add_string(&b, "message", "OK");
    //     ubus_send_reply(ctx, req, b.head);
    // }

    return 0;
}

static const struct ubus_method dhcp_ubus_methods[] = {{.name     = "dhcp_event",
                                                        .handler  = dhcp_event_handler,
                                                        .mask     = 0,
                                                        .tags     = 0,
                                                        .policy   = dhcp_event_policy,
                                                        .n_policy = ARRAY_SIZE(dhcp_event_policy)}};

static struct ubus_object_type dhcp_ubus_object_type = {.name      = "dhcp_event",
                                                        .id        = 0,
                                                        .methods   = dhcp_ubus_methods,
                                                        .n_methods = ARRAY_SIZE(dhcp_ubus_methods)};

static struct ubus_object dhcp_ubus_object = {
    .avl             = {},
    .name            = "dhcp_event",
    .id              = 0,
    .path            = 0,
    .type            = &dhcp_ubus_object_type,
    .subscribe_cb    = 0,
    .has_subscribers = 0,
    .methods         = dhcp_ubus_methods,
    .n_methods       = ARRAY_SIZE(dhcp_ubus_methods),
};

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int dhcp_mon_start(dhcp_mon_cb cb)
{
    // Modules doesn't support multiple starts
    if ((s_pUbusCtx != NULL) /*|| (pCallback == NULL)*/)
        return (-1);

    if ((s_pUbusCtx = ubus_connect(NULL)) == NULL)
        return (-1);

    // Register our object with the UBUS
    if (ubus_add_object(s_pUbusCtx, &dhcp_ubus_object) != 0)
        return (-1);

    // Store the callback
    s_pCallback = cb;

    // Return the UBUS fd
    return (s_pUbusCtx->sock.fd);
}

int dhcp_mon_handle_event()
{
    if (s_pUbusCtx == NULL)
        return -1;

    // Handle the UBUS event
    ubus_handle_event(s_pUbusCtx);

    return 0;
}

int dhcp_mon_stop()
{
    // If the modules wasn't started, exit quitely
    if (!s_pUbusCtx)
        return 0;

    // Remove our object from the UBUS
    if (ubus_remove_object(s_pUbusCtx, &dhcp_ubus_object))
        return -1;

    s_pCallback = NULL;
    ubus_free(s_pUbusCtx);

    return 0;
}

} // namespace bpl
} // namespace beerocks
