// Gen 7's save signature: a SHA-256 of the footer, through the games' AES mode, then RSA.

#include <string.h>

#include "3ds/3ds_internal.h"
#include "spec_internal.h"

constexpr size_t FOOTER_SIZE = 0x200;
// The signed message: the hash, zeros, then the first bytes of a SHA-1 over the two.
constexpr size_t MESSAGE_SIZE = 0x80;
constexpr size_t MESSAGE_CHECK_OFFSET = 0x78;
constexpr size_t MESSAGE_CHECK_SIZE = 8;
constexpr size_t CIPHER_OFFSET = SPEC_3DS_SHA256_SIZE;
constexpr size_t CIPHER_BLOCK_COUNT = (MESSAGE_SIZE - CIPHER_OFFSET) / SPEC_3DS_AES_BLOCK_SIZE;
constexpr uint8_t SUBKEY_POLYNOMIAL = 0x87;
// The top bit is dropped so the message stays below the modulus.
constexpr uint8_t BELOW_MODULUS_MASK = 0x7F;

// The public key as DER (RFC 5280's SubjectPublicKeyInfo), which keys the AES.
constexpr uint8_t PUBLIC_KEY_DER[] = {
    0x30, 0x7C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05,
    0x00, 0x03, 0x6B, 0x00, 0x30, 0x68, 0x02, 0x61, 0x00, 0xB6, 0x1E, 0x19, 0x20, 0x91, 0xF9, 0x0A,
    0x8F, 0x76, 0xA6, 0xEA, 0xAA, 0x9A, 0x3C, 0xE5, 0x8C, 0x86, 0x3F, 0x39, 0xAE, 0x25, 0x3F, 0x03,
    0x78, 0x16, 0xF5, 0x97, 0x58, 0x54, 0xE0, 0x7A, 0x9A, 0x45, 0x66, 0x01, 0xE7, 0xC9, 0x4C, 0x29,
    0x75, 0x9F, 0xE1, 0x55, 0xC0, 0x64, 0xED, 0xDF, 0xA1, 0x11, 0x44, 0x3F, 0x81, 0xEF, 0x1A, 0x42,
    0x8C, 0xF6, 0xCD, 0x32, 0xF9, 0xDA, 0xC9, 0xD4, 0x8E, 0x94, 0xCF, 0xB3, 0xF6, 0x90, 0x12, 0x0E,
    0x8E, 0x6B, 0x91, 0x11, 0xAD, 0xDA, 0xF1, 0x1E, 0x7C, 0x96, 0x20, 0x8C, 0x37, 0xC0, 0x14, 0x3F,
    0xF2, 0xBF, 0x3D, 0x7E, 0x83, 0x11, 0x41, 0xA9, 0x73, 0x02, 0x03, 0x01, 0x00, 0x01,
};

// The private exponent, which every Gen 7 game's code holds with the modulus as an RSAPrivateKey
// (on the US carts, ExeFS/.code#0x4A4030 in Sun and Moon, #0x4CC770 in Ultra Sun and Ultra
// Moon); every Gen 7 save in hand is signed with it.
constexpr uint8_t PRIVATE_EXPONENT[SPEC_3DS_RSA_SIZE] = {
    0x77, 0x54, 0x55, 0x66, 0x8F, 0xFF, 0x3C, 0xBA, 0x30, 0x26, 0xC2, 0xD0, 0xB2, 0x6B, 0x80, 0x85,
    0x89, 0x59, 0x58, 0x34, 0x11, 0x57, 0xAE, 0xB0, 0x3B, 0x6B, 0x04, 0x95, 0xEE, 0x57, 0x80, 0x3E,
    0x21, 0x86, 0xEB, 0x6C, 0xB2, 0xEB, 0x62, 0xA7, 0x1D, 0xF1, 0x8A, 0x3C, 0x9C, 0x65, 0x79, 0x07,
    0x76, 0x70, 0x96, 0x1B, 0x3A, 0x61, 0x02, 0xDA, 0xBE, 0x5A, 0x19, 0x4A, 0xB5, 0x8C, 0x32, 0x50,
    0xAE, 0xD5, 0x97, 0xFC, 0x78, 0x97, 0x8A, 0x32, 0x6D, 0xB1, 0xD7, 0xB2, 0x8D, 0xCC, 0xCB, 0x2A,
    0x3E, 0x01, 0x4E, 0xDB, 0xD3, 0x97, 0xAD, 0x33, 0xB8, 0xF2, 0x8C, 0xD5, 0x25, 0x05, 0x42, 0x51,
};

static uint8_t *signature_of(uint8_t *data, const spec_3ds_layout_t *layout) {
    return &data[layout->blocks[layout->signed_block].offset + SPEC_3DS_SIGNATURE_OFFSET];
}

static void xor_block(uint8_t *block, const uint8_t *other) {
    for (size_t index = 0; index < SPEC_3DS_AES_BLOCK_SIZE; ++index) {
        block[index] ^= other[index];
    }
}

