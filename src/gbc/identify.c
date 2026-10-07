// Telling which game and language made a Gen 2 save, which records neither.

#include "gb/gb_internal.h"
#include "gbc/gbc.h"
#include "gbc/gbc_internal.h"
#include "spec_internal.h"

struct layout_key {
    spec_game_type_t type;
    spec_language_t language;
};
typedef struct layout_key layout_key_t;

constexpr layout_key_t LAYOUT_KEYS[] = {
    {SPEC_GAME_TYPE_GOLD_SILVER, SPEC_LANGUAGE_JAPANESE},
    {SPEC_GAME_TYPE_CRYSTAL, SPEC_LANGUAGE_JAPANESE},
    {SPEC_GAME_TYPE_GOLD_SILVER, SPEC_LANGUAGE_ENGLISH},
    {SPEC_GAME_TYPE_CRYSTAL, SPEC_LANGUAGE_ENGLISH},
};

// A checksum also matches by chance, so the lists must make sense too.
static bool are_lists_sound(const spec_gbc_save_t *save, const spec_gbc_layout_t *layout) {
    if (save->party_count > SPEC_GBC_PARTY_CAPACITY) {
        return false;
    }
    for (size_t box = 0; box < layout->box_count; ++box) {
        if (save->boxes[box].count > layout->box_shape.capacity) {
            return false;
        }
    }
    return true;
}

static void count_pokemon_vote(spec_gb_language_vote_t *vote, const spec_gbc_pokemon_t *pokemon,
                               const spec_gbc_trainer_t *player) {
    bool is_players_own =
        pokemon->trainer.id == player->id
        && spec_gb_is_same_text(pokemon->trainer.name, player->name, SPEC_GBC_NAME_SIZE);
    if (!is_players_own || pokemon->is_egg) {
        return;
    }
    for (size_t index = 0; index < SPEC_GB_VOTE_LANGUAGE_COUNT; ++index) {
        spec_language_t language = SPEC_GB_VOTE_LANGUAGES[index];
        const char8_t *species_name = spec_game_boy_species_name(pokemon->species, language);
        uint8_t written[SPEC_GBC_NAME_SIZE];
        if (species_name != nullptr
            && spec_gbc_text_from_utf8(written, sizeof written, species_name, language) == SPEC_OK
            && spec_gb_is_same_text(written, pokemon->nickname, sizeof written)) {
            vote->counts[index]++;
        }
    }
}

static spec_language_t elect_save_language(const spec_gbc_save_t *save,
                                           const spec_gbc_layout_t *layout) {
    spec_gb_language_vote_t vote = {};
    for (size_t index = 0; index < save->party_count; ++index) {
        count_pokemon_vote(&vote, &save->party[index], &save->trainer);
    }
    for (size_t box = 0; box < layout->box_count; ++box) {
        for (size_t index = 0; index < save->boxes[box].count; ++index) {
            count_pokemon_vote(&vote, &save->boxes[box].pokemon[index], &save->trainer);
        }
    }
    for (size_t index = 0; index < SPEC_GBC_DAYCARE_CAPACITY; ++index) {
        if (save->daycare.parents[index].species != 0) {
            count_pokemon_vote(&vote, &save->daycare.parents[index], &save->trainer);
        }
    }
    return spec_gb_elect_language(&vote);
}

size_t spec_gbc_identify_save(spec_gbc_identity_t identities[static SPEC_GBC_IDENTITY_MAX_COUNT],
                              const uint8_t *data, size_t data_size) {
    size_t identity_count = 0;
    for (size_t index = 0; index < sizeof LAYOUT_KEYS / sizeof LAYOUT_KEYS[0]; ++index) {
        layout_key_t key = LAYOUT_KEYS[index];
        const spec_gbc_layout_t *layout = spec_gbc_get_layout(key.type, key.language);
        spec_gbc_save_t save;
        if (spec_gbc_read_save(&save, data, data_size, key.type, key.language) != SPEC_OK
            || !are_lists_sound(&save, layout)) {
            continue;
        }
        spec_language_t language = SPEC_LANGUAGE_JAPANESE;
        if (key.language != SPEC_LANGUAGE_JAPANESE) {
            language = elect_save_language(&save, layout);
        }
        identities[identity_count++] = (spec_gbc_identity_t){key.type, language};
    }
    return identity_count;
}
