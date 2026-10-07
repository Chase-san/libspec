// Gen 1 Pokémon records: the codec, stats, names, and species data and numbering.

#include <string.h>

#include "gb/gb.h"
#include "gb/gb_internal.h"
#include "gb/tables.h"
#include "spec_internal.h"

constexpr size_t SPECIES_OFFSET = 0x00;         // pret/pokered box_struct Species
constexpr size_t CURRENT_HP_OFFSET = 0x01;      // pret/pokered box_struct HP
constexpr size_t BOX_LEVEL_OFFSET = 0x03;       // pret/pokered box_struct BoxLevel
constexpr size_t STATUS_OFFSET = 0x04;          // pret/pokered box_struct Status
constexpr size_t TYPES_OFFSET = 0x05;           // pret/pokered box_struct Type
constexpr size_t CATCH_RATE_OFFSET = 0x07;      // pret/pokered box_struct CatchRate
constexpr size_t MOVES_OFFSET = 0x08;           // pret/pokered box_struct Moves
constexpr size_t TRAINER_ID_OFFSET = 0x0C;      // pret/pokered box_struct OTID
constexpr size_t EXPERIENCE_OFFSET = 0x0E;      // pret/pokered box_struct Exp
constexpr size_t STAT_EXPERIENCE_OFFSET = 0x11; // pret/pokered box_struct HPExp
constexpr size_t DVS_OFFSET = 0x1B;             // pret/pokered box_struct DVs
constexpr size_t PP_OFFSET = 0x1D;              // pret/pokered box_struct PP
constexpr size_t LEVEL_OFFSET = 0x21;           // pret/pokered party_struct Level
constexpr size_t STATS_OFFSET = 0x22;           // pret/pokered party_struct Stats

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
constexpr uint8_t NO_TYPE_NUMBER = 0xFF;

struct type_number {
    spec_type_t type;
    uint8_t number;
};
typedef struct type_number type_number_t;

// The number a Gen 1 record holds each type as (pret pokered constants/type_constants.asm).
constexpr type_number_t TYPE_NUMBERS[] = {
    {SPEC_TYPE_NORMAL, 0x00},  {SPEC_TYPE_FIGHTING, 0x01}, {SPEC_TYPE_FLYING, 0x02},
    {SPEC_TYPE_POISON, 0x03},  {SPEC_TYPE_GROUND, 0x04},   {SPEC_TYPE_ROCK, 0x05},
    {SPEC_TYPE_BUG, 0x07},     {SPEC_TYPE_GHOST, 0x08},    {SPEC_TYPE_FIRE, 0x14},
    {SPEC_TYPE_WATER, 0x15},   {SPEC_TYPE_GRASS, 0x16},    {SPEC_TYPE_ELECTRIC, 0x17},
    {SPEC_TYPE_PSYCHIC, 0x18}, {SPEC_TYPE_ICE, 0x19},      {SPEC_TYPE_DRAGON, 0x1A},
};
constexpr size_t TYPE_NUMBER_COUNT = sizeof TYPE_NUMBERS / sizeof TYPE_NUMBERS[0];

// Every Gen 1 game has the same base stats and growth rates.
static const spec_gb_species_data_t *species_data_of(const spec_gb_pokemon_t *pokemon) {
    return spec_gb_get_species_data(SPEC_GAME_TYPE_RED_BLUE, pokemon->species);
}

static uint8_t number_of_type(spec_type_t type) {
    for (size_t index = 0; index < TYPE_NUMBER_COUNT; ++index) {
        if (TYPE_NUMBERS[index].type == type) {
            return TYPE_NUMBERS[index].number;
        }
    }
    return NO_TYPE_NUMBER;
}

static size_t text_length(const uint8_t *text, size_t text_size) {
    size_t length = 0;
    while (length < text_size && text[length] != SPEC_GB_END_OF_TEXT) {
        ++length;
    }
    return length;
}

// The HP DV comes from the others, since dvs[SPEC_GB_STAT_HP] is ignored on write.
static void set_level_and_stats(spec_gb_pokemon_t *pokemon,
                                const spec_gb_species_data_t *species_data) {
    uint8_t level = spec_level_for_experience(species_data->growth_rate, pokemon->experience);
    pokemon->party_data.level = level;
    for (size_t stat = 0; stat < SPEC_GB_STAT_COUNT; ++stat) {
        uint8_t dv = stat == SPEC_GB_STAT_HP ? spec_gb_hp_dv(pokemon->dvs) : pokemon->dvs[stat];
        pokemon->party_data.stats[stat] =
            spec_gb_calculate_stat(species_data->base_stats[stat], dv,
                                   pokemon->stat_experience[stat], level, stat == SPEC_GB_STAT_HP);
    }
}

