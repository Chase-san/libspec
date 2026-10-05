// Internal helpers every console uses: errors, bytes and bits, UTF-8, PIDs, shuffles, stats.

#ifndef SPEC_INTERNAL_H
#define SPEC_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <uchar.h>

#include "spec.h"
#include "spec_tables.h"

spec_error_t spec_fail(spec_error_t error, const char *message);
spec_error_t spec_locate_error(spec_error_location_t location, uint32_t index0, uint32_t index1);

uint16_t spec_read_u16_le(const uint8_t *bytes);
uint32_t spec_read_u32_le(const uint8_t *bytes);
void spec_write_u16_le(uint8_t *bytes, uint16_t value);
void spec_write_u32_le(uint8_t *bytes, uint32_t value);
uint32_t spec_get_bits(uint32_t word, unsigned first_bit, unsigned bit_count);
uint32_t spec_set_bits(uint32_t word, unsigned first_bit, unsigned bit_count, uint32_t value);
bool spec_fits_in_bits(uint32_t value, unsigned bit_count);

constexpr char32_t SPEC_REPLACEMENT_CHARACTER = 0xFFFD;

// Return the bytes used; reading returns 0 for malformed UTF-8.
size_t spec_utf8_read(const char8_t *utf8, char32_t *code_point);
size_t spec_utf8_write(char8_t *utf8, char32_t code_point);

spec_gender_t spec_gender_from_ratio(spec_pid_t pid, uint8_t gender_ratio);
bool spec_is_shiny(spec_pid_t pid, uint16_t trainer_id, uint16_t secret_id);

// Moves a record's four data blocks to and from one of 24 stored orders, taken order % 24.
void spec_shuffle_blocks(uint8_t *blocks, size_t block_size, size_t order);
void spec_unshuffle_blocks(uint8_t *blocks, size_t block_size, size_t order);

uint8_t spec_level_for_experience(spec_growth_rate_t growth_rate, uint32_t experience);
// As CalcMonStats; HP ignores the nature.
uint16_t spec_calculate_stat(spec_stat_t stat, uint8_t base_stat, uint8_t iv, uint8_t ev,
                             uint8_t level, spec_nature_t nature);

#endif
