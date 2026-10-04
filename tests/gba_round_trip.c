#include <stdio.h>
#include <string.h>

#include "gba/gba.h"

constexpr size_t SECTOR_SIZE = 0x1000;
constexpr size_t SECTORS_PER_SLOT = 14;
constexpr size_t SLOT_SIZE = SECTORS_PER_SLOT * SECTOR_SIZE;
constexpr size_t SECTION_ID_OFFSET = 0xFF4;
constexpr size_t SIGNATURE_OFFSET = 0xFF8;
constexpr size_t COUNTER_OFFSET = 0xFFC;
constexpr uint32_t SECTOR_SIGNATURE = 0x08012025;

static uint8_t original[SPEC_GBA_SAVE_SIZE];
static uint8_t written[SPEC_GBA_SAVE_SIZE];

static bool read_file(uint8_t *data, const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == nullptr) {
        return false;
    }
    size_t size = fread(data, 1, SPEC_GBA_SAVE_SIZE, file);
    fclose(file);
    return size == SPEC_GBA_SAVE_SIZE;
}

static uint32_t read_u32(const uint8_t *bytes) {
    return (uint32_t)bytes[0] | (uint32_t)bytes[1] << 8 | (uint32_t)bytes[2] << 16
           | (uint32_t)bytes[3] << 24;
}

static uint32_t counter_of_slot(const uint8_t *data, size_t slot) {
    return read_u32(&data[slot * SLOT_SIZE + COUNTER_OFFSET]);
}

// An erased slot reads as all 0xFF.
static size_t newer_slot(const uint8_t *data) {
    if (read_u32(&data[SIGNATURE_OFFSET]) != SECTOR_SIGNATURE) {
        return 1;
    }
    if (read_u32(&data[SLOT_SIZE + SIGNATURE_OFFSET]) != SECTOR_SIGNATURE) {
        return 0;
    }
    return counter_of_slot(data, 1) > counter_of_slot(data, 0) ? 1 : 0;
}

static const uint8_t *sector_of_section(const uint8_t *data, size_t slot, size_t section_id) {
    for (size_t position = 0; position < SECTORS_PER_SLOT; ++position) {
        const uint8_t *sector = &data[slot * SLOT_SIZE + position * SECTOR_SIZE];
        size_t sector_section_id = sector[SECTION_ID_OFFSET] | sector[SECTION_ID_OFFSET + 1] << 8;
        if (sector_section_id == section_id) {
            return sector;
        }
    }
    return nullptr;
}

// An unedited write moves every section to the other slot, changing only the counter.
static bool check_unedited_write(const spec_gba_save_t *save) {
    memcpy(written, original, sizeof written);
    spec_error_t error = spec_gba_write_save(save, written);
    if (error != SPEC_OK) {
        printf("write_save: %s\n", spec_last_error().message);
        return false;
    }
    size_t old_slot = newer_slot(original);
    size_t new_slot = 1 - old_slot;
    for (size_t section_id = 0; section_id < SECTORS_PER_SLOT; ++section_id) {
        const uint8_t *old_sector = sector_of_section(original, old_slot, section_id);
        const uint8_t *new_sector = sector_of_section(written, new_slot, section_id);
        if (new_sector == nullptr || memcmp(old_sector, new_sector, COUNTER_OFFSET) != 0) {
            printf("section %zu changed\n", section_id);
            return false;
        }
    }
    bool is_old_slot_kept =
        memcmp(&original[old_slot * SLOT_SIZE], &written[old_slot * SLOT_SIZE], SLOT_SIZE) == 0;
    bool is_rest_kept = memcmp(&original[2 * SLOT_SIZE], &written[2 * SLOT_SIZE],
                               SPEC_GBA_SAVE_SIZE - 2 * SLOT_SIZE)
                        == 0;
    bool is_counter_next =
        counter_of_slot(written, new_slot) == counter_of_slot(original, old_slot) + 1;
    if (!is_old_slot_kept || !is_rest_kept || !is_counter_next) {
        printf("old slot kept %d, rest kept %d, counter next %d\n", is_old_slot_kept, is_rest_kept,
               is_counter_next);
        return false;
    }
    static spec_gba_save_t reread;
    return spec_gba_read_save(&reread, written) == SPEC_OK && reread.type == save->type
           && reread.language == save->language && reread.party_count == save->party_count;
}

static bool is_failed_write_reported(const spec_gba_save_t *broken, spec_error_t expected_error,
                                     spec_error_location_t location, uint32_t index0,
                                     uint32_t index1) {
    memcpy(written, original, sizeof written);
    spec_error_t error = spec_gba_write_save(broken, written);
    spec_error_data_t detail = spec_last_error();
    bool is_reported = error == expected_error && detail.error == error
                       && detail.location == location && detail.index0 == index0
                       && detail.index1 == index1;
    bool is_unchanged = memcmp(original, written, sizeof written) == 0;
    if (!is_reported || !is_unchanged) {
        printf("failed write: %s, reported %d, unchanged %d\n", detail.message, is_reported,
               is_unchanged);
        return false;
    }
    return true;
}

static bool check_failed_writes(const spec_gba_save_t *save) {
    static spec_gba_save_t broken;
    broken = *save;
    broken.boxes[3][12].origin.met_level = 200;
    if (!is_failed_write_reported(&broken, SPEC_ERROR_VALUE_OUT_OF_RANGE, SPEC_ERROR_LOCATION_BOX,
                                  3, 12)) {
        return false;
    }
    broken = *save;
    broken.items[SPEC_GBA_POCKET_KEY_ITEMS][0] = (spec_gba_item_slot_t){.item = 4, .quantity = 1};
    return is_failed_write_reported(&broken, SPEC_ERROR_WRONG_POCKET, SPEC_ERROR_LOCATION_ITEMS,
                                    SPEC_GBA_POCKET_KEY_ITEMS, 0);
}

int main(int argument_count, char **arguments) {
    if (argument_count != 2 || !read_file(original, arguments[1])) {
        printf("usage: gba_round_trip SAVE (at least %zu bytes)\n", SPEC_GBA_SAVE_SIZE);
        return 1;
    }
    static spec_gba_save_t save;
    if (spec_gba_read_save(&save, original) != SPEC_OK) {
        printf("read_save: %s\n", spec_last_error().message);
        return 1;
    }
    if (!check_unedited_write(&save) || !check_failed_writes(&save)) {
        return 1;
    }
    printf("type %d, language %d, party %d: ok\n", save.type, save.language, save.party_count);
    return 0;
}