void spec_gb_decode_dvs(uint8_t dvs[static SPEC_GB_STAT_COUNT], const uint8_t *bytes) {
    dvs[SPEC_GB_STAT_ATTACK] = (uint8_t)spec_get_bits(bytes[0], DV_BIT_COUNT, DV_BIT_COUNT);
    dvs[SPEC_GB_STAT_DEFENSE] = (uint8_t)spec_get_bits(bytes[0], 0, DV_BIT_COUNT);
    dvs[SPEC_GB_STAT_SPEED] = (uint8_t)spec_get_bits(bytes[1], DV_BIT_COUNT, DV_BIT_COUNT);
    dvs[SPEC_GB_STAT_SPECIAL] = (uint8_t)spec_get_bits(bytes[1], 0, DV_BIT_COUNT);
    dvs[SPEC_GB_STAT_HP] = spec_gb_hp_dv(dvs);
}

void spec_gb_decode_moves(spec_gb_move_t moves[static SPEC_GB_MOVE_COUNT], const uint8_t *ids,
                          const uint8_t *pp_bytes) {
    for (size_t move = 0; move < SPEC_GB_MOVE_COUNT; ++move) {
        moves[move].id = ids[move];
        moves[move].pp = (uint8_t)spec_get_bits(pp_bytes[move], 0, PP_BIT_COUNT);
        moves[move].pp_ups = (uint8_t)spec_get_bits(pp_bytes[move], PP_BIT_COUNT, PP_UP_BIT_COUNT);
    }
}

static void decode_party_data(spec_gb_party_data_t *party_data, const uint8_t *record) {
    party_data->level = record[LEVEL_OFFSET];
    for (size_t stat = 0; stat < SPEC_GB_STAT_COUNT; ++stat) {
        party_data->stats[stat] = spec_read_u16_be(&record[STATS_OFFSET + stat * 2]);
    }
}

static spec_type_t type_of_number(uint8_t number) {
    for (size_t index = 0; index < TYPE_NUMBER_COUNT; ++index) {
        if (TYPE_NUMBERS[index].number == number) {
            return TYPE_NUMBERS[index].type;
        }
    }
    return SPEC_TYPE_COUNT;
}

void spec_gb_decode_pokemon(spec_gb_pokemon_t *pokemon, const uint8_t *record, size_t record_size) {
    pokemon->species = record[SPECIES_OFFSET];
    pokemon->current_hp = spec_read_u16_be(&record[CURRENT_HP_OFFSET]);
    pokemon->box_level = record[BOX_LEVEL_OFFSET];
    spec_gb_decode_status(&pokemon->status, record[STATUS_OFFSET]);
    for (size_t index = 0; index < SPEC_GB_TYPE_COUNT; ++index) {
        pokemon->types[index] = type_of_number(record[TYPES_OFFSET + index]);
    }
    pokemon->catch_rate = record[CATCH_RATE_OFFSET];
    spec_gb_decode_moves(pokemon->moves, &record[MOVES_OFFSET], &record[PP_OFFSET]);
    pokemon->trainer.id = spec_read_u16_be(&record[TRAINER_ID_OFFSET]);
    pokemon->experience = spec_read_u24_be(&record[EXPERIENCE_OFFSET]);
    for (size_t stat = 0; stat < SPEC_GB_STAT_COUNT; ++stat) {
        pokemon->stat_experience[stat] =
            spec_read_u16_be(&record[STAT_EXPERIENCE_OFFSET + stat * 2]);
    }
    spec_gb_decode_dvs(pokemon->dvs, &record[DVS_OFFSET]);
    pokemon->party_data = (spec_gb_party_data_t){};
    if (record_size == SPEC_GB_PARTY_RECORD_SIZE) {
        decode_party_data(&pokemon->party_data, record);
    }
}

void spec_gb_decode_status(spec_gb_status_t *status, uint8_t byte) {
    status->sleep_turns = (uint8_t)spec_get_bits(byte, SLEEP_TURNS_BIT, SLEEP_TURNS_BIT_COUNT);
    status->is_poisoned = spec_get_flag(byte, POISONED_BIT);
    status->is_burned = spec_get_flag(byte, BURNED_BIT);
    status->is_frozen = spec_get_flag(byte, FROZEN_BIT);
    status->is_paralyzed = spec_get_flag(byte, PARALYZED_BIT);
}

