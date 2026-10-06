// Powers modulo the 768-bit modulus of Gen 7's save key, by Montgomery multiplication.

#include <string.h>

#include "3ds/3ds_internal.h"
#include "spec_internal.h"

constexpr size_t LIMB_COUNT = SPEC_3DS_SIGNING_KEY_SIZE / 4;
constexpr size_t LIMB_BIT_COUNT = 32;

// The public key's modulus, which Sun's and Moon's code holds with the private exponent.
constexpr uint8_t MODULUS[SPEC_3DS_SIGNING_KEY_SIZE] = {
    0xB6, 0x1E, 0x19, 0x20, 0x91, 0xF9, 0x0A, 0x8F, 0x76, 0xA6, 0xEA, 0xAA, 0x9A, 0x3C, 0xE5, 0x8C,
    0x86, 0x3F, 0x39, 0xAE, 0x25, 0x3F, 0x03, 0x78, 0x16, 0xF5, 0x97, 0x58, 0x54, 0xE0, 0x7A, 0x9A,
    0x45, 0x66, 0x01, 0xE7, 0xC9, 0x4C, 0x29, 0x75, 0x9F, 0xE1, 0x55, 0xC0, 0x64, 0xED, 0xDF, 0xA1,
    0x11, 0x44, 0x3F, 0x81, 0xEF, 0x1A, 0x42, 0x8C, 0xF6, 0xCD, 0x32, 0xF9, 0xDA, 0xC9, 0xD4, 0x8E,
    0x94, 0xCF, 0xB3, 0xF6, 0x90, 0x12, 0x0E, 0x8E, 0x6B, 0x91, 0x11, 0xAD, 0xDA, 0xF1, 0x1E, 0x7C,
    0x96, 0x20, 0x8C, 0x37, 0xC0, 0x14, 0x3F, 0xF2, 0xBF, 0x3D, 0x7E, 0x83, 0x11, 0x41, 0xA9, 0x73,
};

// Least significant limb first.
typedef uint32_t number_t[LIMB_COUNT];

static bool is_at_least(const uint32_t *left, const number_t right, uint32_t left_top) {
    if (left_top != 0) {
        return true;
    }
    for (size_t limb = LIMB_COUNT; limb-- > 0;) {
        if (left[limb] != right[limb]) {
            return left[limb] > right[limb];
        }
    }
    return true;
}

static void subtract(uint32_t *left, const number_t right) {
    uint64_t borrow = 0;
    for (size_t limb = 0; limb < LIMB_COUNT; ++limb) {
        uint64_t difference = (uint64_t)left[limb] - right[limb] - borrow;
        left[limb] = (uint32_t)difference;
        borrow = (difference >> 63) & 1;
    }
}

static void number_from_bytes(number_t number,
                              const uint8_t bytes[static SPEC_3DS_SIGNING_KEY_SIZE]) {
    for (size_t limb = 0; limb < LIMB_COUNT; ++limb) {
        number[limb] = spec_read_u32_be(&bytes[SPEC_3DS_SIGNING_KEY_SIZE - (limb + 1) * 4]);
    }
}

static void number_to_bytes(uint8_t bytes[static SPEC_3DS_SIGNING_KEY_SIZE],
                            const number_t number) {
    for (size_t limb = 0; limb < LIMB_COUNT; ++limb) {
        spec_write_u32_be(&bytes[SPEC_3DS_SIGNING_KEY_SIZE - (limb + 1) * 4], number[limb]);
    }
}

// -modulus^-1 modulo 2^32, by Newton's iteration, which doubles the correct bits each step.
static uint32_t negative_inverse_of(uint32_t low_limb) {
    uint32_t inverse = 1;
    for (unsigned step = 0; step < 5; ++step) {
        inverse *= 2 - low_limb * inverse;
    }
    return (uint32_t)-inverse;
}

