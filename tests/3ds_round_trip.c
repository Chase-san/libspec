// Writes a Gen 6 or 7 save back unedited and checks nothing changes, then checks an edit and
// refused writes. Gen 7 saves are written only with the games' signing key.

#include <stdio.h>
#include <string.h>

#include "3ds/3ds.h"

static uint8_t original[SPEC_3DS_SAVE_MAX_SIZE];
static uint8_t written[SPEC_3DS_SAVE_MAX_SIZE];
static size_t save_size;
static spec_3ds_signing_key_t signing_key;
static const spec_3ds_signing_key_t *signing_key_given;

static bool read_save_file(const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == nullptr) {
        return false;
    }
    save_size = fread(original, 1, sizeof original, file);
    fclose(file);
    return save_size != 0;
}

// The key file holds the private exponent alone, big-endian.
static bool read_key_file(const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == nullptr) {
        return false;
    }
    size_t size = fread(signing_key.private_exponent, 1, SPEC_3DS_SIGNING_KEY_SIZE, file);
    fclose(file);
    signing_key_given = &signing_key;
    return size == SPEC_3DS_SIGNING_KEY_SIZE;
}

static bool is_gen7(const spec_3ds_save_t *save) {
    return save->type == SPEC_GAME_TYPE_SUN_MOON
           || save->type == SPEC_GAME_TYPE_ULTRA_SUN_ULTRA_MOON;
}

static size_t count_differences(const uint8_t *first, const uint8_t *second, size_t size) {
    size_t difference_count = 0;
    for (size_t index = 0; index < size; ++index) {
        difference_count += first[index] != second[index];
    }
    return difference_count;
}

// Without the key, a Gen 7 write must refuse and change nothing.
static bool check_unedited_write(const spec_3ds_save_t *save) {
    memcpy(written, original, save_size);
    spec_error_t error = spec_3ds_write_save(save, written, save_size, signing_key_given);
    if (is_gen7(save) && signing_key_given == nullptr) {
        bool is_refused =
            error == SPEC_ERROR_VALUE_OUT_OF_RANGE && memcmp(original, written, save_size) == 0;
        printf("no signing key: write %s\n", is_refused ? "refused" : "NOT refused");
        return is_refused;
    }
    if (error != SPEC_OK) {
        printf("write_save: %s\n", spec_last_error().message);
        return false;
    }
    size_t difference_count = count_differences(original, written, save_size);
    if (difference_count != 0) {
        printf("%zu bytes changed\n", difference_count);
        for (size_t index = 0; index < save_size; ++index) {
            if (original[index] != written[index]) {
                printf("first at 0x%zX\n", index);
                break;
            }
        }
        return false;
    }
    return true;
}

static bool check_edit(const spec_3ds_save_t *save) {
    if (is_gen7(save) && signing_key_given == nullptr) {
        return true;
    }
    static spec_3ds_save_t edited;
    static spec_3ds_save_t reread;
    edited = *save;
    edited.money = 123'456;
    edited.boxes[1].name[0] = u'Z';
    memcpy(written, original, save_size);
    if (spec_3ds_write_save(&edited, written, save_size, signing_key_given) != SPEC_OK) {
        printf("edit: %s\n", spec_last_error().message);
        return false;
    }
    if (spec_3ds_read_save(&reread, written, save_size) != SPEC_OK || reread.money != 123'456
        || reread.boxes[1].name[0] != u'Z') {
        printf("edit not read back\n");
        return false;
    }
    return true;
}

static bool is_failed_write_reported(const spec_3ds_save_t *broken, spec_error_t expected_error,
                                     spec_error_location_t location, uint32_t index0,
                                     uint32_t index1) {
    memcpy(written, original, save_size);
    spec_error_t error = spec_3ds_write_save(broken, written, save_size, signing_key_given);
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

static bool check_failed_writes(const spec_3ds_save_t *save) {
    static spec_3ds_save_t broken;
    if (save->party[0].species != 0) {
        broken = *save;
        broken.party[0].origin.met_level = 200;
        if (!is_failed_write_reported(&broken, SPEC_ERROR_VALUE_OUT_OF_RANGE,
                                      SPEC_ERROR_LOCATION_PARTY, 0, 0)) {
            return false;
        }
    }
    broken = *save;
    // Item 1 is the Master Ball, which no game keeps with its key items.
    broken.items[SPEC_3DS_POCKET_KEY_ITEMS][0] = (spec_3ds_item_slot_t){.item = 1, .quantity = 1};
    if (!is_failed_write_reported(&broken, SPEC_ERROR_WRONG_POCKET, SPEC_ERROR_LOCATION_ITEMS,
                                  SPEC_3DS_POCKET_KEY_ITEMS, 0)) {
        return false;
    }
    broken = *save;
    broken.pokedex.seen_looks[25] = 1 << 4;
    return is_failed_write_reported(&broken, SPEC_ERROR_VALUE_OUT_OF_RANGE,
                                    SPEC_ERROR_LOCATION_POKEDEX, 25, 0);
}

int main(int argument_count, char **arguments) {
    bool has_arguments = argument_count == 2 || argument_count == 3;
    if (!has_arguments || !read_save_file(arguments[1])
        || (argument_count == 3 && !read_key_file(arguments[2]))) {
        printf("usage: 3ds_round_trip SAVE [SIGNING_KEY]\n");
        return 1;
    }
    static spec_3ds_save_t save;
    if (spec_3ds_read_save(&save, original, save_size) != SPEC_OK) {
        printf("read_save: %s\n", spec_last_error().message);
        return 1;
    }
    if (!check_unedited_write(&save) || !check_edit(&save) || !check_failed_writes(&save)) {
        return 1;
    }
    printf("type %d, version %d, language %d, party %d: ok\n", save.type, save.version,
           save.language, save.party_count);
    return 0;
}
