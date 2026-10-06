// SHA-1 and SHA-256 (FIPS 180-4), each over one buffer, for Gen 7's save signature.

#include <string.h>

#include "3ds/3ds_internal.h"
#include "spec_internal.h"

constexpr size_t BLOCK_SIZE = 64;
constexpr size_t LENGTH_SIZE = 8;
constexpr uint8_t PADDING_START = 0x80;
constexpr size_t SHA1_WORD_COUNT = 5;
constexpr size_t SHA256_WORD_COUNT = 8;

constexpr uint32_t SHA1_INITIAL[SHA1_WORD_COUNT] = {
    0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0,
};
constexpr uint32_t SHA1_ROUND_CONSTANTS[4] = {0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6};

constexpr uint32_t SHA256_INITIAL[SHA256_WORD_COUNT] = {
    0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A, 0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19,
};
constexpr uint32_t SHA256_ROUND_CONSTANTS[64] = {
    0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
    0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3, 0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
    0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
    0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7, 0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
    0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13, 0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
    0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
    0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
    0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208, 0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2,
};

// The message's last bytes, then 0x80, zeros and the message's length in bits: one or two blocks.
static size_t pad_tail(uint8_t tail[static 2 * BLOCK_SIZE], const uint8_t *bytes, size_t size) {
    size_t remainder = size % BLOCK_SIZE;
    size_t tail_size = remainder + 1 + LENGTH_SIZE <= BLOCK_SIZE ? BLOCK_SIZE : 2 * BLOCK_SIZE;
    memset(tail, 0, 2 * BLOCK_SIZE);
    memcpy(tail, &bytes[size - remainder], remainder);
    tail[remainder] = PADDING_START;
    uint64_t bit_count = (uint64_t)size * 8;
    spec_write_u32_be(&tail[tail_size - LENGTH_SIZE], (uint32_t)(bit_count >> 32));
    spec_write_u32_be(&tail[tail_size - LENGTH_SIZE / 2], (uint32_t)bit_count);
    return tail_size;
}

static uint32_t rotate_left(uint32_t word, unsigned count) {
    return (word << count) | (word >> (32 - count));
}

static void sha1_compress(uint32_t state[static SHA1_WORD_COUNT], const uint8_t *block) {
    uint32_t schedule[80];
    for (size_t index = 0; index < 16; ++index) {
        schedule[index] = spec_read_u32_be(&block[index * 4]);
    }
    for (size_t index = 16; index < 80; ++index) {
        schedule[index] = rotate_left(schedule[index - 3] ^ schedule[index - 8]
                                          ^ schedule[index - 14] ^ schedule[index - 16],
                                      1);
    }
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3], e = state[4];
    for (size_t index = 0; index < 80; ++index) {
        uint32_t mixed = 0;
        if (index < 20) {
            mixed = (b & c) | (~b & d);
        } else if (index < 40 || index >= 60) {
            mixed = b ^ c ^ d;
        } else {
            mixed = (b & c) | (b & d) | (c & d);
        }
        uint32_t next =
            rotate_left(a, 5) + mixed + e + SHA1_ROUND_CONSTANTS[index / 20] + schedule[index];
        e = d;
        d = c;
        c = rotate_left(b, 30);
        b = a;
        a = next;
    }
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

void spec_3ds_sha1(uint8_t digest[static SPEC_3DS_SHA1_SIZE], const uint8_t *bytes, size_t size) {
    uint32_t state[SHA1_WORD_COUNT];
    memcpy(state, SHA1_INITIAL, sizeof state);
    for (size_t offset = 0; offset + BLOCK_SIZE <= size; offset += BLOCK_SIZE) {
        sha1_compress(state, &bytes[offset]);
    }
    uint8_t tail[2 * BLOCK_SIZE];
    size_t tail_size = pad_tail(tail, bytes, size);
    for (size_t offset = 0; offset < tail_size; offset += BLOCK_SIZE) {
        sha1_compress(state, &tail[offset]);
    }
    for (size_t index = 0; index < SHA1_WORD_COUNT; ++index) {
        spec_write_u32_be(&digest[index * 4], state[index]);
    }
}

static uint32_t rotate_right(uint32_t word, unsigned count) {
    return (word >> count) | (word << (32 - count));
}

static void sha256_compress(uint32_t state[static SHA256_WORD_COUNT], const uint8_t *block) {
    uint32_t schedule[64];
    for (size_t index = 0; index < 16; ++index) {
        schedule[index] = spec_read_u32_be(&block[index * 4]);
    }
    for (size_t index = 16; index < 64; ++index) {
        uint32_t word_15 = schedule[index - 15];
        uint32_t word_2 = schedule[index - 2];
        uint32_t sigma0 = rotate_right(word_15, 7) ^ rotate_right(word_15, 18) ^ (word_15 >> 3);
        uint32_t sigma1 = rotate_right(word_2, 17) ^ rotate_right(word_2, 19) ^ (word_2 >> 10);
        schedule[index] = schedule[index - 16] + sigma0 + schedule[index - 7] + sigma1;
    }
    uint32_t working[SHA256_WORD_COUNT];
    memcpy(working, state, sizeof working);
    for (size_t index = 0; index < 64; ++index) {
        uint32_t a = working[0], b = working[1], c = working[2], e = working[4];
        uint32_t f = working[5], g = working[6], h = working[7];
        uint32_t sum1 = rotate_right(e, 6) ^ rotate_right(e, 11) ^ rotate_right(e, 25);
        uint32_t choice = (e & f) ^ (~e & g);
        uint32_t first = h + sum1 + choice + SHA256_ROUND_CONSTANTS[index] + schedule[index];
        uint32_t sum0 = rotate_right(a, 2) ^ rotate_right(a, 13) ^ rotate_right(a, 22);
        uint32_t majority = (a & b) ^ (a & c) ^ (b & c);
        memmove(&working[1], &working[0], (SHA256_WORD_COUNT - 1) * sizeof working[0]);
        working[4] += first;
        working[0] = first + sum0 + majority;
    }
    for (size_t index = 0; index < SHA256_WORD_COUNT; ++index) {
        state[index] += working[index];
    }
}

void spec_3ds_sha256(uint8_t digest[static SPEC_3DS_SHA256_SIZE], const uint8_t *bytes,
                     size_t size) {
    uint32_t state[SHA256_WORD_COUNT];
    memcpy(state, SHA256_INITIAL, sizeof state);
    for (size_t offset = 0; offset + BLOCK_SIZE <= size; offset += BLOCK_SIZE) {
        sha256_compress(state, &bytes[offset]);
    }
    uint8_t tail[2 * BLOCK_SIZE];
    size_t tail_size = pad_tail(tail, bytes, size);
    for (size_t offset = 0; offset < tail_size; offset += BLOCK_SIZE) {
        sha256_compress(state, &tail[offset]);
    }
    for (size_t index = 0; index < SHA256_WORD_COUNT; ++index) {
        spec_write_u32_be(&digest[index * 4], state[index]);
    }
}
