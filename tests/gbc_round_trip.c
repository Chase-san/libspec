// Writes a verified Gen 2 save back unedited and checks nothing changes, then checks edits and
// refused writes.

#include <stdio.h>
#include <string.h>

#include "gbc/gbc.h"

static uint8_t original[SPEC_GBC_JAPANESE_CRYSTAL_SAVE_SIZE];
static uint8_t written[SPEC_GBC_JAPANESE_CRYSTAL_SAVE_SIZE];
static size_t save_size;

// Verified saves are named <device>-<game>[_<region>]-<date>-<note>.sav.
static spec_game_type_t type_of(const char *path) {
    return strstr(path, "-crystal") != nullptr ? SPEC_GAME_TYPE_CRYSTAL
                                               : SPEC_GAME_TYPE_GOLD_SILVER;
}

static spec_language_t language_of(const char *path) {
    return strstr(path, "_jp-") != nullptr ? SPEC_LANGUAGE_JAPANESE : SPEC_LANGUAGE_ENGLISH;
}

static bool read_file(uint8_t *data, const char *path) {
    bool is_japanese_crystal =
        type_of(path) == SPEC_GAME_TYPE_CRYSTAL && language_of(path) == SPEC_LANGUAGE_JAPANESE;
    save_size = is_japanese_crystal ? SPEC_GBC_JAPANESE_CRYSTAL_SAVE_SIZE : SPEC_GBC_SAVE_SIZE;
    FILE *file = fopen(path, "rb");
    if (file == nullptr) {
        return false;
    }
    size_t size = fread(data, 1, save_size, file);
    fclose(file);
    return size == save_size;
}

static bool check_unedited_write(const spec_gbc_save_t *save) {
    memcpy(written, original, save_size);
    if (spec_gbc_write_save(save, written, save_size) != SPEC_OK) {
        printf("write_save: %s\n", spec_last_error().message);
        return false;
    }
    for (size_t offset = 0; offset < save_size; ++offset) {
        if (written[offset] != original[offset]) {
            printf("byte 0x%zX changed\n", offset);
            return false;
        }
    }
    return true;
}

// The edit must survive the backup too, as loading falls back to it.
static bool check_edit(const spec_gbc_save_t *save) {
    static spec_gbc_save_t edited;
    static spec_gbc_save_t reread;
    edited = *save;
    edited.money = 123'456;
    edited.boxes[2].name[0] = edited.boxes[1].name[0];
    memcpy(written, original, save_size);
    if (spec_gbc_write_save(&edited, written, save_size) != SPEC_OK) {
        printf("edit: %s\n", spec_last_error().message);
        return false;
    }
    // Breaking the primary's checksum makes reading take the backup.
    written[0x2009] ^= 0xFF;
    if (spec_gbc_read_save(&reread, written, save_size, edited.type, edited.language) != SPEC_OK
        || reread.money != edited.money || reread.boxes[2].name[0] != edited.boxes[2].name[0]) {
        printf("edit not read back from the backup\n");
        return false;
    }
    return true;
}

static bool is_failed_write_reported(const spec_gbc_save_t *broken, spec_error_t expected_error,
                                     spec_error_location_t location, uint32_t index0,
                                     uint32_t index1) {
    memcpy(written, original, save_size);
    spec_error_t error = spec_gbc_write_save(broken, written, save_size);
    spec_error_data_t detail = spec_last_error();
    bool is_reported = error == expected_error && detail.error == error
                       && detail.location == location && detail.index0 == index0
                       && detail.index1 == index1;
    bool is_unchanged = memcmp(original, written, save_size) == 0;
    if (!is_reported || !is_unchanged) {
        printf("failed write: %s, reported %d, unchanged %d\n", detail.message, is_reported,
               is_unchanged);
        return false;
    }
    return true;
}

static bool check_failed_writes(const spec_gbc_save_t *save) {
    static spec_gbc_save_t broken;
    broken = *save;
    broken.boxes[3].count = 1;
    broken.boxes[3].pokemon[0].dvs[SPEC_GB_STAT_ATTACK] = 16;
    if (!is_failed_write_reported(&broken, SPEC_ERROR_VALUE_OUT_OF_RANGE, SPEC_ERROR_LOCATION_BOX,
                                  3, 0)) {
        return false;
    }
    broken = *save;
    // Item 7 is the Bicycle, a key item.
    broken.items[SPEC_GBC_POCKET_ITEMS][0] = (spec_gbc_item_slot_t){.item = 7, .quantity = 1};
    return is_failed_write_reported(&broken, SPEC_ERROR_WRONG_POCKET, SPEC_ERROR_LOCATION_ITEMS,
                                    SPEC_GBC_POCKET_ITEMS, 0);
}

int main(int argument_count, char **arguments) {
    if (argument_count != 2 || !read_file(original, arguments[1])) {
        printf("usage: gbc_round_trip SAVE\n");
        return 1;
    }
    static spec_gbc_save_t save;
    if (spec_gbc_read_save(&save, original, save_size, type_of(arguments[1]),
                           language_of(arguments[1]))
        != SPEC_OK) {
        printf("read_save: %s\n", spec_last_error().message);
        return 1;
    }
    if (!check_unedited_write(&save) || !check_edit(&save) || !check_failed_writes(&save)) {
        return 1;
    }
    printf("type %d, language %d, party %d: ok\n", save.type, save.language, save.party_count);
    return 0;
}
