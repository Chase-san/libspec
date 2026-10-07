// Writes a verified Gen 2 save back unedited and checks nothing changes, then checks edits and
// refused writes and identification.

#include <stdio.h>
#include <string.h>

#include "gb/gb.h"
#include "gbc/gbc.h"
#include "gbc/gbc_internal.h"

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

// The game saves its backup a few frames after the primary, so the two copies' play-time frames
// can disagree; writing copies the primary's, and the backup's checksum is then the primary's.
static size_t primary_offset_of(size_t offset, const spec_gbc_layout_t *layout) {
    if (offset >= layout->backup_checksum_offset && offset < layout->backup_checksum_offset + 2) {
        return layout->checksum_offset + (offset - layout->backup_checksum_offset);
    }
    for (size_t index = 0; index < layout->backup_chunk_count; ++index) {
        const spec_gbc_backup_chunk_t *chunk = &layout->backup_chunks[index];
        if (offset >= chunk->backup_offset && offset < chunk->backup_offset + chunk->size) {
            return chunk->primary_offset + (offset - chunk->backup_offset);
        }
    }
    return offset;
}

static bool check_unedited_write(const spec_gbc_save_t *save) {
    memcpy(written, original, save_size);
    if (spec_gbc_write_save(save, written, save_size) != SPEC_OK) {
        printf("write_save: %s\n", spec_last_error().message);
        return false;
    }
    const spec_gbc_layout_t *layout = spec_gbc_get_layout(save->type, save->language);
    for (size_t offset = 0; offset < save_size; ++offset) {
        uint8_t primary_value = original[primary_offset_of(offset, layout)];
        if (written[offset] != original[offset] && written[offset] != primary_value) {
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

// Read from the backup, every field must come from where that copy keeps it, so junk in the
// primary changes nothing read.
static bool check_backup_read(const spec_gbc_save_t *save) {
    static spec_gbc_save_t reread;
    static uint8_t rewritten[SPEC_GBC_JAPANESE_CRYSTAL_SAVE_SIZE];
    memcpy(written, original, save_size);
    if (spec_gbc_write_save(save, written, save_size) != SPEC_OK) {
        printf("backup read: %s\n", spec_last_error().message);
        return false;
    }
    const spec_gbc_layout_t *layout = spec_gbc_get_layout(save->type, save->language);
    memset(&written[SPEC_GBC_GAME_DATA_OFFSET], 0x5A, layout->game_data_size);
    if (spec_gbc_read_save(&reread, written, save_size, save->type, save->language) != SPEC_OK) {
        printf("backup read: %s\n", spec_last_error().message);
        return false;
    }
    memcpy(written, original, save_size);
    memcpy(rewritten, original, save_size);
    if (spec_gbc_write_save(save, written, save_size) != SPEC_OK
        || spec_gbc_write_save(&reread, rewritten, save_size) != SPEC_OK) {
        printf("backup read: %s\n", spec_last_error().message);
        return false;
    }
    for (size_t offset = 0; offset < save_size; ++offset) {
        if (written[offset] != rewritten[offset]) {
            printf("backup read: byte 0x%zX differs\n", offset);
            return false;
        }
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

// Emulators and cart dumpers keep the cartridge clock after the save, which reading and writing
// leave alone.
static bool check_clock_data(const spec_gbc_save_t *save) {
    constexpr size_t CLOCK_DATA_SIZE = 48;
    constexpr uint8_t CLOCK_BYTE = 0xA5;
    static uint8_t with_clock[SPEC_GBC_JAPANESE_CRYSTAL_SAVE_SIZE + CLOCK_DATA_SIZE];
    static spec_gbc_save_t reread;
    memcpy(with_clock, original, save_size);
    memset(&with_clock[save_size], CLOCK_BYTE, CLOCK_DATA_SIZE);
    if (spec_gbc_read_save(&reread, with_clock, save_size + CLOCK_DATA_SIZE, save->type,
                           save->language)
            != SPEC_OK
        || spec_gbc_write_save(save, with_clock, save_size + CLOCK_DATA_SIZE) != SPEC_OK) {
        printf("clock data: %s\n", spec_last_error().message);
        return false;
    }
    for (size_t index = 0; index < CLOCK_DATA_SIZE; ++index) {
        if (with_clock[save_size + index] != CLOCK_BYTE) {
            printf("clock data: byte %zu changed\n", index);
            return false;
        }
    }
    return true;
}

// English, Italian and Spanish share species names, so an international save's language may be
// UNKNOWN.
static bool is_identity_of(const spec_gbc_identity_t *identity, const spec_gbc_save_t *save) {
    bool is_language_unsettled =
        save->language != SPEC_LANGUAGE_JAPANESE && identity->language == SPEC_LANGUAGE_UNKNOWN;
    return identity->type == save->type
           && (identity->language == save->language || is_language_unsettled);
}

static bool check_identify(const spec_gbc_save_t *save) {
    spec_gbc_identity_t identities[SPEC_GBC_IDENTITY_MAX_COUNT];
    size_t identity_count = spec_gbc_identify_save(identities, original, save_size);
    bool is_found = false;
    for (size_t index = 0; index < identity_count; ++index) {
        if (is_identity_of(&identities[index], save)) {
            is_found = true;
        }
    }
    if (!is_found) {
        printf("identify_save: the save's game and language are not among %zu\n", identity_count);
        return false;
    }
    spec_gb_identity_t gb_identities[SPEC_GB_IDENTITY_MAX_COUNT];
    if (save_size == SPEC_GB_SAVE_SIZE && spec_gb_identify_save(gb_identities, original) != 0) {
        printf("gb_identify_save: a Gen 2 save fits Gen 1\n");
        return false;
    }
    return true;
}

static bool is_players_own(const spec_gbc_pokemon_t *pokemon, const spec_gbc_save_t *save) {
    char8_t trainer_name[SPEC_GBC_TEXT_BUFFER_SIZE];
    char8_t player_name[SPEC_GBC_TEXT_BUFFER_SIZE];
    bool are_names_read = spec_gbc_text_to_utf8(trainer_name, pokemon->trainer.name,
                                                SPEC_GBC_NAME_SIZE, save->language)
                              == SPEC_OK
                          && spec_gbc_text_to_utf8(player_name, save->trainer.name,
                                                   SPEC_GBC_NAME_SIZE, save->language)
                                 == SPEC_OK;
    return are_names_read && pokemon->trainer.id == save->trainer.id
           && strcmp((const char *)trainer_name, (const char *)player_name) == 0;
}

// True when the German name differs from the English one, and so can settle the vote.
static bool rename_in_german(spec_gbc_pokemon_t *pokemon, const spec_gbc_save_t *save) {
    spec_gbc_pokemon_t english = *pokemon;
    if (pokemon->is_egg || !is_players_own(pokemon, save)
        || spec_gbc_pokemon_remove_nickname(&english, SPEC_LANGUAGE_ENGLISH) != SPEC_OK
        || spec_gbc_pokemon_remove_nickname(pokemon, SPEC_LANGUAGE_GERMAN) != SPEC_OK) {
        return false;
    }
    return memcmp(english.nickname, pokemon->nickname, SPEC_GBC_NAME_SIZE) != 0;
}

// The player's own Pokémon under their German species names make the save German.
static bool check_language_vote(const spec_gbc_save_t *save) {
    if (save->language == SPEC_LANGUAGE_JAPANESE) {
        return true;
    }
    static spec_gbc_save_t edited;
    edited = *save;
    edited.language = SPEC_LANGUAGE_GERMAN;
    bool has_german_name = false;
    for (size_t index = 0; index < edited.party_count; ++index) {
        if (rename_in_german(&edited.party[index], save)) {
            has_german_name = true;
        }
    }
    for (size_t box = 0; box < SPEC_GBC_BOX_COUNT; ++box) {
        for (size_t index = 0; index < edited.boxes[box].count; ++index) {
            if (rename_in_german(&edited.boxes[box].pokemon[index], save)) {
                has_german_name = true;
            }
        }
    }
    memcpy(written, original, save_size);
    if (spec_gbc_write_save(&edited, written, save_size) != SPEC_OK) {
        printf("German names: %s\n", spec_last_error().message);
        return false;
    }
    spec_gbc_identity_t identities[SPEC_GBC_IDENTITY_MAX_COUNT];
    size_t identity_count = spec_gbc_identify_save(identities, written, save_size);
    spec_language_t expected = SPEC_LANGUAGE_UNKNOWN;
    if (has_german_name) {
        expected = SPEC_LANGUAGE_GERMAN;
    }
    if (identity_count == 0) {
        printf("German names: identified as no game\n");
        return false;
    }
    if (identities[0].language != expected) {
        printf("German names: identified as language %d, not %d\n", identities[0].language,
               expected);
        return false;
    }
    return true;
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
    if (!check_unedited_write(&save) || !check_edit(&save) || !check_backup_read(&save)
        || !check_failed_writes(&save) || !check_clock_data(&save) || !check_identify(&save)
        || !check_language_vote(&save)) {
        return 1;
    }
    printf("type %d, language %d, party %d: ok\n", save.type, save.language, save.party_count);
    return 0;
}
