// Gen 1 Pokémon records: the codec, stats, names and species numbering.

#include <string.h>

#include "gb/gb.h"
#include "gb/gb_internal.h"
#include "gb/tables.h"
#include "spec_internal.h"

constexpr size_t SPECIES_OFFSET = 0x00;
constexpr size_t CURRENT_HP_OFFSET = 0x01;
constexpr size_t BOX_LEVEL_OFFSET = 0x03;
constexpr size_t STATUS_OFFSET = 0x04;
constexpr size_t TYPES_OFFSET = 0x05;
constexpr size_t CATCH_RATE_OFFSET = 0x07;
constexpr size_t MOVES_OFFSET = 0x08;
constexpr size_t TRAINER_ID_OFFSET = 0x0C;
constexpr size_t EXPERIENCE_OFFSET = 0x0E;
constexpr size_t STAT_EXPERIENCE_OFFSET = 0x11;
constexpr size_t DVS_OFFSET = 0x1B;
constexpr size_t PP_OFFSET = 0x1D;
constexpr size_t LEVEL_OFFSET = 0x21;
constexpr size_t STATS_OFFSET = 0x22;

constexpr unsigned SLEEP_TURNS_BIT = 0;
constexpr unsigned SLEEP_TURNS_BIT_COUNT = 3;
constexpr unsigned POISONED_BIT = 3;
constexpr unsigned BURNED_BIT = 4;
constexpr unsigned FROZEN_BIT = 5;
constexpr unsigned PARALYZED_BIT = 6;

constexpr unsigned DV_BIT_COUNT = 4;
constexpr unsigned PP_BIT_COUNT = 6;
constexpr unsigned PP_UP_BIT_COUNT = 2;
constexpr unsigned EXPERIENCE_BIT_COUNT = 24;

constexpr uint16_t MAX_STAT_VALUE = 999;
constexpr unsigned MAX_STAT_EXPERIENCE_ROOT = 255;
constexpr uint8_t PERIOD = 0xE8;
constexpr uint8_t DECIMAL_POINT = 0xF2;

static bool get_flag(uint32_t word, unsigned bit) {
    return spec_get_bits(word, bit, 1) != 0;
}

static uint32_t set_flag(uint32_t word, unsigned bit, bool is_set) {
    return spec_set_bits(word, bit, 1, is_set);
}

// The HP DV is the low bits of the others, Attack's highest.
static uint8_t hp_dv_of(const uint8_t dvs[static SPEC_GB_STAT_COUNT]) {
    return (uint8_t)((dvs[SPEC_GB_STAT_ATTACK] & 1) << 3 | (dvs[SPEC_GB_STAT_DEFENSE] & 1) << 2
                     | (dvs[SPEC_GB_STAT_SPEED] & 1) << 1 | (dvs[SPEC_GB_STAT_SPECIAL] & 1));
}

static void decode_dvs(uint8_t dvs[static SPEC_GB_STAT_COUNT], const uint8_t *bytes) {
    dvs[SPEC_GB_STAT_ATTACK] = (uint8_t)spec_get_bits(bytes[0], DV_BIT_COUNT, DV_BIT_COUNT);
    dvs[SPEC_GB_STAT_DEFENSE] = (uint8_t)spec_get_bits(bytes[0], 0, DV_BIT_COUNT);
    dvs[SPEC_GB_STAT_SPEED] = (uint8_t)spec_get_bits(bytes[1], DV_BIT_COUNT, DV_BIT_COUNT);
    dvs[SPEC_GB_STAT_SPECIAL] = (uint8_t)spec_get_bits(bytes[1], 0, DV_BIT_COUNT);
    dvs[SPEC_GB_STAT_HP] = hp_dv_of(dvs);
}

static void encode_dvs(uint8_t *bytes, const uint8_t dvs[static SPEC_GB_STAT_COUNT]) {
    bytes[0] = (uint8_t)(dvs[SPEC_GB_STAT_ATTACK] << DV_BIT_COUNT | dvs[SPEC_GB_STAT_DEFENSE]);
    bytes[1] = (uint8_t)(dvs[SPEC_GB_STAT_SPEED] << DV_BIT_COUNT | dvs[SPEC_GB_STAT_SPECIAL]);
}

