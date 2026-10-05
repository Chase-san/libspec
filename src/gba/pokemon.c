// Gen 3 Pokémon records: the encrypted codec, stats, names and species numbering.

#include <string.h>

#include "gba/gba.h"
#include "gba/gba_internal.h"
#include "gba/tables.h"
#include "spec_internal.h"

constexpr size_t PERSONALITY_OFFSET = 0x00;
constexpr size_t TRAINER_ID_OFFSET = 0x04;
constexpr size_t SECRET_ID_OFFSET = 0x06;
constexpr size_t NICKNAME_OFFSET = 0x08;
constexpr size_t LANGUAGE_OFFSET = 0x12;
constexpr size_t FLAGS_OFFSET = 0x13;
constexpr size_t TRAINER_NAME_OFFSET = 0x14;
constexpr size_t MARKINGS_OFFSET = 0x1B;
constexpr size_t CHECKSUM_OFFSET = 0x1C;
constexpr size_t SUBSTRUCTS_OFFSET = 0x20;
constexpr size_t SPECIES_OFFSET = 0x20;
constexpr size_t HELD_ITEM_OFFSET = 0x22;
constexpr size_t EXPERIENCE_OFFSET = 0x24;
constexpr size_t PP_UPS_OFFSET = 0x28;
constexpr size_t FRIENDSHIP_OFFSET = 0x29;
constexpr size_t MOVES_OFFSET = 0x2C;
constexpr size_t MOVE_PP_OFFSET = 0x34;
constexpr size_t EVS_OFFSET = 0x38;
constexpr size_t CONTEST_STATS_OFFSET = 0x3E;
constexpr size_t SHEEN_OFFSET = 0x43;
constexpr size_t POKERUS_OFFSET = 0x44;
constexpr size_t MET_LOCATION_OFFSET = 0x45;
constexpr size_t ORIGIN_OFFSET = 0x46;
constexpr size_t IVS_OFFSET = 0x48;
constexpr size_t RIBBONS_OFFSET = 0x4C;
constexpr size_t STATUS_OFFSET = 0x50;
constexpr size_t LEVEL_OFFSET = 0x54;
constexpr size_t MAIL_ID_OFFSET = 0x55;
constexpr size_t CURRENT_HP_OFFSET = 0x56;
constexpr size_t STATS_OFFSET = 0x58;

constexpr unsigned BAD_EGG_BIT = 0;
constexpr unsigned HAS_SPECIES_BIT = 1;
constexpr unsigned EGG_NAME_BIT = 2;
constexpr unsigned BOX_RS_BLOCKED_BIT = 3;

constexpr unsigned MARKINGS_BIT_COUNT = 4;
constexpr unsigned PP_UP_BIT_COUNT = 2;

constexpr unsigned POKERUS_DAYS_BIT = 0;
constexpr unsigned POKERUS_STRAIN_BIT = 4;
constexpr unsigned POKERUS_BIT_COUNT = 4;

constexpr unsigned MET_LEVEL_BIT = 0;
constexpr unsigned MET_LEVEL_BIT_COUNT = 7;
constexpr unsigned VERSION_BIT = 7;
constexpr unsigned VERSION_BIT_COUNT = 4;
constexpr unsigned BALL_BIT = 11;
constexpr unsigned BALL_BIT_COUNT = 4;
constexpr unsigned TRAINER_FEMALE_BIT = 15;

constexpr unsigned IV_BIT_COUNT = 5;
constexpr unsigned IS_EGG_BIT = 30;
constexpr unsigned ABILITY_NUMBER_BIT = 31;

constexpr unsigned CONTEST_RANK_BIT_COUNT = 3;
constexpr unsigned RIBBONS_BIT = 15;
constexpr unsigned RIBBONS_BIT_COUNT = 12;
constexpr unsigned FATEFUL_ENCOUNTER_BIT = 31;

constexpr unsigned SLEEP_TURNS_BIT = 0;
constexpr unsigned SLEEP_TURNS_BIT_COUNT = 3;
constexpr unsigned POISONED_BIT = 3;
constexpr unsigned BURNED_BIT = 4;
constexpr unsigned FROZEN_BIT = 5;
constexpr unsigned PARALYZED_BIT = 6;
constexpr unsigned BADLY_POISONED_BIT = 7;
constexpr unsigned TOXIC_TURNS_BIT = 8;
constexpr unsigned TOXIC_TURNS_BIT_COUNT = 4;

