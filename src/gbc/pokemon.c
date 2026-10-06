// Gen 2 Pokémon records: the codec, stats and names.

#include <string.h>

#include "gb/gb_internal.h"
#include "gbc/gbc.h"
#include "gbc/gbc_internal.h"
#include "gbc/tables.h"
#include "spec_internal.h"

constexpr size_t SPECIES_OFFSET = 0x00;
constexpr size_t HELD_ITEM_OFFSET = 0x01;
constexpr size_t MOVES_OFFSET = 0x02;
constexpr size_t TRAINER_ID_OFFSET = 0x06;
constexpr size_t EXPERIENCE_OFFSET = 0x08;
constexpr size_t STAT_EXPERIENCE_OFFSET = 0x0B;
constexpr size_t DVS_OFFSET = 0x15;
constexpr size_t PP_OFFSET = 0x17;
constexpr size_t FRIENDSHIP_OFFSET = 0x1B;
constexpr size_t POKERUS_OFFSET = 0x1C;
constexpr size_t CAUGHT_TIME_LEVEL_OFFSET = 0x1D;
constexpr size_t CAUGHT_GENDER_LOCATION_OFFSET = 0x1E;
constexpr size_t LEVEL_OFFSET = 0x1F;
constexpr size_t STATUS_OFFSET = 0x20;
constexpr size_t CURRENT_HP_OFFSET = 0x22;
constexpr size_t STATS_OFFSET = 0x24;

constexpr unsigned DV_BIT_COUNT = 4;
constexpr unsigned PP_BIT_COUNT = 6;
constexpr unsigned PP_UP_BIT_COUNT = 2;
constexpr unsigned EXPERIENCE_BIT_COUNT = 24;
constexpr unsigned POKERUS_DAYS_BIT = 0;
constexpr unsigned POKERUS_STRAIN_BIT = 4;
constexpr unsigned POKERUS_BIT_COUNT = 4;
constexpr unsigned CAUGHT_LEVEL_BIT_COUNT = 6;
constexpr unsigned CAUGHT_TIME_BIT = 6;
constexpr unsigned CAUGHT_TIME_BIT_COUNT = 2;
constexpr unsigned CAUGHT_LOCATION_BIT_COUNT = 7;
constexpr unsigned CAUGHT_GENDER_BIT = 7;
constexpr unsigned SLEEP_TURNS_BIT_COUNT = 3;

// GetPokemonName and String_Egg; Spanish from erosunica/pokecrystal-es, Japanese pokesilver.
constexpr char8_t ENGLISH_EGG_NICKNAME[] = u8"EGG";
constexpr char8_t SPANISH_EGG_NICKNAME[] = u8"HUEVO";
constexpr char8_t JAPANESE_EGG_NICKNAME[] = u8"タマゴ";

static bool has_species_data(uint8_t species) {
    return species != 0 && species < SPEC_GBC_POKEDEX_SIZE;
}

// Special's DV and stat experience serve both special stats.
static uint16_t stat_of(const spec_gbc_pokemon_t *pokemon, spec_stat_t stat, uint8_t level) {
    constexpr spec_gb_stat_t STORED_STAT_OF[SPEC_STAT_COUNT] = {
        [SPEC_STAT_HP] = SPEC_GB_STAT_HP,
        [SPEC_STAT_ATTACK] = SPEC_GB_STAT_ATTACK,
        [SPEC_STAT_DEFENSE] = SPEC_GB_STAT_DEFENSE,
        [SPEC_STAT_SPEED] = SPEC_GB_STAT_SPEED,
        [SPEC_STAT_SPECIAL_ATTACK] = SPEC_GB_STAT_SPECIAL,
        [SPEC_STAT_SPECIAL_DEFENSE] = SPEC_GB_STAT_SPECIAL,
    };
    spec_gb_stat_t stored_stat = STORED_STAT_OF[stat];
    uint8_t dv = stat == SPEC_STAT_HP ? spec_gb_hp_dv(pokemon->dvs) : pokemon->dvs[stored_stat];
    return spec_gb_calculate_stat(spec_gbc_species_data[pokemon->species].base_stats[stat], dv,
                                  pokemon->stat_experience[stored_stat], level,
                                  stat == SPEC_STAT_HP);
}