static void decode_moves(spec_gb_move_t moves[static SPEC_GB_MOVE_COUNT], const uint8_t *record) {
    for (size_t move = 0; move < SPEC_GB_MOVE_COUNT; ++move) {
        uint8_t pp = record[PP_OFFSET + move];
        moves[move].id = record[MOVES_OFFSET + move];
        moves[move].pp = (uint8_t)spec_get_bits(pp, 0, PP_BIT_COUNT);
        moves[move].pp_ups = (uint8_t)spec_get_bits(pp, PP_BIT_COUNT, PP_UP_BIT_COUNT);
    }
}

static void encode_moves(uint8_t *record, const spec_gb_move_t moves[static SPEC_GB_MOVE_COUNT]) {
    for (size_t move = 0; move < SPEC_GB_MOVE_COUNT; ++move) {
        uint32_t pp =
            spec_set_bits(moves[move].pp, PP_BIT_COUNT, PP_UP_BIT_COUNT, moves[move].pp_ups);
        record[MOVES_OFFSET + move] = moves[move].id;
        record[PP_OFFSET + move] = (uint8_t)pp;
    }
}

static void decode_party_data(spec_gb_party_data_t *party_data, const uint8_t *record) {
    party_data->level = record[LEVEL_OFFSET];
    for (size_t stat = 0; stat < SPEC_GB_STAT_COUNT; ++stat) {
        party_data->stats[stat] = spec_read_u16_be(&record[STATS_OFFSET + stat * 2]);
    }
}

static void encode_party_data(uint8_t *record, const spec_gb_party_data_t *party_data) {
    record[LEVEL_OFFSET] = party_data->level;
    for (size_t stat = 0; stat < SPEC_GB_STAT_COUNT; ++stat) {
        spec_write_u16_be(&record[STATS_OFFSET + stat * 2], party_data->stats[stat]);
    }
}