constexpr size_t SUBSTRUCT_SIZE = 12;

constexpr uint16_t SHEDINJA = 303;
constexpr uint16_t FIRST_MAIL_ITEM = 121;
constexpr uint16_t LAST_MAIL_ITEM = 132;
constexpr uint8_t NO_MAIL = 0xFF;

static bool get_flag(uint32_t word, unsigned bit) {
    return spec_get_bits(word, bit, 1) != 0;
}

static uint32_t set_flag(uint32_t word, unsigned bit, bool is_set) {
    return spec_set_bits(word, bit, 1, is_set);
}

static void xor_substructs(uint8_t *record) {
    uint32_t key = spec_read_u32_le(&record[PERSONALITY_OFFSET])
                   ^ spec_read_u32_le(&record[TRAINER_ID_OFFSET]);
    for (size_t offset = SUBSTRUCTS_OFFSET; offset < SPEC_GBA_BOX_RECORD_SIZE; offset += 4) {
        spec_write_u32_le(&record[offset], spec_read_u32_le(&record[offset]) ^ key);
    }
}

// As the game orders the substructs: personality % 24.
static size_t substruct_order_of(const uint8_t *record) {
    return spec_read_u32_le(&record[PERSONALITY_OFFSET]);
}

static uint16_t checksum_of(const uint8_t *plain) {
    uint16_t checksum = 0;
    for (size_t offset = SUBSTRUCTS_OFFSET; offset < SPEC_GBA_BOX_RECORD_SIZE; offset += 2) {
        checksum = (uint16_t)(checksum + spec_read_u16_le(&plain[offset]));
    }
    return checksum;
}

// hasSpecies and the egg-name bit are derived on write.
static void decode_header(spec_gba_pokemon_t *pokemon, const uint8_t *plain) {
    uint8_t flags = plain[FLAGS_OFFSET];
    pokemon->personality.pid = spec_read_u32_le(&plain[PERSONALITY_OFFSET]);
    pokemon->trainer.id = spec_read_u16_le(&plain[TRAINER_ID_OFFSET]);
    pokemon->trainer.secret_id = spec_read_u16_le(&plain[SECRET_ID_OFFSET]);
    memcpy(pokemon->nickname, &plain[NICKNAME_OFFSET], SPEC_GBA_NICKNAME_SIZE);
    pokemon->language = (spec_language_t)plain[LANGUAGE_OFFSET];
    pokemon->is_bad_egg = get_flag(flags, BAD_EGG_BIT);
    pokemon->is_box_rs_blocked = get_flag(flags, BOX_RS_BLOCKED_BIT);
    memcpy(pokemon->trainer.name, &plain[TRAINER_NAME_OFFSET], SPEC_GBA_TRAINER_NAME_SIZE);
    pokemon->markings = (uint8_t)spec_get_bits(plain[MARKINGS_OFFSET], 0, MARKINGS_BIT_COUNT);
}

static void decode_growth(spec_gba_pokemon_t *pokemon, const uint8_t *plain) {
    pokemon->species = spec_read_u16_le(&plain[SPECIES_OFFSET]);
    pokemon->held_item = spec_read_u16_le(&plain[HELD_ITEM_OFFSET]);
    pokemon->experience = spec_read_u32_le(&plain[EXPERIENCE_OFFSET]);
    for (unsigned move = 0; move < SPEC_GBA_MOVE_COUNT; ++move) {
        pokemon->moves[move].pp_ups =
            (uint8_t)spec_get_bits(plain[PP_UPS_OFFSET], move * PP_UP_BIT_COUNT, PP_UP_BIT_COUNT);
    }
    pokemon->friendship = plain[FRIENDSHIP_OFFSET];
}

static void decode_attacks(spec_gba_pokemon_t *pokemon, const uint8_t *plain) {
    for (size_t move = 0; move < SPEC_GBA_MOVE_COUNT; ++move) {
        pokemon->moves[move].id = spec_read_u16_le(&plain[MOVES_OFFSET + move * 2]);
        pokemon->moves[move].pp = plain[MOVE_PP_OFFSET + move];
    }
}

