// Gen 1 checksums: the game data's, which loading checks, and the box banks', which only saving
// writes.

#include "gb/gb_internal.h"

// As CalcCheckSum: the complement of the bytes' sum.
uint8_t spec_gb_checksum(const uint8_t *bytes, size_t size) {
    uint8_t sum = 0;
    for (size_t index = 0; index < size; ++index) {
        sum = (uint8_t)(sum + bytes[index]);
    }
    return (uint8_t)~sum;
}

static size_t game_data_size(const spec_gb_layout_t *layout) {
    return layout->checksum_offset - SPEC_GB_GAME_DATA_OFFSET;
}

bool spec_gb_is_game_data_valid(const uint8_t *data, const spec_gb_layout_t *layout) {
    uint8_t checksum = spec_gb_checksum(&data[SPEC_GB_GAME_DATA_OFFSET], game_data_size(layout));
    return checksum == data[layout->checksum_offset];
}

void spec_gb_stamp_game_data(uint8_t *data, const spec_gb_layout_t *layout) {
    data[layout->checksum_offset] =
        spec_gb_checksum(&data[SPEC_GB_GAME_DATA_OFFSET], game_data_size(layout));
}

// As CopyBoxToOrFromSRAM: the bank's checksum, then each box's where the game keeps them.
void spec_gb_stamp_box_bank(uint8_t *data, const spec_gb_layout_t *layout, size_t bank) {
    uint8_t *boxes = &data[bank * SPEC_GB_BANK_SIZE];
    size_t box_size = spec_gb_list_size(&layout->box_shape);
    size_t boxes_size = layout->boxes_per_bank * box_size;
    size_t summed_size = layout->does_bank_checksum_sum_itself ? boxes_size + 1 : boxes_size;
    boxes[boxes_size] = spec_gb_checksum(boxes, summed_size);
    if (!layout->has_box_checksums) {
        return;
    }
    for (size_t box = 0; box < layout->boxes_per_bank; ++box) {
        boxes[boxes_size + 1 + box] = spec_gb_checksum(&boxes[box * box_size], box_size);
    }
}
