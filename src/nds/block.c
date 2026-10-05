// The Gen 4 save's two partitions: which block copies the game loads, and full-save writes.

#include <string.h>

#include "nds/nds_internal.h"
#include "spec_internal.h"

constexpr size_t PARTITION_COUNT = 2;
// TODO: Check the block magic on a Korean cart.
constexpr uint32_t BLOCK_MAGIC = 0x20060623;

// Counted back from the block's end, where both footer kinds keep them.
constexpr size_t FOOTER_SIZE_FROM_END = 0xC;
constexpr size_t FOOTER_MAGIC_FROM_END = 0x8;
constexpr size_t FOOTER_ID_FROM_END = 0x4;
constexpr size_t FOOTER_CRC_FROM_END = 0x2;
// Counted from the footer's start; HeartGold and SoulSilver have only the save counter.
constexpr size_t FOOTER_SAVE_COUNTER = 0x0;
constexpr size_t FOOTER_BLOCK_COUNTER = 0x4;

enum block_id : uint8_t {
    GENERAL_BLOCK,
    STORAGE_BLOCK,
    BLOCK_COUNT,
};
typedef enum block_id block_id_t;

// An invalid copy's counters read as 0, as in the games.
struct block_copy {
    bool is_valid;
    size_t offset;
    uint32_t save_counter;
    uint32_t block_counter;
};
typedef struct block_copy block_copy_t;

// stale is only meaningful when both copies are valid.
struct block_choice {
    size_t valid_count;
    size_t current;
    size_t stale;
};
typedef struct block_choice block_choice_t;

static size_t block_size_of(block_id_t block_id, const spec_nds_layout_t *layout) {
    return block_id == GENERAL_BLOCK ? layout->general_size : layout->storage_size;
}

static size_t block_offset_in_partition(block_id_t block_id, const spec_nds_layout_t *layout) {
    return block_id == GENERAL_BLOCK ? 0 : layout->storage_offset;
}

static size_t other_partition_offset(size_t offset) {
    return offset < SPEC_NDS_PARTITION_SIZE ? offset + SPEC_NDS_PARTITION_SIZE
                                            : offset - SPEC_NDS_PARTITION_SIZE;
}

// As SaveBlockFooter_Validate; Diamond, Pearl and Platinum keep the id in one byte.
static bool is_block_valid(const uint8_t *block, block_id_t block_id,
                           const spec_nds_layout_t *layout) {
    size_t size = block_size_of(block_id, layout);
    const uint8_t *end = &block[size];
    uint16_t stored_id = layout->are_blocks_paired ? spec_read_u16_le(end - FOOTER_ID_FROM_END)
                                                   : *(end - FOOTER_ID_FROM_END);
    return spec_read_u32_le(end - FOOTER_SIZE_FROM_END) == size
           && spec_read_u32_le(end - FOOTER_MAGIC_FROM_END) == BLOCK_MAGIC && stored_id == block_id
           && spec_read_u16_le(end - FOOTER_CRC_FROM_END)
                  == spec_crc16(block, size - layout->footer_size);
}

static block_copy_t read_copy(const uint8_t *data, size_t partition, block_id_t block_id,
                              const spec_nds_layout_t *layout) {
    block_copy_t copy = {
        .offset = partition * SPEC_NDS_PARTITION_SIZE + block_offset_in_partition(block_id, layout),
    };
    const uint8_t *block = &data[copy.offset];
    const uint8_t *footer = &block[block_size_of(block_id, layout) - layout->footer_size];
    copy.is_valid = is_block_valid(block, block_id, layout);
    if (copy.is_valid) {
        copy.save_counter = spec_read_u32_le(&footer[FOOTER_SAVE_COUNTER]);
        copy.block_counter = layout->are_blocks_paired
                                 ? copy.save_counter
                                 : spec_read_u32_le(&footer[FOOTER_BLOCK_COUNTER]);
    }
    return copy;
}

// As SaveCheckInfo_CompareCounters: 0 follows 0xFFFFFFFF.
static int compare_counters(uint32_t first, uint32_t second) {
    if (first == UINT32_MAX && second == 0) {
        return -1;
    }
    if (first == 0 && second == UINT32_MAX) {
        return 1;
    }
    return (first > second) - (first < second);
}

// As SaveCheckInfo_CompareSectors: ties go to the first partition.
static block_choice_t choose_copy(const block_copy_t copies[static PARTITION_COUNT]) {
    if (copies[0].is_valid && copies[1].is_valid) {
        int save_order = compare_counters(copies[0].save_counter, copies[1].save_counter);
        int block_order = compare_counters(copies[0].block_counter, copies[1].block_counter);
        bool is_second_newer = save_order < 0 || (save_order == 0 && block_order < 0);
        return is_second_newer ? (block_choice_t){2, 1, 0} : (block_choice_t){2, 0, 1};
    }
    if (copies[0].is_valid) {
        return (block_choice_t){1, 0, 0};
    }
    if (copies[1].is_valid) {
        return (block_choice_t){1, 1, 1};
    }
    return (block_choice_t){};
}

