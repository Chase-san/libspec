// Writes a verified Gen 1 save back unedited and checks nothing changes, then checks edits and
// refused writes.

#include <stdio.h>
#include <string.h>

#include "gb/gb.h"

static uint8_t original[SPEC_GB_SAVE_SIZE];
static uint8_t written[SPEC_GB_SAVE_SIZE];

static bool read_file(uint8_t *data, const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == nullptr) {
        return false;
    }
    size_t size = fread(data, 1, SPEC_GB_SAVE_SIZE, file);
    fclose(file);
    return size == SPEC_GB_SAVE_SIZE;
}

// Verified saves are named <device>-<game>[_<region>]-<date>-<note>.sav.
static spec_game_type_t type_of(const char *path) {
    return strstr(path, "-yellow") != nullptr ? SPEC_GAME_TYPE_YELLOW : SPEC_GAME_TYPE_RED_BLUE;
}

static spec_language_t language_of(const char *path) {
    return strstr(path, "_jp-") != nullptr ? SPEC_LANGUAGE_JAPANESE : SPEC_LANGUAGE_ENGLISH;
}

static bool check_unedited_write(const spec_gb_save_t *save) {
    memcpy(written, original, sizeof written);
    if (spec_gb_write_save(save, written) != SPEC_OK) {
        printf("write_save: %s\n", spec_last_error().message);
        return false;
    }
    for (size_t offset = 0; offset < sizeof written; ++offset) {
        if (written[offset] != original[offset]) {
            printf("byte 0x%zX changed\n", offset);
            return false;
        }
    }
    return true;
}

// Storing a party Pokémon, under its species name, in another box makes the game's first box
// change if the player has made none.
static bool check_box_edit(const spec_gb_save_t *save) {
    static spec_gb_save_t edited;
    static spec_gb_save_t reread;
    edited = *save;
    size_t box = edited.current_box == 5 ? 6 : 5;
    if (edited.party_count == 0 || edited.boxes[box].count != 0) {
        return true;
    }
    edited.boxes[box].pokemon[0] = edited.party[0];
    edited.boxes[box].pokemon[0].party_data = (spec_gb_party_data_t){};
    edited.boxes[box].count = 1;
    if (spec_gb_pokemon_remove_nickname(&edited.boxes[box].pokemon[0], edited.language)
        != SPEC_OK) {
        printf("remove_nickname: %s\n", spec_last_error().message);
        return false;
    }
    memcpy(written, original, sizeof written);
    if (spec_gb_write_save(&edited, written) != SPEC_OK
        || spec_gb_read_save(&reread, written, edited.type, edited.language) != SPEC_OK) {
        printf("box edit: %s\n", spec_last_error().message);
        return false;
    }
    const spec_gb_pokemon_t *stored = &reread.boxes[box].pokemon[0];
    bool is_stored =
        reread.boxes[box].count == 1 && stored->species == edited.party[0].species
        && memcmp(stored->nickname, edited.boxes[box].pokemon[0].nickname, SPEC_GB_NAME_SIZE) == 0;
    if (!is_stored || reread.party_count != save->party_count) {
        printf("box edit not read back\n");
        return false;
    }
    return true;
}

static bool is_failed_write_reported(const spec_gb_save_t *broken, spec_error_t expected_error,
                                     spec_error_location_t location, uint32_t index0,
                                     uint32_t index1) {
    memcpy(written, original, sizeof written);
    spec_error_t error = spec_gb_write_save(broken, written);
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

static bool check_failed_writes(const spec_gb_save_t *save) {
    static spec_gb_save_t broken;
    broken = *save;
    broken.boxes[3].count = 1;
    broken.boxes[3].pokemon[0].dvs[SPEC_GB_STAT_ATTACK] = 16;
    if (!is_failed_write_reported(&broken, SPEC_ERROR_VALUE_OUT_OF_RANGE, SPEC_ERROR_LOCATION_BOX,
                                  3, 0)) {
        return false;
    }
    broken = *save;
    // Item 0x15 is the Boulder Badge, which no pocket holds.
    broken.items[SPEC_GB_POCKET_BAG][0] = (spec_gb_item_slot_t){.item = 0x15, .quantity = 1};
    return is_failed_write_reported(&broken, SPEC_ERROR_INVALID_ITEM, SPEC_ERROR_LOCATION_ITEMS,
                                    SPEC_GB_POCKET_BAG, 0);
}

int main(int argument_count, char **arguments) {
    if (argument_count != 2 || !read_file(original, arguments[1])) {
        printf("usage: gb_round_trip SAVE (%zu bytes)\n", SPEC_GB_SAVE_SIZE);
        return 1;
    }
    static spec_gb_save_t save;
    if (spec_gb_read_save(&save, original, type_of(arguments[1]), language_of(arguments[1]))
        != SPEC_OK) {
        printf("read_save: %s\n", spec_last_error().message);
        return 1;
    }
    if (!check_unedited_write(&save) || !check_box_edit(&save) || !check_failed_writes(&save)) {
        return 1;
    }
    printf("type %d, language %d, party %d: ok\n", save.type, save.language, save.party_count);
    return 0;
}