static void decode_condition(spec_gba_pokemon_t *pokemon, const uint8_t *plain) {
    memcpy(pokemon->evs, &plain[EVS_OFFSET], SPEC_STAT_COUNT);
    memcpy(pokemon->contest.stats, &plain[CONTEST_STATS_OFFSET], SPEC_GBA_CONTEST_CATEGORY_COUNT);
    pokemon->contest.sheen = plain[SHEEN_OFFSET];
}

static void decode_misc(spec_gba_pokemon_t *pokemon, const uint8_t *plain) {
    uint8_t pokerus = plain[POKERUS_OFFSET];
    uint16_t origin = spec_read_u16_le(&plain[ORIGIN_OFFSET]);
    uint32_t ivs = spec_read_u32_le(&plain[IVS_OFFSET]);
    uint32_t ribbons = spec_read_u32_le(&plain[RIBBONS_OFFSET]);
    pokemon->pokerus.strain =
        (uint8_t)spec_get_bits(pokerus, POKERUS_STRAIN_BIT, POKERUS_BIT_COUNT);
    pokemon->pokerus.days = (uint8_t)spec_get_bits(pokerus, POKERUS_DAYS_BIT, POKERUS_BIT_COUNT);
    pokemon->origin.met_location = plain[MET_LOCATION_OFFSET];
    pokemon->origin.met_level = (uint8_t)spec_get_bits(origin, MET_LEVEL_BIT, MET_LEVEL_BIT_COUNT);
    pokemon->origin.version = (spec_version_t)spec_get_bits(origin, VERSION_BIT, VERSION_BIT_COUNT);
    pokemon->origin.ball = (spec_ball_t)spec_get_bits(origin, BALL_BIT, BALL_BIT_COUNT);
    pokemon->trainer.is_female = get_flag(origin, TRAINER_FEMALE_BIT);
    for (unsigned stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        pokemon->ivs[stat] = (uint8_t)spec_get_bits(ivs, stat * IV_BIT_COUNT, IV_BIT_COUNT);
    }
    pokemon->is_egg = get_flag(ivs, IS_EGG_BIT);
    pokemon->ability_number = (uint8_t)spec_get_bits(ivs, ABILITY_NUMBER_BIT, 1);
    for (unsigned category = 0; category < SPEC_GBA_CONTEST_CATEGORY_COUNT; ++category) {
        pokemon->contest.ranks[category] = (spec_gba_contest_rank_t)spec_get_bits(
            ribbons, category * CONTEST_RANK_BIT_COUNT, CONTEST_RANK_BIT_COUNT);
    }
    pokemon->ribbons = (uint16_t)spec_get_bits(ribbons, RIBBONS_BIT, RIBBONS_BIT_COUNT);
    pokemon->is_fateful_encounter = get_flag(ribbons, FATEFUL_ENCOUNTER_BIT);
}

static void decode_status(spec_gba_status_t *status, uint32_t word) {
    status->sleep_turns = (uint8_t)spec_get_bits(word, SLEEP_TURNS_BIT, SLEEP_TURNS_BIT_COUNT);
    status->is_poisoned = get_flag(word, POISONED_BIT);
    status->is_burned = get_flag(word, BURNED_BIT);
    status->is_frozen = get_flag(word, FROZEN_BIT);
    status->is_paralyzed = get_flag(word, PARALYZED_BIT);
    status->is_badly_poisoned = get_flag(word, BADLY_POISONED_BIT);
    status->toxic_turns = (uint8_t)spec_get_bits(word, TOXIC_TURNS_BIT, TOXIC_TURNS_BIT_COUNT);
}

static void decode_party_data(spec_gba_party_data_t *party_data, const uint8_t *plain) {
    decode_status(&party_data->status, spec_read_u32_le(&plain[STATUS_OFFSET]));
    party_data->level = plain[LEVEL_OFFSET];
    party_data->mail_id = plain[MAIL_ID_OFFSET];
    party_data->current_hp = spec_read_u16_le(&plain[CURRENT_HP_OFFSET]);
    for (size_t stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        party_data->stats[stat] = spec_read_u16_le(&plain[STATS_OFFSET + stat * 2]);
    }
}