// left * right / 2^768 modulo the modulus (coarsely integrated operand scanning).
static void montgomery_multiply(number_t result, const number_t left, const number_t right,
                                const number_t modulus, uint32_t negative_inverse) {
    uint32_t sum[LIMB_COUNT + 2] = {};
    for (size_t index = 0; index < LIMB_COUNT; ++index) {
        uint64_t carry = 0;
        for (size_t limb = 0; limb < LIMB_COUNT; ++limb) {
            uint64_t product = (uint64_t)left[limb] * right[index] + sum[limb] + carry;
            sum[limb] = (uint32_t)product;
            carry = product >> LIMB_BIT_COUNT;
        }
        uint64_t top = (uint64_t)sum[LIMB_COUNT] + carry;
        sum[LIMB_COUNT] = (uint32_t)top;
        sum[LIMB_COUNT + 1] = (uint32_t)(top >> LIMB_BIT_COUNT);
        uint32_t factor = sum[0] * negative_inverse;
        carry = ((uint64_t)factor * modulus[0] + sum[0]) >> LIMB_BIT_COUNT;
        for (size_t limb = 1; limb < LIMB_COUNT; ++limb) {
            uint64_t product = (uint64_t)factor * modulus[limb] + sum[limb] + carry;
            sum[limb - 1] = (uint32_t)product;
            carry = product >> LIMB_BIT_COUNT;
        }
        top = (uint64_t)sum[LIMB_COUNT] + carry;
        sum[LIMB_COUNT - 1] = (uint32_t)top;
        sum[LIMB_COUNT] = sum[LIMB_COUNT + 1] + (uint32_t)(top >> LIMB_BIT_COUNT);
    }
    if (is_at_least(sum, modulus, sum[LIMB_COUNT])) {
        subtract(sum, modulus);
    }
    memcpy(result, sum, sizeof(number_t));
}

// 2^1536 modulo the modulus, by doubling 1 that many times, which converts into Montgomery form.
static void montgomery_square_of_radix(number_t result, const number_t modulus) {
    uint32_t doubled[LIMB_COUNT + 1] = {1};
    for (size_t step = 0; step < 2 * LIMB_COUNT * LIMB_BIT_COUNT; ++step) {
        uint32_t carry = 0;
        for (size_t limb = 0; limb <= LIMB_COUNT; ++limb) {
            uint32_t next_carry = doubled[limb] >> (LIMB_BIT_COUNT - 1);
            doubled[limb] = (doubled[limb] << 1) | carry;
            carry = next_carry;
        }
        if (is_at_least(doubled, modulus, doubled[LIMB_COUNT])) {
            subtract(doubled, modulus);
            doubled[LIMB_COUNT] = 0;
        }
    }
    memcpy(result, doubled, sizeof(number_t));
}

void spec_3ds_rsa_power(uint8_t result[static SPEC_3DS_SIGNING_KEY_SIZE],
                        const uint8_t base[static SPEC_3DS_SIGNING_KEY_SIZE],
                        const uint8_t *exponent, size_t exponent_size) {
    number_t modulus;
    number_from_bytes(modulus, MODULUS);
    uint32_t negative_inverse = negative_inverse_of(modulus[0]);
    number_t radix_squared;
    montgomery_square_of_radix(radix_squared, modulus);
    number_t one = {1};
    number_t base_number;
    number_from_bytes(base_number, base);
    number_t power;
    number_t base_montgomery;
    montgomery_multiply(power, one, radix_squared, modulus, negative_inverse);
    montgomery_multiply(base_montgomery, base_number, radix_squared, modulus, negative_inverse);
    for (size_t byte = 0; byte < exponent_size; ++byte) {
        for (unsigned bit = 8; bit-- > 0;) {
            montgomery_multiply(power, power, power, modulus, negative_inverse);
            if (((exponent[byte] >> bit) & 1) != 0) {
                montgomery_multiply(power, power, base_montgomery, modulus, negative_inverse);
            }
        }
    }
    number_t plain;
    montgomery_multiply(plain, power, one, modulus, negative_inverse);
    number_to_bytes(result, plain);
}