// As SaveData_LoadCheck: each block picks its own copy, linked by the save counter.
static bool find_split_blocks(spec_nds_blocks_t *loaded,
                              const block_copy_t copies[static BLOCK_COUNT][PARTITION_COUNT]) {
    block_choice_t general = choose_copy(copies[GENERAL_BLOCK]);
    block_choice_t storage = choose_copy(copies[STORAGE_BLOCK]);
    if (general.valid_count == 0 || storage.valid_count == 0) {
        return false;
    }
    size_t general_partition = general.current;
    size_t storage_partition = storage.current;
    uint32_t general_save_counter = copies[GENERAL_BLOCK][general.current].save_counter;
    bool is_linked = general_save_counter == copies[STORAGE_BLOCK][storage.current].save_counter;
    if (!is_linked && general.valid_count == 2) {
        general_partition = general.stale;
    } else if (!is_linked && storage.valid_count == 2) {
        if (general_save_counter != copies[STORAGE_BLOCK][storage.stale].save_counter) {
            return false;
        }
        storage_partition = storage.stale;
    }
    const block_copy_t *general_copy = &copies[GENERAL_BLOCK][general_partition];
    const block_copy_t *storage_copy = &copies[STORAGE_BLOCK][storage_partition];
    *loaded = (spec_nds_blocks_t){
        .general_offset = general_copy->offset,
        .storage_offset = storage_copy->offset,
        .save_counter = general_copy->save_counter,
        .general_counter = general_copy->block_counter,
        .storage_counter = storage_copy->block_counter,
    };
    return true;
}

static bool are_counters_equal(const block_copy_t copies[static BLOCK_COUNT][PARTITION_COUNT],
                               size_t partition) {
    return copies[GENERAL_BLOCK][partition].save_counter
           == copies[STORAGE_BLOCK][partition].save_counter;
}

// As Save_GetSaveFilesStatus: both blocks come from one partition.
static bool find_paired_partition(size_t *partition,
                                  const block_copy_t copies[static BLOCK_COUNT][PARTITION_COUNT]) {
    block_choice_t general = choose_copy(copies[GENERAL_BLOCK]);
    block_choice_t storage = choose_copy(copies[STORAGE_BLOCK]);
    if (general.valid_count == 0 || storage.valid_count == 0) {
        return false;
    }
    if (general.valid_count == 1 && storage.valid_count == 1) {
        *partition = general.current;
        return general.current == storage.current;
    }
    if (are_counters_equal(copies, general.current)) {
        *partition = general.current;
        return true;
    }
    if (general.valid_count == 2 && are_counters_equal(copies, general.stale)) {
        *partition = general.stale;
        return true;
    }
    return false;
}

// A tool can leave the partition the game picks with an invalid copy; nothing loads then.
static bool find_paired_blocks(spec_nds_blocks_t *loaded,
                               const block_copy_t copies[static BLOCK_COUNT][PARTITION_COUNT]) {
    size_t partition = 0;
    if (!find_paired_partition(&partition, copies)) {
        return false;
    }
    const block_copy_t *general_copy = &copies[GENERAL_BLOCK][partition];
    const block_copy_t *storage_copy = &copies[STORAGE_BLOCK][partition];
    if (!general_copy->is_valid || !storage_copy->is_valid) {
        return false;
    }
    *loaded = (spec_nds_blocks_t){
        .general_offset = general_copy->offset,
        .storage_offset = storage_copy->offset,
        .save_counter = general_copy->save_counter,
    };
    return true;
}

bool spec_nds_find_loaded_blocks(spec_nds_blocks_t *loaded, const uint8_t *data,
                                 const spec_nds_layout_t *layout) {
    block_copy_t copies[BLOCK_COUNT][PARTITION_COUNT];
    for (size_t partition = 0; partition < PARTITION_COUNT; ++partition) {
        copies[GENERAL_BLOCK][partition] = read_copy(data, partition, GENERAL_BLOCK, layout);
        copies[STORAGE_BLOCK][partition] = read_copy(data, partition, STORAGE_BLOCK, layout);
    }
    if (layout->are_blocks_paired) {
        return find_paired_blocks(loaded, copies);
    }
    return find_split_blocks(loaded, copies);
}

// As a full save: each block goes opposite its loaded copy, and every counter advances.
spec_nds_blocks_t spec_nds_copy_to_next_blocks(uint8_t *data, const spec_nds_blocks_t *loaded,
                                               const spec_nds_layout_t *layout) {
    spec_nds_blocks_t next = {
        .general_offset = other_partition_offset(loaded->general_offset),
        .storage_offset = other_partition_offset(loaded->storage_offset),
        .save_counter = loaded->save_counter + 1,
        .general_counter = loaded->general_counter + 1,
        .storage_counter = loaded->storage_counter + 1,
    };
    memcpy(&data[next.general_offset], &data[loaded->general_offset], layout->general_size);
    memcpy(&data[next.storage_offset], &data[loaded->storage_offset], layout->storage_size);
    return next;
}

static void stamp_block(uint8_t *block, block_id_t block_id, uint32_t save_counter,
                        uint32_t block_counter, const spec_nds_layout_t *layout) {
    size_t size = block_size_of(block_id, layout);
    uint8_t *footer = &block[size - layout->footer_size];
    spec_write_u32_le(&footer[FOOTER_SAVE_COUNTER], save_counter);
    if (!layout->are_blocks_paired) {
        spec_write_u32_le(&footer[FOOTER_BLOCK_COUNTER], block_counter);
    }
    spec_write_u16_le(&block[size - FOOTER_CRC_FROM_END],
                      spec_crc16(block, size - layout->footer_size));
}

void spec_nds_stamp_blocks(uint8_t *data, const spec_nds_blocks_t *blocks,
                           const spec_nds_layout_t *layout) {
    stamp_block(&data[blocks->general_offset], GENERAL_BLOCK, blocks->save_counter,
                blocks->general_counter, layout);
    stamp_block(&data[blocks->storage_offset], STORAGE_BLOCK, blocks->save_counter,
                blocks->storage_counter, layout);
}
