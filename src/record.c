// A Pokémon record's data blocks: their stored order, their encryption and their checksum.

#include <string.h>

#include "spec_internal.h"

constexpr size_t BLOCK_COUNT = 4;
constexpr size_t ORDER_COUNT = 24;
// Gen 3 to 5's blocks are 12 or 32 bytes, Gen 6 and 7's 56.
constexpr size_t BLOCK_MAX_SIZE = 56;

// The stored position of each block for each shuffle order.
constexpr uint8_t STORED_POSITIONS[ORDER_COUNT][BLOCK_COUNT] = {
    {0, 1, 2, 3}, {0, 1, 3, 2}, {0, 2, 1, 3}, {0, 3, 1, 2}, {0, 2, 3, 1}, {0, 3, 2, 1},
    {1, 0, 2, 3}, {1, 0, 3, 2}, {2, 0, 1, 3}, {3, 0, 1, 2}, {2, 0, 3, 1}, {3, 0, 2, 1},
    {1, 2, 0, 3}, {1, 3, 0, 2}, {2, 1, 0, 3}, {3, 1, 0, 2}, {2, 3, 0, 1}, {3, 2, 0, 1},
    {1, 2, 3, 0}, {1, 3, 2, 0}, {2, 1, 3, 0}, {3, 1, 2, 0}, {2, 3, 1, 0}, {3, 2, 1, 0},
};

uint16_t spec_sum_u16(const uint8_t *bytes, size_t size) {
    uint16_t sum = 0;
    for (size_t offset = 0; offset < size; offset += 2) {
        sum = (uint16_t)(sum + spec_read_u16_le(&bytes[offset]));
    }
    return sum;
}

void spec_shuffle_blocks(uint8_t *blocks, size_t block_size, size_t order) {
    const uint8_t *positions = STORED_POSITIONS[order % ORDER_COUNT];
    uint8_t in_order[BLOCK_COUNT * BLOCK_MAX_SIZE];
    memcpy(in_order, blocks, BLOCK_COUNT * block_size);
    for (size_t block = 0; block < BLOCK_COUNT; ++block) {
        memcpy(&blocks[positions[block] * block_size], &in_order[block * block_size], block_size);
    }
}

void spec_unshuffle_blocks(uint8_t *blocks, size_t block_size, size_t order) {
    const uint8_t *positions = STORED_POSITIONS[order % ORDER_COUNT];
    uint8_t as_stored[BLOCK_COUNT * BLOCK_MAX_SIZE];
    memcpy(as_stored, blocks, BLOCK_COUNT * block_size);
    for (size_t block = 0; block < BLOCK_COUNT; ++block) {
        memcpy(&blocks[block * block_size], &as_stored[positions[block] * block_size], block_size);
    }
}

// As Pokemon_EncryptData: the generator's draws, seeded by the key, mask each u16.
void spec_xor_with_random_stream(uint8_t *bytes, size_t size, uint32_t seed) {
    for (size_t offset = 0; offset < size; offset += 2) {
        uint16_t mask = spec_random(&seed);
        spec_write_u16_le(&bytes[offset], (uint16_t)(spec_read_u16_le(&bytes[offset]) ^ mask));
    }
}
