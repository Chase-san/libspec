// Little- and big-endian reads and writes, bit fields and flags, and zero checks.

#include "spec_internal.h"

static uint32_t mask_of(unsigned bit_count) {
    return bit_count == 32 ? UINT32_MAX : (1U << bit_count) - 1;
}

void spec_decode_flags(bool *flags, size_t flag_count, uint32_t word) {
    for (unsigned flag = 0; flag < flag_count; ++flag) {
        flags[flag] = spec_get_flag(word, flag);
    }
}

uint32_t spec_encode_flags(const bool *flags, size_t flag_count) {
    uint32_t word = 0;
    for (unsigned flag = 0; flag < flag_count; ++flag) {
        word = spec_set_flag(word, flag, flags[flag]);
    }
    return word;
}

uint16_t spec_read_u16_be(const uint8_t *bytes) {
    return (uint16_t)((bytes[0] << 8) | bytes[1]);
}

uint16_t spec_read_u16_le(const uint8_t *bytes) {
    return (uint16_t)(bytes[0] | (bytes[1] << 8));
}

uint32_t spec_read_u24_be(const uint8_t *bytes) {
    return ((uint32_t)bytes[0] << 16) | ((uint32_t)bytes[1] << 8) | (uint32_t)bytes[2];
}

uint32_t spec_read_u32_le(const uint8_t *bytes) {
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) | ((uint32_t)bytes[2] << 16)
           | ((uint32_t)bytes[3] << 24);
}

void spec_write_u16_be(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

void spec_write_u16_le(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
}

void spec_write_u24_be(uint8_t *bytes, uint32_t value) {
    bytes[0] = (uint8_t)(value >> 16);
    bytes[1] = (uint8_t)(value >> 8);
    bytes[2] = (uint8_t)value;
}

void spec_write_u32_le(uint8_t *bytes, uint32_t value) {
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
    bytes[2] = (uint8_t)(value >> 16);
    bytes[3] = (uint8_t)(value >> 24);
}

bool spec_fits_in_bits(uint32_t value, unsigned bit_count) {
    return value <= mask_of(bit_count);
}

bool spec_get_array_flag(const uint8_t *flags, size_t index) {
    return spec_get_flag(flags[index / 8], (unsigned)(index % 8));
}

uint32_t spec_get_bits(uint32_t word, unsigned first_bit, unsigned bit_count) {
    return (word >> first_bit) & mask_of(bit_count);
}

bool spec_get_flag(uint32_t word, unsigned bit) {
    return spec_get_bits(word, bit, 1) != 0;
}

bool spec_is_all_zero(const uint8_t *bytes, size_t size) {
    for (size_t index = 0; index < size; ++index) {
        if (bytes[index] != 0) {
            return false;
        }
    }
    return true;
}

void spec_set_array_flag(uint8_t *flags, size_t index, bool is_set) {
    flags[index / 8] = (uint8_t)spec_set_flag(flags[index / 8], (unsigned)(index % 8), is_set);
}

uint32_t spec_set_bits(uint32_t word, unsigned first_bit, unsigned bit_count, uint32_t value) {
    uint32_t mask = mask_of(bit_count) << first_bit;
    return (word & ~mask) | ((value << first_bit) & mask);
}

uint32_t spec_set_flag(uint32_t word, unsigned bit, bool is_set) {
    return spec_set_bits(word, bit, 1, is_set);
}
