// The Gen 6 and 7 save's footer: the table of its blocks, with their CRCs.

#include <string.h>

#include "3ds/3ds_internal.h"
#include "spec_internal.h"

// The community's. TODO: Verify against the carts.
constexpr size_t FOOTER_SIZE = 0x200;
constexpr size_t FOOTER_MAGIC_OFFSET = 0x10;
constexpr uint32_t FOOTER_MAGIC = 0x42454546;
constexpr size_t FOOTER_ENTRIES_OFFSET = 0x14;
constexpr size_t FOOTER_ENTRY_SIZE = 8;
constexpr size_t ENTRY_ID_OFFSET = 4;
constexpr size_t ENTRY_CRC_OFFSET = 6;
constexpr size_t SIGNED_BLOCK_SIZE = 0x200;

static size_t entry_offset(const spec_3ds_layout_t *layout, size_t block_index) {
    size_t footer_offset = layout->save_size - FOOTER_SIZE;
    return footer_offset + FOOTER_ENTRIES_OFFSET + block_index * FOOTER_ENTRY_SIZE;
}

// Gen 7 signs after it stamps, so the signed block's CRC reads the signature as zeros.
static uint16_t block_crc(const uint8_t *data, const spec_3ds_layout_t *layout,
                          size_t block_index) {
    const spec_3ds_block_t *block = &layout->blocks[block_index];
    if (!layout->is_gen7) {
        return spec_crc16(&data[block->offset], block->size);
    }
    if (block_index != layout->signed_block) {
        return spec_crc16_usb(&data[block->offset], block->size);
    }
    uint8_t signed_block[SIGNED_BLOCK_SIZE];
    memcpy(signed_block, &data[block->offset], SIGNED_BLOCK_SIZE);
    memset(&signed_block[SPEC_3DS_SIGNATURE_OFFSET], 0, SPEC_3DS_SIGNATURE_SIZE);
    return spec_crc16_usb(signed_block, SIGNED_BLOCK_SIZE);
}

bool spec_3ds_is_footer_valid(const uint8_t *data, const spec_3ds_layout_t *layout) {
    size_t footer_offset = layout->save_size - FOOTER_SIZE;
    if (spec_read_u32_le(&data[footer_offset + FOOTER_MAGIC_OFFSET]) != FOOTER_MAGIC) {
        return false;
    }
    for (size_t block_index = 0; block_index < layout->block_count; ++block_index) {
        const uint8_t *entry = &data[entry_offset(layout, block_index)];
        bool is_entry_valid =
            spec_read_u32_le(entry) == layout->blocks[block_index].size
            && spec_read_u16_le(&entry[ENTRY_ID_OFFSET]) == block_index
            && spec_read_u16_le(&entry[ENTRY_CRC_OFFSET]) == block_crc(data, layout, block_index);
        if (!is_entry_valid) {
            return false;
        }
    }
    return true;
}

void spec_3ds_stamp_footer(uint8_t *data, const spec_3ds_layout_t *layout) {
    for (size_t block_index = 0; block_index < layout->block_count; ++block_index) {
        uint8_t *entry = &data[entry_offset(layout, block_index)];
        spec_write_u16_le(&entry[ENTRY_CRC_OFFSET], block_crc(data, layout, block_index));
    }
}
