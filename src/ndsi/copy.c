// The Gen 5 save's two copies: which one loads, the block checksums, and the mirrored write.

#include <string.h>

#include "ndsi/ndsi_internal.h"
#include "spec_internal.h"

constexpr uint32_t FOOTER_MAGIC = 0x31053527;
constexpr size_t FOOTER_SAVE_COUNT_OFFSET = 0x0;
constexpr size_t FOOTER_USED_SIZE_OFFSET = 0x4;
constexpr size_t FOOTER_MAGIC_OFFSET = 0x8;
constexpr size_t FOOTER_CRC_OFFSET = 0xE;
constexpr size_t FOOTER_SIZE = 0x10;
constexpr size_t TRAILER_COUNTER_OFFSET = 0x0;
constexpr size_t TRAILER_CRC_OFFSET = 0x2;

static size_t used_size_of(const spec_ndsi_layout_t *layout) {
    return layout->footer_offset + FOOTER_SIZE;
}

// One u16 per block, then the table's own slot, which no known checksum explains.
static size_t table_size_of(const spec_ndsi_layout_t *layout) {
    return (layout->block_count + 1) * 2;
}

static size_t other_copy_offset(size_t copy_offset, const spec_ndsi_layout_t *layout) {
    return copy_offset == 0 ? layout->copy_size : 0;
}

static bool is_block_valid(const uint8_t *copy, size_t block_index,
                           const spec_ndsi_layout_t *layout) {
    const spec_ndsi_block_t *block = &layout->blocks[block_index];
    uint16_t crc = spec_crc16(&copy[block->offset], block->size);
    const uint8_t *trailer = &copy[block->offset + block->size];
    const uint8_t *table_slot = &copy[layout->table_offset + block_index * 2];
    return spec_read_u16_le(&trailer[TRAILER_CRC_OFFSET]) == crc
           && spec_read_u16_le(table_slot) == crc;
}

static bool is_copy_valid(const uint8_t *copy, const spec_ndsi_layout_t *layout) {
    const uint8_t *footer = &copy[layout->footer_offset];
    bool is_footer_valid =
        spec_read_u32_le(&footer[FOOTER_MAGIC_OFFSET]) == FOOTER_MAGIC
        && spec_read_u32_le(&footer[FOOTER_USED_SIZE_OFFSET]) == used_size_of(layout)
        && spec_read_u16_le(&footer[FOOTER_CRC_OFFSET])
               == spec_crc16(&copy[layout->table_offset], table_size_of(layout));
    if (!is_footer_valid) {
        return false;
    }
    for (size_t block_index = 0; block_index < layout->block_count; ++block_index) {
        if (!is_block_valid(copy, block_index, layout)) {
            return false;
        }
    }
    return true;
}

// The game's own rule is unknown; this one agrees with "newest wins" but after a cut-off write.
bool spec_ndsi_find_loaded_copy(size_t *copy_offset, const uint8_t *data,
                                const spec_ndsi_layout_t *layout) {
    if (is_copy_valid(&data[0], layout)) {
        *copy_offset = 0;
        return true;
    }
    if (is_copy_valid(&data[layout->copy_size], layout)) {
        *copy_offset = layout->copy_size;
        return true;
    }
    return false;
}

size_t spec_ndsi_copy_to_other(uint8_t *data, size_t loaded_offset,
                               const spec_ndsi_layout_t *layout) {
    size_t other_offset = other_copy_offset(loaded_offset, layout);
    memcpy(&data[other_offset], &data[loaded_offset], used_size_of(layout));
    return other_offset;
}

// As the game saves: a block's counter advances only when its bytes change.
static void stamp_block(uint8_t *copy, const uint8_t *previous, size_t block_index,
                        const spec_ndsi_layout_t *layout) {
    const spec_ndsi_block_t *block = &layout->blocks[block_index];
    uint8_t *trailer = &copy[block->offset + block->size];
    if (memcmp(&copy[block->offset], &previous[block->offset], block->size) != 0) {
        uint16_t counter = spec_read_u16_le(&trailer[TRAILER_COUNTER_OFFSET]);
        spec_write_u16_le(&trailer[TRAILER_COUNTER_OFFSET], (uint16_t)(counter + 1));
    }
    uint16_t crc = spec_crc16(&copy[block->offset], block->size);
    spec_write_u16_le(&trailer[TRAILER_CRC_OFFSET], crc);
    spec_write_u16_le(&copy[layout->table_offset + block_index * 2], crc);
}

void spec_ndsi_stamp_copy(uint8_t *data, size_t copy_offset, size_t previous_offset,
                          const spec_ndsi_layout_t *layout) {
    uint8_t *copy = &data[copy_offset];
    for (size_t block_index = 0; block_index < layout->block_count; ++block_index) {
        stamp_block(copy, &data[previous_offset], block_index, layout);
    }
    uint8_t *footer = &copy[layout->footer_offset];
    uint32_t save_count = spec_read_u32_le(&footer[FOOTER_SAVE_COUNT_OFFSET]);
    spec_write_u32_le(&footer[FOOTER_SAVE_COUNT_OFFSET], save_count + 1);
    spec_write_u16_le(&footer[FOOTER_CRC_OFFSET],
                      spec_crc16(&copy[layout->table_offset], table_size_of(layout)));
}

// As the game saves: both copies end the same.
void spec_ndsi_mirror_copy(uint8_t *data, size_t from_offset, size_t to_offset,
                           const spec_ndsi_layout_t *layout) {
    memcpy(&data[to_offset], &data[from_offset], used_size_of(layout));
}
