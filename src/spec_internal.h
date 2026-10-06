// Internal helpers every console uses: errors, bytes and bits, UTF-8, PIDs, shuffles, stats.

#ifndef SPEC_INTERNAL_H
#define SPEC_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <uchar.h>

#include "spec.h"
#include "spec_tables.h"

constexpr char32_t SPEC_REPLACEMENT_CHARACTER = 0xFFFD;

// Error functions

spec_error_t spec_fail(spec_error_t error, const char *message);
spec_error_t spec_locate_error(spec_error_location_t location, uint32_t index0, uint32_t index1);

// Name functions

// The names as Game Boy games, Gen 5 and Gen 3 and 4 store them; nullptr when the language has
// none.
const char8_t *spec_game_boy_species_name(uint16_t national_number, spec_language_t language);
const char8_t *spec_gen5_species_name(uint16_t national_number, spec_language_t language);
// Items as Gen 4 to 7 number them; Gen 1 to 3 number their own.
const char *spec_item_name(uint16_t item, spec_language_t language);
const char8_t *spec_upper_case_species_name(uint16_t national_number, spec_language_t language);

// Byte and bit functions

// Flags as bits of a word, flag 0 the lowest.
void spec_decode_flags(bool *flags, size_t flag_count, uint32_t word);
uint32_t spec_encode_flags(const bool *flags, size_t flag_count);
uint16_t spec_read_u16_be(const uint8_t *bytes);
uint16_t spec_read_u16_le(const uint8_t *bytes);
uint32_t spec_read_u24_be(const uint8_t *bytes);
uint32_t spec_read_u32_be(const uint8_t *bytes);
uint32_t spec_read_u32_le(const uint8_t *bytes);
void spec_write_u16_be(uint8_t *bytes, uint16_t value);
void spec_write_u16_le(uint8_t *bytes, uint16_t value);
void spec_write_u24_be(uint8_t *bytes, uint32_t value);
void spec_write_u32_be(uint8_t *bytes, uint32_t value);
void spec_write_u32_le(uint8_t *bytes, uint32_t value);

bool spec_fits_in_bits(uint32_t value, unsigned bit_count);
// Flag arrays, as the Pokédex and event flags store them: index / 8's byte, bit index % 8.
bool spec_get_array_flag(const uint8_t *flags, size_t index);
uint32_t spec_get_bits(uint32_t word, unsigned first_bit, unsigned bit_count);
bool spec_get_flag(uint32_t word, unsigned bit);
bool spec_is_all_zero(const uint8_t *bytes, size_t size);

void spec_set_array_flag(uint8_t *flags, size_t index, bool is_set);
uint32_t spec_set_bits(uint32_t word, unsigned first_bit, unsigned bit_count, uint32_t value);
uint32_t spec_set_flag(uint32_t word, unsigned bit, bool is_set);

// UTF-8 functions

// Return the bytes used; reading returns 0 for malformed UTF-8.
size_t spec_utf8_read(const char8_t *utf8, char32_t *code_point);
size_t spec_utf8_write(char8_t *utf8, char32_t code_point);

// CRC functions

uint16_t spec_crc16(const uint8_t *bytes, size_t size);
uint16_t spec_crc16_usb(const uint8_t *bytes, size_t size);

// Item slot functions

size_t spec_count_filled_slots(const spec_item_slot_t *item_slots, size_t slot_count);
bool spec_is_item_slot_empty(const spec_item_slot_t *item_slot);

void spec_condense_pocket(spec_item_slot_t *condensed, const spec_item_slot_t *item_slots,
                          size_t slot_count);

// Personality functions

spec_gender_t spec_gender_from_ratio(spec_pid_t pid, uint8_t gender_ratio);
bool spec_is_shiny(spec_pid_t pid, uint16_t trainer_id, uint16_t secret_id);

// Record functions

// The records' checksum: a wrapping sum of little-endian u16s.
uint16_t spec_sum_u16(const uint8_t *bytes, size_t size);

// Moves a record's four data blocks to and from one of 24 stored orders, taken order % 24.
void spec_shuffle_blocks(uint8_t *blocks, size_t block_size, size_t order);
void spec_unshuffle_blocks(uint8_t *blocks, size_t block_size, size_t order);
void spec_xor_with_random_stream(uint8_t *bytes, size_t size, uint32_t seed);

// Stat functions

// As CalcMonStats; HP ignores the nature.
uint16_t spec_calculate_stat(spec_stat_t stat, uint8_t base_stat, uint8_t iv, uint8_t ev,
                             uint8_t level, spec_nature_t nature);
// Gen 3's tables ask 1 experience of level 1; Gen 1 and 2's code, and Gen 4 on's tables
// (pokeplatinum's exp_tables.csv, the 3DS games' own), none.
uint32_t spec_experience_for_level(spec_growth_rate_t growth_rate, uint8_t level);
uint8_t spec_gen3_level_for_experience(spec_growth_rate_t growth_rate, uint32_t experience);
uint8_t spec_level_for_experience(spec_growth_rate_t growth_rate, uint32_t experience);

#endif