static void set_stats(spec_gbc_pokemon_t *pokemon, uint8_t level) {
    for (spec_stat_t stat = SPEC_STAT_HP; stat < SPEC_STAT_COUNT; ++stat) {
        pokemon->party_data.stats[stat] = stat_of(pokemon, stat, level);
    }
}

static void decode_caught(spec_gbc_pokemon_t *pokemon, const uint8_t *record) {
    uint8_t time_and_level = record[CAUGHT_TIME_LEVEL_OFFSET];
    uint8_t gender_and_location = record[CAUGHT_GENDER_LOCATION_OFFSET];
    pokemon->caught.time_of_day = (spec_gbc_time_of_day_t)spec_get_bits(
        time_and_level, CAUGHT_TIME_BIT, CAUGHT_TIME_BIT_COUNT);
    pokemon->caught.level = (uint8_t)spec_get_bits(time_and_level, 0, CAUGHT_LEVEL_BIT_COUNT);
    pokemon->caught.location =
        (uint8_t)spec_get_bits(gender_and_location, 0, CAUGHT_LOCATION_BIT_COUNT);
    pokemon->trainer.is_female = spec_get_flag(gender_and_location, CAUGHT_GENDER_BIT);
}

static void decode_party_data(spec_gbc_party_data_t *party_data, const uint8_t *record) {
    spec_gb_decode_status(&party_data->status, record[STATUS_OFFSET]);
    party_data->current_hp = spec_read_u16_be(&record[CURRENT_HP_OFFSET]);
    for (size_t stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        party_data->stats[stat] = spec_read_u16_be(&record[STATS_OFFSET + stat * 2]);
    }
}

void spec_gbc_decode_pokemon(spec_gbc_pokemon_t *pokemon, const uint8_t *record,
                             size_t record_size) {
    pokemon->species = record[SPECIES_OFFSET];
    pokemon->held_item = record[HELD_ITEM_OFFSET];
    spec_gb_decode_moves(pokemon->moves, &record[MOVES_OFFSET], &record[PP_OFFSET]);
    pokemon->trainer.id = spec_read_u16_be(&record[TRAINER_ID_OFFSET]);
    pokemon->experience = spec_read_u24_be(&record[EXPERIENCE_OFFSET]);
    for (size_t stat = 0; stat < SPEC_GB_STAT_COUNT; ++stat) {
        pokemon->stat_experience[stat] =
            spec_read_u16_be(&record[STAT_EXPERIENCE_OFFSET + stat * 2]);
    }
    spec_gb_decode_dvs(pokemon->dvs, &record[DVS_OFFSET]);
    pokemon->traits = spec_gbc_decode_traits(pokemon->dvs, pokemon->species);
    pokemon->friendship = record[FRIENDSHIP_OFFSET];
    pokemon->pokerus.strain =
        (uint8_t)spec_get_bits(record[POKERUS_OFFSET], POKERUS_STRAIN_BIT, POKERUS_BIT_COUNT);
    pokemon->pokerus.days =
        (uint8_t)spec_get_bits(record[POKERUS_OFFSET], POKERUS_DAYS_BIT, POKERUS_BIT_COUNT);
    decode_caught(pokemon, record);
    pokemon->level = record[LEVEL_OFFSET];
    pokemon->party_data = (spec_gbc_party_data_t){};
    if (record_size == SPEC_GBC_PARTY_RECORD_SIZE) {
        decode_party_data(&pokemon->party_data, record);
    }
}

static void encode_caught(uint8_t *record, const spec_gbc_pokemon_t *pokemon) {
    uint32_t time_and_level = pokemon->caught.level;
    time_and_level = spec_set_bits(time_and_level, CAUGHT_TIME_BIT, CAUGHT_TIME_BIT_COUNT,
                                   pokemon->caught.time_of_day);
    uint32_t gender_and_location = pokemon->caught.location;
    gender_and_location =
        spec_set_bits(gender_and_location, CAUGHT_GENDER_BIT, 1, pokemon->trainer.is_female);
    record[CAUGHT_TIME_LEVEL_OFFSET] = (uint8_t)time_and_level;
    record[CAUGHT_GENDER_LOCATION_OFFSET] = (uint8_t)gender_and_location;
}

