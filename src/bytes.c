// Little-endian reads and writes, and bit fields within a word.

#include "spec_internal.h"

uint16_t spec_read_u16_le(const uint8_t *bytes) {
    return (uint16_t)(bytes[0] | (bytes[1] << 8));
}

uint32_t spec_read_u32_le(const uint8_t *bytes) {
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) | ((uint32_t)bytes[2] << 16)
           | ((uint32_t)bytes[3] << 24);
}

void spec_write_u16_le(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
}

void spec_write_u32_le(uint8_t *bytes, uint32_t value) {
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
    bytes[2] = (uint8_t)(value >> 16);
    bytes[3] = (uint8_t)(value >> 24);
}

static uint32_t mask_of(unsigned bit_count) {
    return bit_count == 32 ? UINT32_MAX : (1U << bit_count) - 1;
}

uint32_t spec_get_bits(uint32_t word, unsigned first_bit, unsigned bit_count) {
    return (word >> first_bit) & mask_of(bit_count);
}

uint32_t spec_set_bits(uint32_t word, unsigned first_bit, unsigned bit_count, uint32_t value) {
    uint32_t mask = mask_of(bit_count) << first_bit;
    return (word & ~mask) | ((value << first_bit) & mask);
}

bool spec_fits_in_bits(uint32_t value, unsigned bit_count) {
    return value <= mask_of(bit_count);
}
