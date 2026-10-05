// Gen 2's two copies of the game data: which one the game loads, and writing both as it saves.

#include <string.h>

#include "gbc/gbc_internal.h"
#include "spec_internal.h"

constexpr size_t CHECK_VALUE_1_OFFSET = 0x2008;
constexpr uint8_t CHECK_VALUE_1 = 99;
constexpr uint8_t CHECK_VALUE_2 = 127;
// Each copy's second check value follows its checksum.
constexpr size_t CHECK_VALUE_2_DISTANCE = 2;

// As Checksum: a wrapping 16-bit sum of the bytes.
static uint16_t sum_bytes(const uint8_t *bytes, size_t size) {
    uint16_t sum = 0;
    for (size_t index = 0; index < size; ++index) {
        sum = (uint16_t)(sum + bytes[index]);
    }
    return sum;
}

static uint16_t sum_backup(const uint8_t *data, const spec_gbc_layout_t *layout) {
    uint16_t sum = 0;
    for (size_t index = 0; index < layout->backup_chunk_count; ++index) {
        const spec_gbc_backup_chunk_t *chunk = &layout->backup_chunks[index];
        sum = (uint16_t)(sum + sum_bytes(&data[chunk->backup_offset], chunk->size));
    }
    return sum;
}

// As TryLoadSaveData, which offers CONTINUE when either copy's check values hold.
bool spec_gbc_has_save(const uint8_t *data, const spec_gbc_layout_t *layout) {
    bool is_primary_marked =
        data[CHECK_VALUE_1_OFFSET] == CHECK_VALUE_1
        && data[layout->checksum_offset + CHECK_VALUE_2_DISTANCE] == CHECK_VALUE_2;
    bool is_backup_marked =
        data[layout->backup_check_value_offset] == CHECK_VALUE_1
        && data[layout->backup_checksum_offset + CHECK_VALUE_2_DISTANCE] == CHECK_VALUE_2;
    return is_primary_marked || is_backup_marked;
}

// As VerifyChecksum, which loading then trusts alone.
bool spec_gbc_is_primary_valid(const uint8_t *data, const spec_gbc_layout_t *layout) {
    uint16_t sum = sum_bytes(&data[SPEC_GBC_GAME_DATA_OFFSET], layout->game_data_size);
    return sum == spec_read_u16_le(&data[layout->checksum_offset]);
}

bool spec_gbc_is_backup_valid(const uint8_t *data, const spec_gbc_layout_t *layout) {
    return sum_backup(data, layout) == spec_read_u16_le(&data[layout->backup_checksum_offset]);
}

// As loading the backup does, which then saves it over the primary.
void spec_gbc_restore_primary(uint8_t *data, const spec_gbc_layout_t *layout) {
    for (size_t index = 0; index < layout->backup_chunk_count; ++index) {
        const spec_gbc_backup_chunk_t *chunk = &layout->backup_chunks[index];
        memcpy(&data[chunk->primary_offset], &data[chunk->backup_offset], chunk->size);
    }
}

// As _SaveGameData: the primary's check values and checksum, then the backup copied from it.
void spec_gbc_stamp_copies(uint8_t *data, const spec_gbc_layout_t *layout) {
    data[CHECK_VALUE_1_OFFSET] = CHECK_VALUE_1;
    data[layout->checksum_offset + CHECK_VALUE_2_DISTANCE] = CHECK_VALUE_2;
    spec_write_u16_le(&data[layout->checksum_offset],
                      sum_bytes(&data[SPEC_GBC_GAME_DATA_OFFSET], layout->game_data_size));
    for (size_t index = 0; index < layout->backup_chunk_count; ++index) {
        const spec_gbc_backup_chunk_t *chunk = &layout->backup_chunks[index];
        memcpy(&data[chunk->backup_offset], &data[chunk->primary_offset], chunk->size);
    }
    data[layout->backup_check_value_offset] = CHECK_VALUE_1;
    data[layout->backup_checksum_offset + CHECK_VALUE_2_DISTANCE] = CHECK_VALUE_2;
    spec_write_u16_le(&data[layout->backup_checksum_offset], sum_backup(data, layout));
}
