/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Arnout Vandecappelle (Essensium/Mind)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef ENCRYPTION_H_
#define ENCRYPTION_H_

#include <cstdint>

extern "C" {
/** Internal defintion from OpenSSL, avoids requiring include of openssl headers. */
struct dh_st;
}

namespace mapf {

/**
 * @brief Wrapper functions for performing encryption
 */
namespace encryption {

///
/// @brief Diffie-Hellman key exchange
///
class diffie_hellman {
public:
    /**
     * @brief Generate a keypair suitable for Diffie-Hellman key exchange
     */
    diffie_hellman();

    ~diffie_hellman();

    /**
     * @brief Compute the shared secret key
     *
     * Diffie-Hellman generates a shared secret key based on a local private key
     * (contained in this object) and a remote public key.
     *
     * @param[out] key The derived shared secret key
     * @param[in,out] key_length Length of @a key buffer, updated to actual length on output
     * @param[in] remote_pubkey The public key of the peer
     * @param[in] remote_pubkey_length Length of @a remote_pubkey.
     * @return true if successful, false if not.
     */
    bool compute_key(uint8_t *key, unsigned &key_length, const uint8_t *remote_pubkey,
                     unsigned remote_pubkey_length);

    /**
     * @brief Get the public key
     */
    const uint8_t *pubkey() { return m_pubkey; }

    /**
     * @brief Get the length of pubkey().
     */
    unsigned pubkey_length() { return m_pubkey_length; }

private:
    /**
     * OpenSSL Diffie-Hellman state structure.
     */
    struct dh_st *m_dh;

    /**
     * If keypair generation failed in the constructor, this will be @a nullptr.
     */
    uint8_t *m_pubkey;
    unsigned m_pubkey_length;
};

} // namespace encryption
} // namespace mapf
#endif // ENCRYPTION_H_