static const char *unencodable_field_of(const spec_gba_pokemon_t *pokemon) {
    if (!spec_fits_in_bits(pokemon->markings, MARKINGS_BIT_COUNT)) {
        return "markings do not fit in 4 bits";
    }
    for (size_t move = 0; move < SPEC_GBA_MOVE_COUNT; ++move) {
        if (!spec_fits_in_bits(pokemon->moves[move].pp_ups, PP_UP_BIT_COUNT)) {
            return "a move's pp_ups do not fit in 2 bits";
        }
    }
    if (!spec_fits_in_bits(pokemon->pokerus.strain, POKERUS_BIT_COUNT)) {
        return "pokerus.strain does not fit in 4 bits";
    }
    if (!spec_fits_in_bits(pokemon->pokerus.days, POKERUS_BIT_COUNT)) {
        return "pokerus.days does not fit in 4 bits";
    }
    if (!spec_fits_in_bits(pokemon->origin.met_level, MET_LEVEL_BIT_COUNT)) {
        return "origin.met_level does not fit in 7 bits";
    }
    if (!spec_fits_in_bits(pokemon->origin.version, VERSION_BIT_COUNT)) {
        return "origin.version does not fit in 4 bits";
    }
    if (!spec_fits_in_bits(pokemon->origin.ball, BALL_BIT_COUNT)) {
        return "origin.ball does not fit in 4 bits";
    }
    for (size_t stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        if (!spec_fits_in_bits(pokemon->ivs[stat], IV_BIT_COUNT)) {
            return "ivs do not fit in 5 bits";
        }
    }
    if (!spec_fits_in_bits(pokemon->ability_number, 1)) {
        return "ability_number does not fit in 1 bit";
    }
    for (size_t category = 0; category < SPEC_GBA_CONTEST_CATEGORY_COUNT; ++category) {
        if (!spec_fits_in_bits(pokemon->contest.ranks[category], CONTEST_RANK_BIT_COUNT)) {
            return "contest.ranks do not fit in 3 bits";
        }
    }
    if (!spec_fits_in_bits(pokemon->ribbons, RIBBONS_BIT_COUNT)) {
        return "ribbons do not fit in 12 bits";
    }
    if (!spec_fits_in_bits(pokemon->party_data.status.sleep_turns, SLEEP_TURNS_BIT_COUNT)) {
        return "party_data.status.sleep_turns does not fit in 3 bits";
    }
    if (!spec_fits_in_bits(pokemon->party_data.status.toxic_turns, TOXIC_TURNS_BIT_COUNT)) {
        return "party_data.status.toxic_turns does not fit in 4 bits";
    }
    return nullptr;
}

static void encode_header(uint8_t *plain, const spec_gba_pokemon_t *pokemon) {
    uint32_t flags = 0;
    flags = set_flag(flags, BAD_EGG_BIT, pokemon->is_bad_egg);
    flags = set_flag(flags, HAS_SPECIES_BIT, pokemon->species != 0);
    flags = set_flag(flags, EGG_NAME_BIT, pokemon->is_egg);
    flags = set_flag(flags, BOX_RS_BLOCKED_BIT, pokemon->is_box_rs_blocked);
    spec_write_u32_le(&plain[PERSONALITY_OFFSET], pokemon->personality.pid);
    spec_write_u16_le(&plain[TRAINER_ID_OFFSET], pokemon->trainer.id);
    spec_write_u16_le(&plain[SECRET_ID_OFFSET], pokemon->trainer.secret_id);
    memcpy(&plain[NICKNAME_OFFSET], pokemon->nickname, SPEC_GBA_NICKNAME_SIZE);
    plain[LANGUAGE_OFFSET] = pokemon->language;
    plain[FLAGS_OFFSET] = (uint8_t)flags;
    memcpy(&plain[TRAINER_NAME_OFFSET], pokemon->trainer.name, SPEC_GBA_TRAINER_NAME_SIZE);
    plain[MARKINGS_OFFSET] = pokemon->markings;
}

