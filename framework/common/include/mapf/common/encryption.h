/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Arnout Vandecappelle (Essensium/Mind)
 * Copyright (c) 2019 Tomer Eliyahu (Intel)
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
                     unsigned remote_pubkey_length) const;

    /**
     * @brief Get the public key
     */
    const uint8_t *pubkey() const { return m_pubkey; }

    /**
     * @brief Get the length of pubkey().
     */
    unsigned pubkey_length() const { return m_pubkey_length; }

    /**
     * @brief Get the nonce
     */
    const uint8_t *nonce() const { return m_nonce; }

    /**
     * @brief Get the length of nonce().
     */
    unsigned nonce_length() const { return sizeof(m_nonce); }

private:
    /**
     * OpenSSL Diffie-Hellman state structure.
     */
    struct dh_st *m_dh;

    /**
     * If keypair generation failed in the constructor, this will be @a nullptr.
     */
    uint8_t *m_pubkey = nullptr;
    unsigned m_pubkey_length;
    uint8_t m_nonce[16];
};

/**
 * @brief Create an initialization vector - a random byte stream used for generating the WSC authkey
 *        and keywrapkey which are used for encrypting the WSC M2 config_data according to the WSC
 *        specification v2.0.6.
 * 
 * @param[in/out] iv buffer of generated initialization vector
 * @param[in] iv_length length of initialization buffer
 * @return true on success
 * @return false on failure
 */
bool create_iv(uint8_t *iv, unsigned iv_length);

/**
 * @brief Compute KWA (Key Wrap Attribute)
 *        KWA = 1st 64 Bits of HMAC(authkey, DataToEncrypt)
 *        See section 7.5 in WiFi Simple configuration technical specification v2.0.6
 * 
 * @param[in] authkey 32 bytes authkey, calculated using wps_calculate_keys()
 * @param[in] data ConfigData before encryption
 * @param[in] data_len ConfigData length in bytes
 * @param[out] kwa 8 bytes calculated Key Wrap attribute (64 bits of HMAC(ConfigData))
 * @return true on success
 * @return false on failure
 */
bool kwa_compute(const uint8_t *key, uint8_t *data, uint32_t data_len, uint8_t *kwa);

/**
 * @brief AES encryption
 *
 * @param[in] key 32 byte KeyWrapKey calculated according to WSC v2.0.6 specification
 * @param[in] iv random 128bit input vector
 * @param[in/out] plaintext bytestream, aligned to 16 bytes boundary
 * @param[in] data_len plaintext buffer length
 * @return true on success
 * @return false on error
 */
bool aes_encrypt(const uint8_t *key, const uint8_t *iv, uint8_t *data, uint32_t data_len);

/**
 * @brief AES decryption
 *
 * @param[in] key 32 byte KeyWrapKey calculated according to WSC v2.0.6 specification
 * @param[in] iv random 128bit input vector
 * @param[in/out] cyphertext bytestream, aligned to 16 bytes boundary
 * @param[in] data_len cyphertext buffer length
 * @return true on success
 * @return false on error
 */
bool aes_decrypt(const uint8_t *key, const uint8_t *iv, uint8_t *data, uint32_t data_len);

/**
 * @brief Calculate WPS secret authkey and KeyWrapKey based on remote and local public keys
 *        generated with diffie-hellman key exchange, WSC M1 and M2 nonces, and M1 MAC address.
 *        The authkey used for the Key Wrap Algorithm to generate the KWA, used for the Key Wrap
 *        Authenticator attribute of the M2 WSC TLV.
 *        The keywrapkey is used for AES encryption/decryption of M2 WSC encrypted settings.
 *
 * @param[in] dh diffie helman instance containing the local public key
 * @param[in] remote_pubkey remote public key (from M1 or M2 WSC TLV)
 * @param[in] remote_pubkey_length remote public key length
 * @param[in] m1_nonce WSC M1 TLV enrollee nonce attribute
 * @param[in] mac WSC M1 TLV mac attribute
 * @param[in] m2_nonce WSC M2 TLV registrar nonce attribute
 * @param[out] authkey calculated 256 bit authentication key
 * @param[out] keywrapkey calculated 128 bit keywrapkey
 * @return true on success, false on error
 */
bool wps_calculate_keys(const diffie_hellman &dh, const uint8_t *remote_pubkey,
                        unsigned remote_pubkey_length, const uint8_t *m1_nonce, const uint8_t *mac,
                        const uint8_t *m2_nonce, uint8_t *authkey, uint8_t *keywrapkey);

} // namespace encryption
} // namespace mapf
#endif // ENCRYPTION_H_