void spec_gb_encode_dvs(uint8_t *bytes, const uint8_t dvs[static SPEC_GB_STAT_COUNT]) {
    bytes[0] = (uint8_t)(dvs[SPEC_GB_STAT_ATTACK] << DV_BIT_COUNT | dvs[SPEC_GB_STAT_DEFENSE]);
    bytes[1] = (uint8_t)(dvs[SPEC_GB_STAT_SPEED] << DV_BIT_COUNT | dvs[SPEC_GB_STAT_SPECIAL]);
}

void spec_gb_encode_moves(uint8_t *ids, uint8_t *pp_bytes,
                          const spec_gb_move_t moves[static SPEC_GB_MOVE_COUNT]) {
    for (size_t move = 0; move < SPEC_GB_MOVE_COUNT; ++move) {
        ids[move] = moves[move].id;
        pp_bytes[move] = (uint8_t)spec_set_bits(moves[move].pp, PP_BIT_COUNT, PP_UP_BIT_COUNT,
                                                moves[move].pp_ups);
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
    for (size_t index = 0; index < SPEC_GB_TYPE_COUNT; ++index) {
        if (number_of_type(pokemon->types[index]) == NO_TYPE_NUMBER) {
            return "types hold a type Gen 1 lacks";
        }
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
    for (size_t index = 0; index < SPEC_GB_TYPE_COUNT; ++index) {
        plain[TYPES_OFFSET + index] = number_of_type(pokemon->types[index]);
    }
    plain[CATCH_RATE_OFFSET] = pokemon->catch_rate;
    spec_gb_encode_moves(&plain[MOVES_OFFSET], &plain[PP_OFFSET], pokemon->moves);
    spec_write_u16_be(&plain[TRAINER_ID_OFFSET], pokemon->trainer.id);
    spec_write_u24_be(&plain[EXPERIENCE_OFFSET], pokemon->experience);
    for (size_t stat = 0; stat < SPEC_GB_STAT_COUNT; ++stat) {
        spec_write_u16_be(&plain[STAT_EXPERIENCE_OFFSET + stat * 2],
                          pokemon->stat_experience[stat]);
    }
    spec_gb_encode_dvs(&plain[DVS_OFFSET], pokemon->dvs);
    encode_party_data(plain, &pokemon->party_data);
    memcpy(record, plain, record_size);
    return SPEC_OK;
}

uint8_t spec_gb_encode_status(const spec_gb_status_t *status) {
    uint32_t byte = 0;
    byte = spec_set_bits(byte, SLEEP_TURNS_BIT, SLEEP_TURNS_BIT_COUNT, status->sleep_turns);
    byte = spec_set_flag(byte, POISONED_BIT, status->is_poisoned);
    byte = spec_set_flag(byte, BURNED_BIT, status->is_burned);
    byte = spec_set_flag(byte, FROZEN_BIT, status->is_frozen);
    byte = spec_set_flag(byte, PARALYZED_BIT, status->is_paralyzed);
    return (uint8_t)byte;
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

// The HP DV is the low bits of the others, Attack's highest.
uint8_t spec_gb_hp_dv(const uint8_t dvs[static SPEC_GB_STAT_COUNT]) {
    return (uint8_t)((dvs[SPEC_GB_STAT_ATTACK] & 1) << 3 | (dvs[SPEC_GB_STAT_DEFENSE] & 1) << 2
                     | (dvs[SPEC_GB_STAT_SPEED] & 1) << 1 | (dvs[SPEC_GB_STAT_SPECIAL] & 1));
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

// As the game names a new Pokémon: its species name, padded with terminators.
spec_error_t spec_gb_pokemon_remove_nickname(spec_gb_pokemon_t *pokemon, spec_language_t language) {
    const char8_t *name =
        spec_game_boy_species_name(spec_gb_species_to_national(pokemon->species), language);
    if (name == nullptr) {
        return spec_fail(SPEC_ERROR_UNKNOWN_NAME, "the species has no name in that language");
    }
    return spec_gb_text_from_utf8(pokemon->nickname, spec_gb_name_size(language), name, language);
}

// The level's least experience, as a Rare Candy leaves it, and the level boxes show.
spec_error_t spec_gb_pokemon_set_level(spec_gb_pokemon_t *pokemon, uint8_t level) {
    const spec_gb_species_data_t *species_data = species_data_of(pokemon);
    if (species_data == nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "species has no species data");
    }
    if (level == 0 || level >= SPEC_LEVEL_COUNT) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "level is not 1 to 100");
    }
    pokemon->experience = spec_experience[species_data->growth_rate][level];
    pokemon->box_level = level;
    return spec_gb_pokemon_calculate_stats(pokemon);
}

spec_error_t spec_gb_pokemon_set_nickname(spec_gb_pokemon_t *pokemon, const char8_t *nickname,
                                          spec_language_t language) {
    return spec_gb_pokemon_set_nickname_ext(pokemon, nickname, 0, language);
}

// As the bag leaves a chosen item's name in the text buffer (pret CopyToStringBuffer): up to its
// terminator, which a long name puts past the field.
static spec_error_t write_item_name(uint8_t *name, size_t name_size, uint16_t item,
                                    spec_language_t language) {
    const char *item_name = spec_gb_item_name(item, language);
    if (item_name == nullptr) {
        return spec_fail(SPEC_ERROR_INVALID_ITEM, "the item has no name in that language");
    }
    uint8_t encoded[SPEC_GB_ITEM_NAME_SIZE];
    spec_error_t error =
        spec_gb_text_from_utf8(encoded, sizeof encoded, (const char8_t *)item_name, language);
    if (error != SPEC_OK) {
        return error;
    }
    size_t written_size = text_length(encoded, sizeof encoded) + 1;
    if (written_size > name_size) {
        written_size = name_size;
    }
    memcpy(name, encoded, written_size);
    return SPEC_OK;
}

// The naming keyboard types '.' as the decimal point, where species names use the period.
static void type_periods_as_decimal_points(uint8_t *text, size_t text_size) {
    for (size_t index = 0; index < text_size; ++index) {
        if (text[index] == PERIOD) {
            text[index] = DECIMAL_POINT;
        }
    }
}

// Each letter typed puts a terminator after it, and naming copies the whole text buffer (pret
// DisplayNamingScreen), so what the buffer held before shows past the name.
spec_error_t spec_gb_pokemon_set_nickname_ext(spec_gb_pokemon_t *pokemon, const char8_t *nickname,
                                              uint16_t buffered_item, spec_language_t language) {
    uint8_t typed[SPEC_GB_NAME_SIZE] = {};
    size_t name_size = spec_gb_name_size(language);
    spec_error_t error = spec_gb_text_from_utf8(typed, name_size, nickname, language);
    if (error != SPEC_OK) {
        return error;
    }
    if (language != SPEC_LANGUAGE_JAPANESE) {
        type_periods_as_decimal_points(typed, name_size);
    }
    if (buffered_item != 0) {
        error = write_item_name(pokemon->nickname, name_size, buffered_item, language);
        if (error != SPEC_OK) {
            return error;
        }
    }
    memcpy(pokemon->nickname, typed, text_length(typed, name_size) + 1);
    return SPEC_OK;
}

// As evolving does (pret evos_moves.asm, SetPartyMonTypes). Every Gen 1 game types a species alike.
spec_error_t spec_gb_pokemon_set_species(spec_gb_pokemon_t *pokemon, spec_gb_species_t species) {
    const spec_gb_species_data_t *species_data =
        spec_gb_get_species_data(SPEC_GAME_TYPE_RED_BLUE, species);
    if (species_data == nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "species is no Gen 1 species");
    }
    pokemon->species = species;
    memcpy(pokemon->types, species_data->types, sizeof pokemon->types);
    return SPEC_OK;
}

