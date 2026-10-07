// Writes a verified Gen 1 save back unedited and checks nothing changes, then checks edits and
// refused writes and identification.

#include <stdio.h>
#include <string.h>

#include "gb/gb.h"
#include "gbc/gbc.h"

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
    broken.boxes[3].count = 1;
    broken.boxes[3].pokemon[0].types[0] = SPEC_TYPE_DARK;
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

// English, Italian and Spanish share species names, so an international save's language may be
// UNKNOWN.
static bool is_identity_of(const spec_gb_identity_t *identity, const spec_gb_save_t *save) {
    bool is_language_unsettled =
        save->language != SPEC_LANGUAGE_JAPANESE && identity->language == SPEC_LANGUAGE_UNKNOWN;
    return identity->type == save->type
           && (identity->language == save->language || is_language_unsettled);
}

static bool check_identify(const spec_gb_save_t *save) {
    spec_gb_identity_t identities[SPEC_GB_IDENTITY_MAX_COUNT];
    size_t identity_count = spec_gb_identify_save(identities, original);
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
    spec_gbc_identity_t gbc_identities[SPEC_GBC_IDENTITY_MAX_COUNT];
    if (spec_gbc_identify_save(gbc_identities, original, sizeof original) != 0) {
        printf("gbc_identify_save: a Gen 1 save fits Gen 2\n");
        return false;
    }
    return true;
}

static bool is_players_own(const spec_gb_pokemon_t *pokemon, const spec_gb_save_t *save) {
    char8_t trainer_name[SPEC_GB_TEXT_BUFFER_SIZE];
    char8_t player_name[SPEC_GB_TEXT_BUFFER_SIZE];
    bool are_names_read =
        spec_gb_text_to_utf8(trainer_name, pokemon->trainer.name, SPEC_GB_NAME_SIZE, save->language)
            == SPEC_OK
        && spec_gb_text_to_utf8(player_name, save->trainer.name, SPEC_GB_NAME_SIZE, save->language)
               == SPEC_OK;
    return are_names_read && pokemon->trainer.id == save->trainer.id
           && strcmp((const char *)trainer_name, (const char *)player_name) == 0;
}

// True when the French name differs from the English one, and so can settle the vote.
static bool rename_in_french(spec_gb_pokemon_t *pokemon, const spec_gb_save_t *save) {
    spec_gb_pokemon_t english = *pokemon;
    if (!is_players_own(pokemon, save)
        || spec_gb_pokemon_remove_nickname(&english, SPEC_LANGUAGE_ENGLISH) != SPEC_OK
        || spec_gb_pokemon_remove_nickname(pokemon, SPEC_LANGUAGE_FRENCH) != SPEC_OK) {
        return false;
    }
    return memcmp(english.nickname, pokemon->nickname, SPEC_GB_NAME_SIZE) != 0;
}

// The player's own Pokémon under their French species names make the save French.
static bool check_language_vote(const spec_gb_save_t *save) {
    if (save->language == SPEC_LANGUAGE_JAPANESE) {
        return true;
    }
    static spec_gb_save_t edited;
    edited = *save;
    edited.language = SPEC_LANGUAGE_FRENCH;
    bool has_french_name = false;
    for (size_t index = 0; index < edited.party_count; ++index) {
        if (rename_in_french(&edited.party[index], save)) {
            has_french_name = true;
        }
    }
    for (size_t box = 0; box < SPEC_GB_BOX_COUNT; ++box) {
        for (size_t index = 0; index < edited.boxes[box].count; ++index) {
            if (rename_in_french(&edited.boxes[box].pokemon[index], save)) {
                has_french_name = true;
            }
        }
    }
    memcpy(written, original, sizeof written);
    if (spec_gb_write_save(&edited, written) != SPEC_OK) {
        printf("French names: %s\n", spec_last_error().message);
        return false;
    }
    spec_gb_identity_t identities[SPEC_GB_IDENTITY_MAX_COUNT];
    size_t identity_count = spec_gb_identify_save(identities, written);
    spec_language_t expected = SPEC_LANGUAGE_UNKNOWN;
    if (has_french_name) {
        expected = SPEC_LANGUAGE_FRENCH;
    }
    if (identity_count == 0) {
        printf("French names: identified as no game\n");
        return false;
    }
    if (identities[0].language != expected) {
        printf("French names: identified as language %d, not %d\n", identities[0].language,
               expected);
        return false;
    }
    return true;
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
    if (!check_unedited_write(&save) || !check_box_edit(&save) || !check_failed_writes(&save)
        || !check_identify(&save) || !check_language_vote(&save)) {
        return 1;
    }
    printf("type %d, language %d, party %d: ok\n", save.type, save.language, save.party_count);
    return 0;
}