static const char *unencodable_field_of(const spec_gb_pokemon_t *pokemon) {
    if (!spec_fits_in_bits(pokemon->status.sleep_turns, SLEEP_TURNS_BIT_COUNT)) {
        return "status.sleep_turns does not fit in 3 bits";
    }
    for (size_t stat = SPEC_GB_STAT_ATTACK; stat < SPEC_GB_STAT_COUNT; ++stat) {
        if (!spec_fits_in_bits(pokemon->dvs[stat], DV_BIT_COUNT)) {
            return "dvs do not fit in 4 bits";
        }
    }
    for (size_t move = 0; move < SPEC_GB_MOVE_COUNT; ++move) {
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
    return nullptr;
}

void spec_gb_decode_status(spec_gb_status_t *status, uint8_t byte) {
    status->sleep_turns = (uint8_t)spec_get_bits(byte, SLEEP_TURNS_BIT, SLEEP_TURNS_BIT_COUNT);
    status->is_poisoned = get_flag(byte, POISONED_BIT);
    status->is_burned = get_flag(byte, BURNED_BIT);
    status->is_frozen = get_flag(byte, FROZEN_BIT);
    status->is_paralyzed = get_flag(byte, PARALYZED_BIT);
}

uint8_t spec_gb_encode_status(const spec_gb_status_t *status) {
    uint32_t byte = 0;
    byte = spec_set_bits(byte, SLEEP_TURNS_BIT, SLEEP_TURNS_BIT_COUNT, status->sleep_turns);
    byte = set_flag(byte, POISONED_BIT, status->is_poisoned);
    byte = set_flag(byte, BURNED_BIT, status->is_burned);
    byte = set_flag(byte, FROZEN_BIT, status->is_frozen);
    byte = set_flag(byte, PARALYZED_BIT, status->is_paralyzed);
    return (uint8_t)byte;
}

void spec_gb_decode_pokemon(spec_gb_pokemon_t *pokemon, const uint8_t *record, size_t record_size) {
    pokemon->species = record[SPECIES_OFFSET];
    pokemon->current_hp = spec_read_u16_be(&record[CURRENT_HP_OFFSET]);
    pokemon->box_level = record[BOX_LEVEL_OFFSET];
    spec_gb_decode_status(&pokemon->status, record[STATUS_OFFSET]);
    memcpy(pokemon->types, &record[TYPES_OFFSET], SPEC_GB_TYPE_COUNT);
    pokemon->catch_rate = record[CATCH_RATE_OFFSET];
    decode_moves(pokemon->moves, record);
    pokemon->trainer.id = spec_read_u16_be(&record[TRAINER_ID_OFFSET]);
    pokemon->experience = spec_read_u24_be(&record[EXPERIENCE_OFFSET]);
    for (size_t stat = 0; stat < SPEC_GB_STAT_COUNT; ++stat) {
        pokemon->stat_experience[stat] =
            spec_read_u16_be(&record[STAT_EXPERIENCE_OFFSET + stat * 2]);
    }
    decode_dvs(pokemon->dvs, &record[DVS_OFFSET]);
    pokemon->party_data = (spec_gb_party_data_t){};
    if (record_size == SPEC_GB_PARTY_RECORD_SIZE) {
        decode_party_data(&pokemon->party_data, record);
    }
}

spec_error_t spec_gb_encode_pokemon(uint8_t *record, size_t record_size,
                                    const spec_gb_pokemon_t *pokemon) {
    const char *unencodable_field = unencodable_field_of(pokemon);
    if (unencodable_field != nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, unencodable_field);
    }
    uint8_t plain[SPEC_GB_PARTY_RECORD_SIZE] = {};
    plain[SPECIES_OFFSET] = pokemon->species;
    spec_write_u16_be(&plain[CURRENT_HP_OFFSET], pokemon->current_hp);
    plain[BOX_LEVEL_OFFSET] = pokemon->box_level;
    plain[STATUS_OFFSET] = spec_gb_encode_status(&pokemon->status);
    memcpy(&plain[TYPES_OFFSET], pokemon->types, SPEC_GB_TYPE_COUNT);
    plain[CATCH_RATE_OFFSET] = pokemon->catch_rate;
    encode_moves(plain, pokemon->moves);
    spec_write_u16_be(&plain[TRAINER_ID_OFFSET], pokemon->trainer.id);
    spec_write_u24_be(&plain[EXPERIENCE_OFFSET], pokemon->experience);
    for (size_t stat = 0; stat < SPEC_GB_STAT_COUNT; ++stat) {
        spec_write_u16_be(&plain[STAT_EXPERIENCE_OFFSET + stat * 2],
                          pokemon->stat_experience[stat]);
    }
    encode_dvs(&plain[DVS_OFFSET], pokemon->dvs);
    encode_party_data(plain, &pokemon->party_data);
    memcpy(record, plain, record_size);
    return SPEC_OK;
}

uint8_t spec_gb_level_for_experience(spec_growth_rate_t growth_rate, uint32_t experience) {
    uint8_t level = spec_level_for_experience(growth_rate, experience);
    return level == 0 ? 1 : level;
}

// The smallest root whose square reaches the stat experience, counted up to 255.
static unsigned stat_experience_bonus(uint16_t stat_experience) {
    unsigned root = 1;
    while (root < MAX_STAT_EXPERIENCE_ROOT && root * root < stat_experience) {
        ++root;
    }
    return root / 4;
}

uint16_t spec_gb_calculate_stat(uint8_t base_stat, uint8_t dv, uint16_t stat_experience,
                                uint8_t level, bool is_hp) {
    unsigned doubled_stat = (base_stat + dv) * 2U + stat_experience_bonus(stat_experience);
    unsigned value = doubled_stat * level / 100 + (is_hp ? level + 10U : 5U);
    return (uint16_t)(value > MAX_STAT_VALUE ? MAX_STAT_VALUE : value);
}

static const spec_gb_species_data_t *species_data_of(const spec_gb_pokemon_t *pokemon) {
    if (spec_gb_species_to_national(pokemon->species) == 0) {
        return nullptr;
    }
    return &spec_gb_species_data[pokemon->species];
}

