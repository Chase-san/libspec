// Reading and writing a whole Gen 3 save: detection, then each part's codec in turn.

#include "gba/gba.h"
#include "gba/gba_internal.h"
#include "spec_internal.h"

// Smallest first: a save also validates under a larger game's section sizes.
constexpr spec_game_type_t TYPES_BY_SECTION_SIZE[] = {
    SPEC_GAME_TYPE_RUBY_SAPPHIRE,
    SPEC_GAME_TYPE_FIRERED_LEAFGREEN,
    SPEC_GAME_TYPE_EMERALD,
};

static bool is_gen3_language(spec_language_t language) {
    return (language >= SPEC_LANGUAGE_JAPANESE && language <= SPEC_LANGUAGE_GERMAN)
           || language == SPEC_LANGUAGE_SPANISH;
}

static void count_language_vote(size_t votes[], const spec_gba_pokemon_t *pokemon,
                                const spec_gba_trainer_t *player) {
    bool is_players_own =
        pokemon->trainer.id == player->id && pokemon->trainer.secret_id == player->secret_id;
    bool is_hatched_species = spec_gba_species_to_national(pokemon->species) != 0
                              && !pokemon->is_egg && !pokemon->is_bad_egg;
    if (is_players_own && is_hatched_species && is_gen3_language(pokemon->language)) {
        votes[pokemon->language]++;
    }
}

// The save has no language, so the player's own Pokémon vote.
static spec_language_t detect_language(const spec_gba_save_t *save) {
    size_t votes[SPEC_LANGUAGE_SPANISH + 1] = {};
    for (size_t index = 0; index < save->party_count && index < SPEC_GBA_PARTY_CAPACITY; ++index) {
        count_language_vote(votes, &save->party[index], &save->trainer);
    }
    for (size_t box = 0; box < SPEC_GBA_BOX_COUNT; ++box) {
        for (size_t index = 0; index < SPEC_GBA_BOX_CAPACITY; ++index) {
            count_language_vote(votes, &save->boxes[box].pokemon[index], &save->trainer);
        }
    }
    spec_language_t leader = SPEC_LANGUAGE_UNKNOWN;
    bool is_tied = false;
    for (size_t language = 1; language <= SPEC_LANGUAGE_SPANISH; ++language) {
        if (votes[language] > votes[leader]) {
            leader = (spec_language_t)language;
            is_tied = false;
        } else if (votes[language] != 0 && votes[language] == votes[leader]) {
            is_tied = true;
        }
    }
    return is_tied ? SPEC_LANGUAGE_UNKNOWN : leader;
}

static spec_error_t check_save(const spec_gba_save_t *save, const spec_gba_layout_t *layout) {
    spec_error_t error = spec_gba_check_player(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    error = spec_gba_check_storage(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    return spec_gba_check_items(save, layout);
}

spec_error_t spec_gba_read_save(spec_gba_save_t *save,
                                const uint8_t data[static SPEC_GBA_SAVE_SIZE]) {
    size_t type_count = sizeof TYPES_BY_SECTION_SIZE / sizeof TYPES_BY_SECTION_SIZE[0];
    for (size_t index = 0; index < type_count; ++index) {
        const spec_gba_layout_t *layout = spec_gba_get_layout(TYPES_BY_SECTION_SIZE[index]);
        spec_gba_slot_t active;
        if (!spec_gba_find_active_slot(&active, data, layout->section_sizes)) {
            continue;
        }
        *save = (spec_gba_save_t){.type = layout->type};
        spec_gba_decode_player(save, data, &active, layout);
        spec_gba_decode_pokedex(&save->pokedex, data, &active, layout);
        spec_gba_decode_storage(save, data, &active, layout);
        spec_gba_decode_items(save, data, &active, layout);
        save->language = detect_language(save);
        return SPEC_OK;
    }
    return spec_fail(SPEC_ERROR_INVALID_SAVE, "no save slot is valid for any Gen 3 game");
}

// Everything is checked before the save changes, so a failure changes nothing.
spec_error_t spec_gba_write_save(const spec_gba_save_t *save,
                                 uint8_t data[static SPEC_GBA_SAVE_SIZE]) {
    const spec_gba_layout_t *layout = spec_gba_get_layout(save->type);
    if (layout == nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "type is not a GBA game type");
    }
    spec_gba_slot_t active;
    if (!spec_gba_find_active_slot(&active, data, layout->section_sizes)) {
        return spec_fail(SPEC_ERROR_INVALID_SAVE, "no save slot is valid for the save's type");
    }
    spec_error_t error = check_save(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    spec_gba_slot_t next = spec_gba_copy_to_next_slot(data, &active);
    spec_gba_encode_player(data, &next, layout, save);
    spec_gba_encode_pokedex(data, &next, layout, &save->pokedex);
    spec_gba_encode_storage(data, &next, layout, save);
    spec_gba_encode_items(data, &next, layout, save);
    spec_gba_stamp_slot(data, &next, layout->section_sizes);
    return SPEC_OK;
}
