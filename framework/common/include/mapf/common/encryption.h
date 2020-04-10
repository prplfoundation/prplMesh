/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
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

    diffie_hellman &operator=(const diffie_hellman &) = delete;

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

    /**
     * @brief Maximum length of pubkey().
     */
    static const unsigned max_pubkey_length = 192;

private:
    /**
     * OpenSSL Diffie-Hellman state structure.
     */
    struct dh_st *m_dh;

    /**
     * If keypair generation failed in the constructor, this will be @a nullptr.
     */
    uint8_t *m_pubkey        = nullptr;
    unsigned m_pubkey_length = 0;
    uint8_t m_nonce[16];

    diffie_hellman(const diffie_hellman &) = delete;
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
 * @brief AES128-CBC Encryption
 * 
 * Perform AES128 CBC (cipher block chaining) encryption on given plaintext
 * and write the output encrypted data into the ciphertext buffer.
 * plaintext and ciphertext can be the same buffer (in place encryption).
 * Padding is handled internally, so the encrypted buffer can be up to 16
 * bytes larger than the plaintext buffer, and therefore this function
 * will fail if the given clen is not big enough (>= plen + 15).
 * Key and IV should be the same as block size, which 16 bytes
 * since we are using 128 bit AES (i.e. a 128 bit key).
 * The IV size for *most* modes is the same as the block size.
 * 
 * @param[in] key 16 bytes encryption key
 * @param[in] iv 16 bytes random initialization vector 
 * @param[in] plaintext bytestream (data to encrypt)
 * @param[in] plen plaintext buffer length
 * @param[out] ciphertext output buffer of encrypted data (can be the same as plaintext buffer)
 * @param[in,out] clen in - length of ciphertext buffer (>= plen + 15) out - length of output ciphertext
 * @return true on success
 * @return false failure
 */
bool aes_encrypt(const uint8_t *key, const uint8_t *iv, uint8_t *plaintext, int plen,
                 uint8_t *ciphertext, int &clen);

/**
 * @brief AES128-CBC Decryption
 * 
 * Perform AES128 CBC (cipher block chaining) decryption on given ciphertext
 * and write the output decrypted data into the plaintext buffer.
 * plaintext and ciphertext can be the same buffer (in place decryption).
 * Padding is handled internally, so the decrypted buffer can be up to 16
 * bytes larger than the plaintext buffer, and therefore this function
 * will fail if the given plen is not big enough (>= plen + 16).
 * Key and IV should be the same as block size, which is 16 bytes
 * since we are using 128 bit AES (i.e. a 128 bit key).
 * The IV size for *most* modes is the same as the block size.
 * 
 * @param[in] key 16 byte encryption key
 * @param[in] iv 16 bytes random initialization vector 
 * @param[in] ciphertext bytestream (data to decrypt)
 * @param[in] clen length of ciphertext buffer
 * @param[out] plaintext output buffer of encrypted data (can be the same as ciphertext buffer)
 * @param[in,out] plen in - length of plaintext buffer (>= clen + 16) out - length of output plaintext
 * @return true on success
 * @return false on failure
 */
bool aes_decrypt(const uint8_t *key, const uint8_t *iv, uint8_t *ciphertext, int clen,
                 uint8_t *plaintext, int &plen);

/**
 * @brief Copy the public key of @a dh to @a dest
 *
 * Copy the public key of @a dh to @a dest, padding with zeroes on the left. Size of @a dest must be
 * pubkey_max_length.
 */
void copy_pubkey(const diffie_hellman &dh, uint8_t *dest);

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
void wps_calculate_keys(const diffie_hellman &dh, const uint8_t *remote_pubkey,
                        unsigned remote_pubkey_length, const uint8_t *m1_nonce, const uint8_t *mac,
                        const uint8_t *m2_nonce, uint8_t *authkey, uint8_t *keywrapkey);

} // namespace encryption
} // namespace mapf
#endif // ENCRYPTION_H_