static void encode_growth(uint8_t *plain, const spec_gba_pokemon_t *pokemon) {
    uint32_t pp_ups = 0;
    for (unsigned move = 0; move < SPEC_GBA_MOVE_COUNT; ++move) {
        pp_ups = spec_set_bits(pp_ups, move * PP_UP_BIT_COUNT, PP_UP_BIT_COUNT,
                               pokemon->moves[move].pp_ups);
    }
    spec_write_u16_le(&plain[SPECIES_OFFSET], pokemon->species);
    spec_write_u16_le(&plain[HELD_ITEM_OFFSET], pokemon->held_item);
    spec_write_u32_le(&plain[EXPERIENCE_OFFSET], pokemon->experience);
    plain[PP_UPS_OFFSET] = (uint8_t)pp_ups;
    plain[FRIENDSHIP_OFFSET] = pokemon->friendship;
}

static void encode_attacks(uint8_t *plain, const spec_gba_pokemon_t *pokemon) {
    for (size_t move = 0; move < SPEC_GBA_MOVE_COUNT; ++move) {
        spec_write_u16_le(&plain[MOVES_OFFSET + move * 2], pokemon->moves[move].id);
        plain[MOVE_PP_OFFSET + move] = pokemon->moves[move].pp;
    }
}

static void encode_condition(uint8_t *plain, const spec_gba_pokemon_t *pokemon) {
    memcpy(&plain[EVS_OFFSET], pokemon->evs, SPEC_STAT_COUNT);
    memcpy(&plain[CONTEST_STATS_OFFSET], pokemon->contest.stats, SPEC_GBA_CONTEST_CATEGORY_COUNT);
    plain[SHEEN_OFFSET] = pokemon->contest.sheen;
}

static void encode_misc(uint8_t *plain, const spec_gba_pokemon_t *pokemon) {
    uint32_t pokerus = 0;
    pokerus =
        spec_set_bits(pokerus, POKERUS_STRAIN_BIT, POKERUS_BIT_COUNT, pokemon->pokerus.strain);
    pokerus = spec_set_bits(pokerus, POKERUS_DAYS_BIT, POKERUS_BIT_COUNT, pokemon->pokerus.days);
    uint32_t origin = 0;
    origin = spec_set_bits(origin, MET_LEVEL_BIT, MET_LEVEL_BIT_COUNT, pokemon->origin.met_level);
    origin = spec_set_bits(origin, VERSION_BIT, VERSION_BIT_COUNT, pokemon->origin.version);
    origin = spec_set_bits(origin, BALL_BIT, BALL_BIT_COUNT, pokemon->origin.ball);
    origin = set_flag(origin, TRAINER_FEMALE_BIT, pokemon->trainer.is_female);
    uint32_t ivs = 0;
    for (unsigned stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        ivs = spec_set_bits(ivs, stat * IV_BIT_COUNT, IV_BIT_COUNT, pokemon->ivs[stat]);
    }
    ivs = set_flag(ivs, IS_EGG_BIT, pokemon->is_egg);
    ivs = spec_set_bits(ivs, ABILITY_NUMBER_BIT, 1, pokemon->ability_number);
    uint32_t ribbons = 0;
    for (unsigned category = 0; category < SPEC_GBA_CONTEST_CATEGORY_COUNT; ++category) {
        ribbons = spec_set_bits(ribbons, category * CONTEST_RANK_BIT_COUNT, CONTEST_RANK_BIT_COUNT,
                                pokemon->contest.ranks[category]);
    }
    ribbons = spec_set_bits(ribbons, RIBBONS_BIT, RIBBONS_BIT_COUNT, pokemon->ribbons);
    ribbons = set_flag(ribbons, FATEFUL_ENCOUNTER_BIT, pokemon->is_fateful_encounter);
    plain[POKERUS_OFFSET] = (uint8_t)pokerus;
    plain[MET_LOCATION_OFFSET] = pokemon->origin.met_location;
    spec_write_u16_le(&plain[ORIGIN_OFFSET], (uint16_t)origin);
    spec_write_u32_le(&plain[IVS_OFFSET], ivs);
    spec_write_u32_le(&plain[RIBBONS_OFFSET], ribbons);
}

static uint32_t encode_status(const spec_gba_status_t *status) {
    uint32_t word = 0;
    word = spec_set_bits(word, SLEEP_TURNS_BIT, SLEEP_TURNS_BIT_COUNT, status->sleep_turns);
    word = set_flag(word, POISONED_BIT, status->is_poisoned);
    word = set_flag(word, BURNED_BIT, status->is_burned);
    word = set_flag(word, FROZEN_BIT, status->is_frozen);
    word = set_flag(word, PARALYZED_BIT, status->is_paralyzed);
    word = set_flag(word, BADLY_POISONED_BIT, status->is_badly_poisoned);
    word = spec_set_bits(word, TOXIC_TURNS_BIT, TOXIC_TURNS_BIT_COUNT, status->toxic_turns);
    return word;
}