// The HP DV comes from the others, since dvs[SPEC_GB_STAT_HP] is ignored on write.
static void set_level_and_stats(spec_gb_pokemon_t *pokemon,
                                const spec_gb_species_data_t *species_data) {
    uint8_t level = spec_gb_level_for_experience(species_data->growth_rate, pokemon->experience);
    pokemon->party_data.level = level;
    for (size_t stat = 0; stat < SPEC_GB_STAT_COUNT; ++stat) {
        uint8_t dv = stat == SPEC_GB_STAT_HP ? hp_dv_of(pokemon->dvs) : pokemon->dvs[stat];
        pokemon->party_data.stats[stat] =
            spec_gb_calculate_stat(species_data->base_stats[stat], dv,
                                   pokemon->stat_experience[stat], level, stat == SPEC_GB_STAT_HP);
    }
}

// A level-up adds the max HP's gain to the current HP; a fainted Pokémon stays fainted.
spec_error_t spec_gb_pokemon_calculate_stats(spec_gb_pokemon_t *pokemon) {
    const spec_gb_species_data_t *species_data = species_data_of(pokemon);
    if (species_data == nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "species has no species data");
    }
    uint16_t old_max_hp = pokemon->party_data.stats[SPEC_GB_STAT_HP];
    set_level_and_stats(pokemon, species_data);
    uint16_t new_max_hp = pokemon->party_data.stats[SPEC_GB_STAT_HP];
    if (pokemon->current_hp != 0 || old_max_hp == 0) {
        pokemon->current_hp = (uint16_t)(pokemon->current_hp + new_max_hp - old_max_hp);
    }
    return SPEC_OK;
}

// As withdrawing does: the level from the experience, then the stats; the HP stays.
void spec_gb_fill_party_data(spec_gb_pokemon_t *pokemon) {
    bool has_party_data =
        pokemon->party_data.level != 0 || pokemon->party_data.stats[SPEC_GB_STAT_HP] != 0;
    const spec_gb_species_data_t *species_data = species_data_of(pokemon);
    if (has_party_data || species_data == nullptr) {
        return;
    }
    set_level_and_stats(pokemon, species_data);
}

spec_error_t spec_gb_pokemon_get_name(const spec_gb_pokemon_t *pokemon,
                                      char8_t name[static SPEC_GB_TEXT_BUFFER_SIZE],
                                      spec_language_t language) {
    return spec_gb_text_to_utf8(name, pokemon->nickname, spec_gb_name_size(language), language);
}

// The naming keyboard types '.' as the decimal point, where species names use the period.
static void type_periods_as_decimal_points(uint8_t *text, size_t text_size) {
    for (size_t index = 0; index < text_size; ++index) {
        if (text[index] == PERIOD) {
            text[index] = DECIMAL_POINT;
        }
    }
}

// As the naming screen writes it, but for the bytes after the terminator.
spec_error_t spec_gb_pokemon_set_nickname(spec_gb_pokemon_t *pokemon, const char8_t *nickname,
                                          spec_language_t language) {
    uint8_t text[SPEC_GB_NAME_SIZE] = {};
    size_t name_size = spec_gb_name_size(language);
    spec_error_t error = spec_gb_text_from_utf8(text, name_size, nickname, language);
    if (error != SPEC_OK) {
        return error;
    }
    if (language != SPEC_LANGUAGE_JAPANESE) {
        type_periods_as_decimal_points(text, name_size);
    }
    // TODO: Check trash bytes on Gen 1 naming; the game copies its whole text buffer.
    memcpy(pokemon->nickname, text, name_size);
    return SPEC_OK;
}

// As the game names a new Pokémon: its species name, padded with terminators.
spec_error_t spec_gb_pokemon_remove_nickname(spec_gb_pokemon_t *pokemon, spec_language_t language) {
    const char8_t *name =
        spec_game_boy_species_name(spec_gb_species_to_national(pokemon->species), language);
    if (name == nullptr) {
        return spec_fail(SPEC_ERROR_UNKNOWN_NAME, "the species has no name in that language");
    }
    return spec_gb_text_from_utf8(pokemon->nickname, spec_gb_name_size(language), name, language);
}

uint16_t spec_gb_species_to_national(uint8_t species) {
    if (species >= SPEC_GB_SPECIES_INDEX_COUNT) {
        return 0;
    }
    return spec_gb_national_of_species[species];
}

uint8_t spec_gb_species_from_national(uint16_t national_number) {
    if (national_number >= SPEC_GB_POKEDEX_SIZE) {
        return 0;
    }
    return spec_gb_species_of_national[national_number];
}