// Multiplication by x in GF(2^128), as CMAC derives its subkeys.
static void double_block(uint8_t block[static SPEC_3DS_AES_BLOCK_SIZE]) {
    bool is_top_bit_set = (block[0] & 0x80) != 0;
    for (size_t index = 0; index < SPEC_3DS_AES_BLOCK_SIZE; ++index) {
        uint8_t next_bit = index + 1 < SPEC_3DS_AES_BLOCK_SIZE ? block[index + 1] >> 7 : 0;
        block[index] = (uint8_t)((block[index] << 1) | next_bit);
    }
    if (is_top_bit_set) {
        block[SPEC_3DS_AES_BLOCK_SIZE - 1] ^= SUBKEY_POLYNOMIAL;
    }
}

// CBC forward, a subkey from the first and last blocks, then a backward pass chained by inputs.
static void encrypt_message(uint8_t *blocks,
                            const uint8_t round_keys[static SPEC_3DS_AES_ROUND_KEYS_SIZE]) {
    uint8_t previous[SPEC_3DS_AES_BLOCK_SIZE] = {};
    for (size_t index = 0; index < CIPHER_BLOCK_COUNT; ++index) {
        uint8_t *block = &blocks[index * SPEC_3DS_AES_BLOCK_SIZE];
        xor_block(block, previous);
        spec_3ds_aes128_encrypt(block, round_keys);
        memcpy(previous, block, sizeof previous);
    }
    uint8_t subkey[SPEC_3DS_AES_BLOCK_SIZE];
    memcpy(subkey, blocks, sizeof subkey);
    xor_block(subkey, &blocks[(CIPHER_BLOCK_COUNT - 1) * SPEC_3DS_AES_BLOCK_SIZE]);
    double_block(subkey);
    for (size_t index = 0; index < CIPHER_BLOCK_COUNT; ++index) {
        xor_block(&blocks[index * SPEC_3DS_AES_BLOCK_SIZE], subkey);
    }
    uint8_t next_input[SPEC_3DS_AES_BLOCK_SIZE] = {};
    for (size_t index = CIPHER_BLOCK_COUNT; index-- > 0;) {
        uint8_t *block = &blocks[index * SPEC_3DS_AES_BLOCK_SIZE];
        uint8_t input[SPEC_3DS_AES_BLOCK_SIZE];
        memcpy(input, block, sizeof input);
        spec_3ds_aes128_encrypt(block, round_keys);
        xor_block(block, next_input);
        memcpy(next_input, input, sizeof next_input);
    }
}

static void build_message(uint8_t message[static MESSAGE_SIZE],
                          const uint8_t hash[static SPEC_3DS_SHA256_SIZE]) {
    memset(message, 0, MESSAGE_SIZE);
    memcpy(message, hash, SPEC_3DS_SHA256_SIZE);
    uint8_t check[SPEC_3DS_SHA1_SIZE];
    spec_3ds_sha1(check, message, MESSAGE_CHECK_OFFSET);
    memcpy(&message[MESSAGE_CHECK_OFFSET], check, MESSAGE_CHECK_SIZE);
}

// The AES key: the first bytes of a SHA-1 over the public key and the hash.
static void derive_round_keys(uint8_t round_keys[static SPEC_3DS_AES_ROUND_KEYS_SIZE],
                              const uint8_t hash[static SPEC_3DS_SHA256_SIZE]) {
    uint8_t keyed[sizeof PUBLIC_KEY_DER + SPEC_3DS_SHA256_SIZE];
    memcpy(keyed, PUBLIC_KEY_DER, sizeof PUBLIC_KEY_DER);
    memcpy(&keyed[sizeof PUBLIC_KEY_DER], hash, SPEC_3DS_SHA256_SIZE);
    uint8_t digest[SPEC_3DS_SHA1_SIZE];
    spec_3ds_sha1(digest, keyed, sizeof keyed);
    spec_3ds_aes128_expand_key(round_keys, digest);
}

void spec_3ds_sign_save(uint8_t *data, const spec_3ds_layout_t *layout) {
    uint8_t hash[SPEC_3DS_SHA256_SIZE];
    spec_3ds_sha256(hash, &data[layout->save_size - FOOTER_SIZE], layout->hashed_footer_size);
    uint8_t message[MESSAGE_SIZE];
    build_message(message, hash);
    uint8_t round_keys[SPEC_3DS_AES_ROUND_KEYS_SIZE];
    derive_round_keys(round_keys, hash);
    encrypt_message(&message[CIPHER_OFFSET], round_keys);
    message[CIPHER_OFFSET] &= BELOW_MODULUS_MASK;
    uint8_t *signature = signature_of(data, layout);
    memcpy(signature, hash, SPEC_3DS_SHA256_SIZE);
    spec_3ds_rsa_power(&signature[SPEC_3DS_SHA256_SIZE], &message[CIPHER_OFFSET], PRIVATE_EXPONENT);
}
