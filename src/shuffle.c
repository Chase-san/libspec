// A Gen 3 or Gen 4 record's four data blocks, moved to and from their stored order.

#include <string.h>

#include "spec_internal.h"

constexpr size_t BLOCK_COUNT = 4;
constexpr size_t ORDER_COUNT = 24;
constexpr size_t BLOCK_MAX_SIZE = 32;

// The stored position of each block for each shuffle order.
constexpr uint8_t STORED_POSITIONS[ORDER_COUNT][BLOCK_COUNT] = {
    {0, 1, 2, 3}, {0, 1, 3, 2}, {0, 2, 1, 3}, {0, 3, 1, 2}, {0, 2, 3, 1}, {0, 3, 2, 1},
    {1, 0, 2, 3}, {1, 0, 3, 2}, {2, 0, 1, 3}, {3, 0, 1, 2}, {2, 0, 3, 1}, {3, 0, 2, 1},
    {1, 2, 0, 3}, {1, 3, 0, 2}, {2, 1, 0, 3}, {3, 1, 0, 2}, {2, 3, 0, 1}, {3, 2, 0, 1},
    {1, 2, 3, 0}, {1, 3, 2, 0}, {2, 1, 3, 0}, {3, 1, 2, 0}, {2, 3, 1, 0}, {3, 2, 1, 0},
};

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