static void encode_party_data(uint8_t *plain, const spec_gba_party_data_t *party_data) {
    spec_write_u32_le(&plain[STATUS_OFFSET], encode_status(&party_data->status));
    plain[LEVEL_OFFSET] = party_data->level;
    plain[MAIL_ID_OFFSET] = party_data->mail_id;
    spec_write_u16_le(&plain[CURRENT_HP_OFFSET], party_data->current_hp);
    for (size_t stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        spec_write_u16_le(&plain[STATS_OFFSET + stat * 2], party_data->stats[stat]);
    }
}

void spec_gba_decode_pokemon(spec_gba_pokemon_t *pokemon, const uint8_t *record,
                             size_t record_size) {
    uint8_t plain[SPEC_GBA_PARTY_RECORD_SIZE] = {};
    memcpy(plain, record, record_size);
    xor_substructs(plain);
    spec_unshuffle_blocks(&plain[SUBSTRUCTS_OFFSET], SUBSTRUCT_SIZE, substruct_order_of(plain));
    *pokemon = (spec_gba_pokemon_t){};
    decode_header(pokemon, plain);
    decode_growth(pokemon, plain);
    decode_attacks(pokemon, plain);
    decode_condition(pokemon, plain);
    decode_misc(pokemon, plain);
    if (record_size == SPEC_GBA_PARTY_RECORD_SIZE) {
        decode_party_data(&pokemon->party_data, plain);
    }
    // A failed checksum reads as a Bad Egg, as in the game.
    if (checksum_of(plain) != spec_read_u16_le(&plain[CHECKSUM_OFFSET])) {
        pokemon->is_bad_egg = true;
        pokemon->is_egg = true;
    }
    if (pokemon->species != 0) {
        pokemon->personality = spec_gba_decode_personality(pokemon->personality.pid,
                                                           pokemon->species, &pokemon->trainer);
        pokemon->iv_method = spec_find_iv_method(pokemon->personality.pid, pokemon->ivs);
    }
}

spec_error_t spec_gba_encode_pokemon(uint8_t *record, size_t record_size,
                                     const spec_gba_pokemon_t *pokemon) {
    const char *unencodable_field = unencodable_field_of(pokemon);
    if (unencodable_field != nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, unencodable_field);
    }
    uint8_t plain[SPEC_GBA_PARTY_RECORD_SIZE] = {};
    encode_header(plain, pokemon);
    encode_growth(plain, pokemon);
    encode_attacks(plain, pokemon);
    encode_condition(plain, pokemon);
    encode_misc(plain, pokemon);
    encode_party_data(plain, &pokemon->party_data);
    spec_write_u16_le(&plain[CHECKSUM_OFFSET], checksum_of(plain));
    spec_shuffle_blocks(&plain[SUBSTRUCTS_OFFSET], SUBSTRUCT_SIZE, substruct_order_of(plain));
    xor_substructs(plain);
    memcpy(record, plain, record_size);
    return SPEC_OK;
}

static bool is_record_size(size_t raw_size) {
    return raw_size == SPEC_GBA_BOX_RECORD_SIZE || raw_size == SPEC_GBA_PARTY_RECORD_SIZE;
}

spec_error_t spec_gba_read_pokemon(spec_gba_pokemon_t *pokemon, const uint8_t *raw,
                                   size_t raw_size) {
    if (!is_record_size(raw_size)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "raw_size is neither 80 nor 100");
    }
    spec_gba_decode_pokemon(pokemon, raw, raw_size);
    return SPEC_OK;
}

spec_error_t spec_gba_write_pokemon(uint8_t *raw, size_t raw_size,
                                    const spec_gba_pokemon_t *pokemon) {
    if (!is_record_size(raw_size)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "raw_size is neither 80 nor 100");
    }
    return spec_gba_encode_pokemon(raw, raw_size, pokemon);
}

