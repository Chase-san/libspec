// Telling which game and language made a Gen 1 save, which records neither.

#include "gb/gb.h"
#include "gb/gb_internal.h"
#include "spec_internal.h"

// National Dex numbers.
constexpr uint16_t BULBASAUR = 1;
constexpr uint16_t CHARMANDER = 4;
constexpr uint16_t SQUIRTLE = 7;
constexpr uint16_t PIKACHU = 25;

spec_language_t spec_gb_elect_language(const spec_gb_language_vote_t *vote) {
    size_t leader = 0;
    bool is_tied = false;
    for (size_t index = 1; index < SPEC_GB_VOTE_LANGUAGE_COUNT; ++index) {
        if (vote->counts[index] > vote->counts[leader]) {
            leader = index;
            is_tied = false;
        } else if (vote->counts[index] == vote->counts[leader]) {
            is_tied = true;
        }
    }
    if (is_tied || vote->counts[leader] == 0) {
        return SPEC_LANGUAGE_UNKNOWN;
    }
    return SPEC_GB_VOTE_LANGUAGES[leader];
}

bool spec_gb_is_same_text(const uint8_t *text, const uint8_t *other_text, size_t text_size) {
    for (size_t index = 0; index < text_size; ++index) {
        if (text[index] != other_text[index]) {
            return false;
        }
        if (text[index] == SPEC_GB_END_OF_TEXT) {
            return true;
        }
    }
    return true;
}

static bool is_list_terminated(const uint8_t *list, const spec_gb_list_shape_t *shape) {
    return list[0] <= shape->capacity
           && list[spec_gb_list_species_offset(shape, list[0])] == SPEC_GB_END_OF_LIST;
}

// A one-byte checksum also matches by chance, so the lists must make sense too.
static bool are_lists_sound(const uint8_t *data, const spec_gb_layout_t *layout) {
    if (!is_list_terminated(&data[layout->party_offset], &layout->party_shape)
        || !is_list_terminated(&data[layout->current_box_list_offset], &layout->box_shape)) {
        return false;
    }
    for (size_t box = 0; box < layout->box_count; ++box) {
        size_t list_offset = 0;
        if (spec_gb_find_box_list(&list_offset, data, layout, box)
            && data[list_offset] > layout->box_shape.capacity) {
            return false;
        }
    }
    return true;
}

static void count_pokemon_vote(spec_gb_language_vote_t *vote, const spec_gb_pokemon_t *pokemon,
                               const spec_gb_trainer_t *player) {
    bool is_players_own =
        pokemon->trainer.id == player->id
        && spec_gb_is_same_text(pokemon->trainer.name, player->name, SPEC_GB_NAME_SIZE);
    if (!is_players_own) {
        return;
    }
    uint16_t national_number = spec_gb_species_to_national(pokemon->species);
    for (size_t index = 0; index < SPEC_GB_VOTE_LANGUAGE_COUNT; ++index) {
        spec_language_t language = SPEC_GB_VOTE_LANGUAGES[index];
        const char8_t *species_name = spec_game_boy_species_name(national_number, language);
        uint8_t written[SPEC_GB_NAME_SIZE];
        if (species_name != nullptr
            && spec_gb_text_from_utf8(written, sizeof written, species_name, language) == SPEC_OK
            && spec_gb_is_same_text(written, pokemon->nickname, sizeof written)) {
            vote->counts[index]++;
        }
    }
}

// Decoding one Pokémon at a time keeps a whole save off the stack.
static void count_list_votes(spec_gb_language_vote_t *vote, const uint8_t *list,
                             const spec_gb_list_shape_t *shape, const spec_gb_trainer_t *player) {
    for (size_t index = 0; index < list[0]; ++index) {
        spec_gb_pokemon_t pokemon;
        spec_gb_decode_list_entry(&pokemon, list, shape, index);
        count_pokemon_vote(vote, &pokemon, player);
    }
}

static spec_language_t elect_save_language(const uint8_t *data, const spec_gb_layout_t *layout) {
    spec_gb_trainer_t player;
    spec_gb_decode_trainer(&player, data, layout);
    spec_gb_language_vote_t vote = {};
    count_list_votes(&vote, &data[layout->party_offset], &layout->party_shape, &player);
    for (size_t box = 0; box < layout->box_count; ++box) {
        size_t list_offset = 0;
        if (spec_gb_find_box_list(&list_offset, data, layout, box)) {
            count_list_votes(&vote, &data[list_offset], &layout->box_shape, &player);
        }
    }
    spec_gb_pokemon_t daycare;
    spec_gb_decode_daycare(&daycare, data, layout);
    if (daycare.species != 0) {
        count_pokemon_vote(&vote, &daycare, &player);
    }
    return spec_gb_elect_language(&vote);
}

static bool is_red_blue_starter(uint16_t national_number) {
    return national_number == BULBASAUR || national_number == CHARMANDER
           || national_number == SQUIRTLE;
}

// Oak's lab gives Red and Blue's player one of three starters and Yellow's Pikachu (pret
// OaksLab.asm); until then the byte is 0 in both.
static size_t find_types(spec_game_type_t types[static 2], const uint8_t *data,
                         const spec_gb_layout_t *layout) {
    uint16_t starter = spec_gb_species_to_national(data[layout->player_starter_offset]);
    if (starter == PIKACHU) {
        types[0] = SPEC_GAME_TYPE_YELLOW;
        return 1;
    }
    if (is_red_blue_starter(starter)) {
        types[0] = SPEC_GAME_TYPE_RED_BLUE;
        return 1;
    }
    types[0] = SPEC_GAME_TYPE_RED_BLUE;
    types[1] = SPEC_GAME_TYPE_YELLOW;
    return 2;
}

size_t spec_gb_identify_save(spec_gb_identity_t identities[static SPEC_GB_IDENTITY_MAX_COUNT],
                             const uint8_t data[static SPEC_GB_SAVE_SIZE]) {
    constexpr spec_language_t LAYOUT_LANGUAGES[] = {SPEC_LANGUAGE_JAPANESE, SPEC_LANGUAGE_ENGLISH};
    size_t identity_count = 0;
    for (size_t index = 0; index < sizeof LAYOUT_LANGUAGES / sizeof LAYOUT_LANGUAGES[0]; ++index) {
        const spec_gb_layout_t *layout = spec_gb_get_layout(LAYOUT_LANGUAGES[index]);
        if (!spec_gb_is_game_data_valid(data, layout) || !are_lists_sound(data, layout)) {
            continue;
        }
        spec_language_t language = SPEC_LANGUAGE_JAPANESE;
        if (LAYOUT_LANGUAGES[index] != SPEC_LANGUAGE_JAPANESE) {
            language = elect_save_language(data, layout);
        }
        spec_game_type_t types[2];
        size_t type_count = find_types(types, data, layout);
        for (size_t type_index = 0; type_index < type_count; ++type_index) {
            identities[identity_count++] = (spec_gb_identity_t){types[type_index], language};
        }
    }
    return identity_count;
}