static bool is_gen1_game(spec_game_type_t type) {
    return type == SPEC_GAME_TYPE_RED_BLUE || type == SPEC_GAME_TYPE_YELLOW;
}

// Game types count up generation by generation, so the latest row begun by the type is in force.
const spec_gb_species_data_t *spec_gb_get_species_data(spec_game_type_t type,
                                                       spec_gb_species_t species) {
    if (!is_gen1_game(type)) {
        return nullptr;
    }
    const spec_gb_species_row_t *latest = nullptr;
    for (size_t index = 0; index < spec_gb_species_row_count; ++index) {
        const spec_gb_species_row_t *row = &spec_gb_species_rows[index];
        bool applies = row->species == species && row->from_game <= type;
        if (applies && (latest == nullptr || row->from_game >= latest->from_game)) {
            latest = row;
        }
    }
    if (latest == nullptr) {
        return nullptr;
    }
    return &latest->data;
}

spec_gb_species_t spec_gb_species_from_national(uint16_t national_number) {
    if (national_number >= SPEC_GB_POKEDEX_SIZE) {
        return 0;
    }
    return spec_gb_species_of_national[national_number];
}

uint16_t spec_gb_species_to_national(spec_gb_species_t species) {
    if (species >= SPEC_GB_SPECIES_INDEX_COUNT) {
        return 0;
    }
    return spec_gb_national_of_species[species];
}