static void encode_party_data(uint8_t *record, const spec_gbc_party_data_t *party_data) {
    record[STATUS_OFFSET] = spec_gb_encode_status(&party_data->status);
    spec_write_u16_be(&record[CURRENT_HP_OFFSET], party_data->current_hp);
    for (size_t stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        spec_write_u16_be(&record[STATS_OFFSET + stat * 2], party_data->stats[stat]);
    }
}

static const char *unencodable_field_of(const spec_gbc_pokemon_t *pokemon) {
    for (size_t stat = SPEC_GB_STAT_ATTACK; stat < SPEC_GB_STAT_COUNT; ++stat) {
        if (!spec_fits_in_bits(pokemon->dvs[stat], DV_BIT_COUNT)) {
            return "dvs do not fit in 4 bits";
        }
    }
    for (size_t move = 0; move < SPEC_GBC_MOVE_COUNT; ++move) {
        if (!spec_fits_in_bits(pokemon->moves[move].pp, PP_BIT_COUNT)) {
            return "a move's pp does not fit in 6 bits";
        }
        if (!spec_fits_in_bits(pokemon->moves[move].pp_ups, PP_UP_BIT_COUNT)) {
            return "a move's pp_ups do not fit in 2 bits";
        }
    }
    if (!spec_fits_in_bits(pokemon->experience, EXPERIENCE_BIT_COUNT)) {
        return "experience does not fit in 24 bits";
    }
    if (!spec_fits_in_bits(pokemon->pokerus.strain, POKERUS_BIT_COUNT)
        || !spec_fits_in_bits(pokemon->pokerus.days, POKERUS_BIT_COUNT)) {
        return "pokerus does not fit in 4 bits";
    }
    if (!spec_fits_in_bits(pokemon->caught.time_of_day, CAUGHT_TIME_BIT_COUNT)
        || !spec_fits_in_bits(pokemon->caught.level, CAUGHT_LEVEL_BIT_COUNT)
        || !spec_fits_in_bits(pokemon->caught.location, CAUGHT_LOCATION_BIT_COUNT)) {
        return "caught does not fit in its bits";
    }
    if (!spec_fits_in_bits(pokemon->party_data.status.sleep_turns, SLEEP_TURNS_BIT_COUNT)) {
        return "party_data.status.sleep_turns does not fit in 3 bits";
    }
    return nullptr;
}

spec_error_t spec_gbc_encode_pokemon(uint8_t *record, size_t record_size,
                                     const spec_gbc_pokemon_t *pokemon) {
    const char *unencodable_field = unencodable_field_of(pokemon);
    if (unencodable_field != nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, unencodable_field);
    }
    uint8_t plain[SPEC_GBC_PARTY_RECORD_SIZE] = {};
    plain[SPECIES_OFFSET] = pokemon->species;
    plain[HELD_ITEM_OFFSET] = pokemon->held_item;
    spec_gb_encode_moves(&plain[MOVES_OFFSET], &plain[PP_OFFSET], pokemon->moves);
    spec_write_u16_be(&plain[TRAINER_ID_OFFSET], pokemon->trainer.id);
    spec_write_u24_be(&plain[EXPERIENCE_OFFSET], pokemon->experience);
    for (size_t stat = 0; stat < SPEC_GB_STAT_COUNT; ++stat) {
        spec_write_u16_be(&plain[STAT_EXPERIENCE_OFFSET + stat * 2],
                          pokemon->stat_experience[stat]);
    }
    spec_gb_encode_dvs(&plain[DVS_OFFSET], pokemon->dvs);
    plain[FRIENDSHIP_OFFSET] = pokemon->friendship;
    plain[POKERUS_OFFSET] = (uint8_t)(pokemon->pokerus.strain << POKERUS_STRAIN_BIT
                                      | pokemon->pokerus.days << POKERUS_DAYS_BIT);
    encode_caught(plain, pokemon);
    plain[LEVEL_OFFSET] = pokemon->level;
    encode_party_data(plain, &pokemon->party_data);
    memcpy(record, plain, record_size);
    return SPEC_OK;
}

// As withdrawing does: the stats at the stored level, and full HP.
void spec_gbc_fill_party_data(spec_gbc_pokemon_t *pokemon) {
    bool has_party_data = pokemon->party_data.stats[SPEC_STAT_HP] != 0;
    if (has_party_data || !has_species_data(pokemon->species)) {
        return;
    }
    set_stats(pokemon, pokemon->level);
    pokemon->party_data.current_hp = pokemon->party_data.stats[SPEC_STAT_HP];
}

