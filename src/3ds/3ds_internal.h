// Gen 6 and 7 internals: save layouts, the block footer, the save signature and the codecs
// save.c runs.

#ifndef SPEC_3DS_INTERNAL_H
#define SPEC_3DS_INTERNAL_H

#include <stddef.h>
#include <stdint.h>

#include "3ds/3ds.h"

constexpr size_t SPEC_3DS_BLOCK_MAX_COUNT = 58;
constexpr size_t SPEC_3DS_SHA1_SIZE = 20;
constexpr size_t SPEC_3DS_SHA256_SIZE = 32;
constexpr size_t SPEC_3DS_AES_BLOCK_SIZE = 16;
constexpr size_t SPEC_3DS_AES_ROUND_KEYS_SIZE = 176;
// Gen 7's signature, inside its signed block: the footer's SHA-256, then the RSA signature.
constexpr size_t SPEC_3DS_SIGNATURE_OFFSET = 0x100;
constexpr size_t SPEC_3DS_SIGNATURE_SIZE = 0x80;

// size is the payload the block's CRC covers.
struct spec_3ds_block {
    size_t offset;
    size_t size;
};
typedef struct spec_3ds_block spec_3ds_block_t;

// Section offsets are absolute; pocket offsets are bag-relative.
struct spec_3ds_layout {
    spec_game_type_t type;
    size_t save_size;
    size_t block_count;
    spec_3ds_block_t blocks[SPEC_3DS_BLOCK_MAX_COUNT];
    // Gen 7 signs a hash of the footer's first hashed_footer_size bytes inside signed_block.
    bool is_gen7;
    size_t signed_block;
    size_t hashed_footer_size;

    size_t trainer_offset;
    size_t misc_offset;
    size_t battle_points_offset;
    size_t play_time_offset;
    size_t options_offset;
    size_t pokedex_offset;
    bool has_bank_caught_flags;

    size_t party_offset;
    size_t box_info_offset;
    size_t boxes_offset;
    size_t box_count;
    size_t daycare_offset;
    size_t daycare_count;

    size_t bag_offset;
    size_t pocket_offsets[SPEC_3DS_POCKET_COUNT];
    size_t pocket_capacities[SPEC_3DS_POCKET_COUNT];
};
typedef struct spec_3ds_layout spec_3ds_layout_t;

// Layout functions

const spec_3ds_layout_t *spec_3ds_find_layout(size_t save_size);
const spec_3ds_layout_t *spec_3ds_get_layout(spec_game_type_t type);

// Footer functions

// As the game loads: the footer's table matches the layout's and every block's CRC its bytes.
bool spec_3ds_is_footer_valid(const uint8_t *data, const spec_3ds_layout_t *layout);

// Every block's CRC, Gen 7's signature region read as zeros.
void spec_3ds_stamp_footer(uint8_t *data, const spec_3ds_layout_t *layout);

// Signature functions

spec_error_t spec_3ds_check_signing_key(const spec_3ds_signing_key_t *signing_key);

// Run last: the signature covers the footer, and so every block's CRC.
void spec_3ds_sign_save(uint8_t *data, const spec_3ds_layout_t *layout,
                        const spec_3ds_signing_key_t *signing_key);

// Crypto functions: SHA-1 and SHA-256 (FIPS 180-4), AES-128 (FIPS 197) and textbook RSA

void spec_3ds_aes128_encrypt(uint8_t block[static SPEC_3DS_AES_BLOCK_SIZE],
                             const uint8_t round_keys[static SPEC_3DS_AES_ROUND_KEYS_SIZE]);
void spec_3ds_aes128_expand_key(uint8_t round_keys[static SPEC_3DS_AES_ROUND_KEYS_SIZE],
                                const uint8_t key[static SPEC_3DS_AES_BLOCK_SIZE]);
// Modulo the save key's modulus; numbers are big-endian, SPEC_3DS_SIGNING_KEY_SIZE bytes.
void spec_3ds_rsa_power(uint8_t result[static SPEC_3DS_SIGNING_KEY_SIZE],
                        const uint8_t base[static SPEC_3DS_SIGNING_KEY_SIZE],
                        const uint8_t *exponent, size_t exponent_size);
void spec_3ds_sha1(uint8_t digest[static SPEC_3DS_SHA1_SIZE], const uint8_t *bytes, size_t size);
void spec_3ds_sha256(uint8_t digest[static SPEC_3DS_SHA256_SIZE], const uint8_t *bytes,
                     size_t size);

// Player functions

void spec_3ds_decode_player(spec_3ds_save_t *save, const uint8_t *data,
                            const spec_3ds_layout_t *layout);
void spec_3ds_encode_player(uint8_t *data, const spec_3ds_layout_t *layout,
                            const spec_3ds_save_t *save);

spec_error_t spec_3ds_check_player(const spec_3ds_save_t *save, const spec_3ds_layout_t *layout);

// Pokédex functions

void spec_3ds_decode_pokedex(spec_3ds_pokedex_t *pokedex, const uint8_t *data,
                             const spec_3ds_layout_t *layout);
void spec_3ds_encode_pokedex(uint8_t *data, const spec_3ds_layout_t *layout,
                             const spec_3ds_pokedex_t *pokedex);

spec_error_t spec_3ds_check_pokedex(const spec_3ds_pokedex_t *pokedex,
                                    const spec_3ds_layout_t *layout);

// Storage functions

void spec_3ds_decode_storage(spec_3ds_save_t *save, const uint8_t *data,
                             const spec_3ds_layout_t *layout);
void spec_3ds_encode_storage(uint8_t *data, const spec_3ds_layout_t *layout,
                             const spec_3ds_save_t *save);

spec_error_t spec_3ds_check_storage(const spec_3ds_save_t *save, const spec_3ds_layout_t *layout);

// Pokémon functions

// Records are encrypted, as stored.
void spec_3ds_decode_pokemon(spec_3ds_pokemon_t *pokemon, const uint8_t *record, size_t record_size,
                             uint8_t generation);
spec_error_t spec_3ds_encode_pokemon(uint8_t *record, size_t record_size,
                                     const spec_3ds_pokemon_t *pokemon);

void spec_3ds_fill_party_data(spec_3ds_pokemon_t *pokemon);

// Item functions

void spec_3ds_decode_items(spec_3ds_save_t *save, const uint8_t *data,
                           const spec_3ds_layout_t *layout);
void spec_3ds_encode_items(uint8_t *data, const spec_3ds_layout_t *layout,
                           const spec_3ds_save_t *save);

spec_error_t spec_3ds_check_items(const spec_3ds_save_t *save, const spec_3ds_layout_t *layout);

#endif