static uint16_t stat_of(const spec_gba_pokemon_t *pokemon,
                        const spec_gba_species_data_t *species_data, spec_stat_t stat,
                        uint8_t level) {
    if (stat == SPEC_STAT_HP && pokemon->species == SHEDINJA) {
        return 1;
    }
    // The nature comes from the pid, since personality.nature is ignored on write.
    spec_nature_t nature = (spec_nature_t)(pokemon->personality.pid % SPEC_NATURE_COUNT);
    return spec_calculate_stat(stat, species_data->base_stats[stat], pokemon->ivs[stat],
                               pokemon->evs[stat], level, nature);
}

static uint16_t current_hp_after(const spec_gba_pokemon_t *pokemon, uint16_t old_max_hp,
                                 uint16_t new_max_hp) {
    // As CalculateMonStats.
    bool is_fainted = pokemon->party_data.current_hp == 0 && old_max_hp != 0;
    if (is_fainted) {
        return 0;
    }
    if (pokemon->species == SHEDINJA) {
        return 1;
    }
    if (pokemon->party_data.current_hp == 0) {
        return new_max_hp;
    }
    return (uint16_t)(pokemon->party_data.current_hp + new_max_hp - old_max_hp);
}

static void calculate_stats(spec_gba_pokemon_t *pokemon,
                            const spec_gba_species_data_t *species_data) {
    uint8_t level = spec_level_for_experience(species_data->growth_rate, pokemon->experience);
    uint16_t old_max_hp = pokemon->party_data.stats[SPEC_STAT_HP];
    uint16_t new_max_hp = stat_of(pokemon, species_data, SPEC_STAT_HP, level);
    pokemon->party_data.current_hp = current_hp_after(pokemon, old_max_hp, new_max_hp);
    pokemon->party_data.level = level;
    pokemon->party_data.stats[SPEC_STAT_HP] = new_max_hp;
    for (spec_stat_t stat = SPEC_STAT_ATTACK; stat < SPEC_STAT_COUNT; ++stat) {
        pokemon->party_data.stats[stat] = stat_of(pokemon, species_data, stat, level);
    }
}

spec_error_t spec_gba_pokemon_calculate_stats(spec_gba_pokemon_t *pokemon) {
    if (spec_gba_species_to_national(pokemon->species) == 0) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "species has no species data");
    }
    calculate_stats(pokemon, &spec_gba_species_data[pokemon->species]);
    return SPEC_OK;
}

void spec_gba_fill_party_data(spec_gba_pokemon_t *pokemon) {
    bool has_party_data =
        pokemon->party_data.level != 0 || pokemon->party_data.stats[SPEC_STAT_HP] != 0;
    bool has_species_data = spec_gba_species_to_national(pokemon->species) != 0;
    if (has_party_data || !has_species_data) {
        return;
    }
    pokemon->party_data = (spec_gba_party_data_t){.mail_id = NO_MAIL};
    calculate_stats(pokemon, &spec_gba_species_data[pokemon->species]);
}

spec_error_t spec_gba_pokemon_get_name(const spec_gba_pokemon_t *pokemon,
                                       char8_t name[static SPEC_GBA_TEXT_BUFFER_SIZE]) {
    return spec_gba_text_to_utf8(name, pokemon->nickname, SPEC_GBA_NICKNAME_SIZE,
                                 pokemon->language);
}

spec_error_t spec_gba_pokemon_set_name(spec_gba_pokemon_t *pokemon, const char8_t *name) {
    return spec_gba_text_from_utf8(pokemon->nickname, SPEC_GBA_NICKNAME_SIZE, name,
                                   pokemon->language);
}

// The game's PC refuses Pokémon holding mail.
bool spec_gba_is_safe_to_box(const spec_gba_pokemon_t *pokemon) {
    bool is_holding_mail =
        pokemon->held_item >= FIRST_MAIL_ITEM && pokemon->held_item <= LAST_MAIL_ITEM;
    return !is_holding_mail;
}

uint16_t spec_gba_species_to_national(uint16_t species) {
    if (species >= SPEC_GBA_SPECIES_INDEX_COUNT) {
        return 0;
    }
    return spec_gba_national_of_species[species];
}

uint16_t spec_gba_species_from_national(uint16_t national_number) {
    if (national_number >= SPEC_GBA_NATIONAL_COUNT) {
        return 0;
    }
    return spec_gba_species_of_national[national_number];
}
