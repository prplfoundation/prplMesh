/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Arnout Vandecappelle (Essensium/Mind)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <mapf/common/encryption.h>
#include <mapf/common/logger.h>

MAPF_INITIALIZE_LOGGER

static bool check(int &errors, bool check, std::string message)
{
    if (check) {
        MAPF_DBG(" OK  ") << message;
    } else {
        MAPF_ERR("FAIL ") << message;
        errors++;
    }
    return check;
}

int main()
{
    mapf::Logger::Instance().LoggerInit("encryption_test");
    int errors = 0;

    MAPF_INFO("Start encryption test");
    mapf::encryption::diffie_hellman m1;
    mapf::encryption::diffie_hellman m2;

    uint8_t key1[192];
    uint8_t key2[192];
    unsigned key1_length = sizeof(key1);
    unsigned key2_length = sizeof(key2);
    std::fill(key1, key1 + key1_length, 1);
    std::fill(key2, key2 + key2_length, 2);

    check(errors, m1.compute_key(key1, key1_length, m2.pubkey(), m2.pubkey_length()),
          "Compute key 1");
    check(errors, m2.compute_key(key2, key2_length, m1.pubkey(), m1.pubkey_length()),
          "Compute key 2");
    MAPF_INFO(key1_length);

    if (check(errors, key1_length == key2_length, "Keys should be equal size")) {
        check(errors, std::equal(key1, key1 + key1_length, key2), "Keys should be equal");
    }

    check(errors, m1.compute_key(key1, key1_length, m1.pubkey(), m1.pubkey_length()),
          "Compute key 1 with wrong pubkey");
    if (check(errors, key1_length == key2_length, "Keys should be equal size")) {
        check(errors, !std::equal(key1, key1 + key1_length, key2),
              "With wrong pubkey, keys should be different");
    }

    key1_length = 10;
    check(errors, !m1.compute_key(key1, key1_length, m2.pubkey(), m2.pubkey_length()),
          "Too small shared key buffer");

    return errors;
}