spec_error_t spec_gbc_pokemon_get_name(const spec_gbc_pokemon_t *pokemon,
                                       char8_t name[static SPEC_GBC_TEXT_BUFFER_SIZE],
                                       spec_language_t language) {
    return spec_gbc_text_to_utf8(name, pokemon->nickname, spec_gb_name_size(language), language);
}

// The game's PC refuses Pokémon holding mail.
bool spec_gbc_pokemon_is_safe_to_box(const spec_gbc_pokemon_t *pokemon) {
    return !spec_gbc_is_mail(pokemon->held_item);
}

// A level-up adds the max HP's gain to the current HP; a fainted Pokémon stays fainted.
spec_error_t spec_gbc_pokemon_calculate_stats(spec_gbc_pokemon_t *pokemon) {
    if (!has_species_data(pokemon->species)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "species has no species data");
    }
    uint16_t old_max_hp = pokemon->party_data.stats[SPEC_STAT_HP];
    pokemon->level = spec_level_for_experience(spec_gbc_species_data[pokemon->species].growth_rate,
                                               pokemon->experience);
    set_stats(pokemon, pokemon->level);
    uint16_t new_max_hp = pokemon->party_data.stats[SPEC_STAT_HP];
    uint16_t current_hp = pokemon->party_data.current_hp;
    if (current_hp != 0 || old_max_hp == 0) {
        pokemon->party_data.current_hp = (uint16_t)(current_hp + new_max_hp - old_max_hp);
    }
    return SPEC_OK;
}

static const char8_t *egg_nickname_of(spec_language_t language) {
    switch (language) {
        case SPEC_LANGUAGE_JAPANESE:
            return JAPANESE_EGG_NICKNAME;
        case SPEC_LANGUAGE_SPANISH:
            return SPANISH_EGG_NICKNAME;
        default:
            // TODO: Determine egg names for French, German and Italian.
            return ENGLISH_EGG_NICKNAME;
    }
}

// The game writes an egg's name over the old one, keeping what follows its terminator.
static spec_error_t write_egg_nickname(spec_gbc_pokemon_t *pokemon, spec_language_t language) {
    size_t name_size = spec_gb_name_size(language);
    uint8_t egg_nickname[SPEC_GBC_NAME_SIZE];
    spec_error_t error =
        spec_gbc_text_from_utf8(egg_nickname, name_size, egg_nickname_of(language), language);
    if (error != SPEC_OK) {
        return error;
    }
    for (size_t index = 0; index < name_size; ++index) {
        pokemon->nickname[index] = egg_nickname[index];
        if (egg_nickname[index] == SPEC_GB_END_OF_TEXT) {
            break;
        }
    }
    return SPEC_OK;
}

// As the games name a new Pokémon: its species name, padded with terminators.
spec_error_t spec_gbc_pokemon_remove_nickname(spec_gbc_pokemon_t *pokemon,
                                              spec_language_t language) {
    if (pokemon->is_egg) {
        return write_egg_nickname(pokemon, language);
    }
    const char8_t *name = spec_game_boy_species_name(pokemon->species, language);
    if (name == nullptr) {
        return spec_fail(SPEC_ERROR_UNKNOWN_NAME, "the species has no name in that language");
    }
    return spec_gbc_text_from_utf8(pokemon->nickname, spec_gb_name_size(language), name, language);
}

// The level's least experience, as a Rare Candy leaves it.
spec_error_t spec_gbc_pokemon_set_level(spec_gbc_pokemon_t *pokemon, uint8_t level) {
    if (!has_species_data(pokemon->species)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "species has no species data");
    }
    if (level == 0 || level >= SPEC_LEVEL_COUNT) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "level is not 1 to 100");
    }
    pokemon->experience =
        spec_experience[spec_gbc_species_data[pokemon->species].growth_rate][level];
    return spec_gbc_pokemon_calculate_stats(pokemon);
}

// The naming screen fills its buffer with terminators, then writes the whole of it.
spec_error_t spec_gbc_pokemon_set_nickname(spec_gbc_pokemon_t *pokemon, const char8_t *nickname,
                                           spec_language_t language) {
    if (pokemon->is_egg) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "the games never name an egg");
    }
    return spec_gbc_text_from_utf8(pokemon->nickname, spec_gb_name_size(language), nickname,
                                   language);
}
