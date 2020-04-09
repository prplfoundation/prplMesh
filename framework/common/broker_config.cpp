/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <json-c/json.h>
#include <mapf/broker/broker_config.h>
#include <mapf/common/err.h>
#include <mapf/common/logger.h>

namespace mapf {

void BrokerConfig::Print()
{
    MAPF_INFO("name: " << name);
    MAPF_INFO("monitor: " << monitor);
    MAPF_INFO("path: " << path);
    MAPF_INFO("sockets: ");
    MAPF_INFO("	frontend: ");
    for (auto &endpoint : frontend)
        MAPF_INFO("		" << endpoint);
    MAPF_INFO("	backend: ");
    for (auto &endpoint : backend)
        MAPF_INFO("		" << endpoint);
    MAPF_INFO("	capture: ");
    for (auto &endpoint : capture)
        MAPF_INFO("		" << endpoint);

    MAPF_INFO("security: ");
    MAPF_INFO("	enable: " << security.enable);
    MAPF_INFO("	key_len: " << security.key_len);
    MAPF_INFO("	xsub_secret: " << security.xsub_secret);
    MAPF_INFO("	xsub_public: " << security.xsub_public);
    MAPF_INFO("	pub_secret: " << security.pub_secret);
    MAPF_INFO("	pub_public: " << security.pub_public);
    MAPF_INFO("	xpub_secret: " << security.xpub_secret);
    MAPF_INFO("	xpub_public: " << security.xpub_public);
    MAPF_INFO("	sub_secret: " << security.sub_secret);
    MAPF_INFO("	sub_public: " << security.sub_public);
}

int BrokerConfig::Parse()
{
    struct json_object *jobj = json_object_from_file(path.c_str());
    mapf_assert(jobj);

    struct json_object *jsecurity, *jsockets, *jbroker, *jtmp;

    if (json_object_object_get_ex(jobj, "broker", &jbroker)) {
        if (json_object_object_get_ex(jbroker, "name", &jtmp))
            name = json_object_get_string(jtmp);
        if (json_object_object_get_ex(jbroker, "monitor", &jtmp))
            monitor = json_object_get_boolean(jtmp);
        if (json_object_object_get_ex(jbroker, "sockets", &jsockets)) {
            if (json_object_object_get_ex(jsockets, "frontend", &jtmp))
                frontend.insert(json_object_get_string(jtmp));
            if (json_object_object_get_ex(jsockets, "backend", &jtmp))
                backend.insert(json_object_get_string(jtmp));
            if (json_object_object_get_ex(jsockets, "capture", &jtmp))
                capture.insert(json_object_get_string(jtmp));
        }
        if (json_object_object_get_ex(jbroker, "security", &jsecurity)) {
            if (json_object_object_get_ex(jsecurity, "enable", &jtmp))
                security.enable = json_object_get_boolean(jtmp);
            if (json_object_object_get_ex(jsecurity, "key_size", &jtmp))
                security.key_len = json_object_get_int(jtmp);
            if (json_object_object_get_ex(jsecurity, "XSUB_secret", &jtmp))
                security.xsub_secret = json_object_get_string(jtmp);
            if (json_object_object_get_ex(jsecurity, "XSUB_public", &jtmp))
                security.xsub_public = json_object_get_string(jtmp);
            if (json_object_object_get_ex(jsecurity, "PUB_secret", &jtmp))
                security.pub_secret = json_object_get_string(jtmp);
            if (json_object_object_get_ex(jsecurity, "PUB_public", &jtmp))
                security.pub_public = json_object_get_string(jtmp);
            if (json_object_object_get_ex(jsecurity, "XPUB_secret", &jtmp))
                security.xpub_secret = json_object_get_string(jtmp);
            if (json_object_object_get_ex(jsecurity, "XPUB_public", &jtmp))
                security.xpub_public = json_object_get_string(jtmp);
            if (json_object_object_get_ex(jsecurity, "SUB_secret", &jtmp))
                security.sub_secret = json_object_get_string(jtmp);
            if (json_object_object_get_ex(jsecurity, "SUB_public", &jtmp))
                security.sub_public = json_object_get_string(jtmp);
        }
    }

    return 0;
}

} // namespace mapf
