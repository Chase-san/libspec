// Gen 6 and 7 Pokémon records, PK6 and PK7: the encrypted codec, stats, names and species data.

#include <string.h>

#include "3ds/3ds.h"
#include "3ds/3ds_internal.h"
#include "3ds/tables.h"
#include "nds/nds_internal.h"
#include "spec_internal.h"

constexpr size_t ENCRYPTION_CONSTANT_OFFSET = 0x00;
constexpr size_t FLAGS_OFFSET = 0x04;
constexpr size_t CHECKSUM_OFFSET = 0x06;
constexpr size_t BLOCKS_OFFSET = 0x08;
constexpr size_t BLOCK_SIZE = 0x38;
constexpr size_t BLOCKS_SIZE = 4 * BLOCK_SIZE;

constexpr size_t SPECIES_OFFSET = 0x08;
constexpr size_t HELD_ITEM_OFFSET = 0x0A;
constexpr size_t TRAINER_ID_OFFSET = 0x0C;
constexpr size_t SECRET_ID_OFFSET = 0x0E;
constexpr size_t EXPERIENCE_OFFSET = 0x10;
constexpr size_t ABILITY_OFFSET = 0x14;
constexpr size_t ABILITY_NUMBER_OFFSET = 0x15;
constexpr size_t TRAINING_BAG_HITS_OFFSET = 0x16; // Gen 6
constexpr size_t TRAINING_BAG_OFFSET = 0x17;      // Gen 6
constexpr size_t GEN7_MARKINGS_OFFSET = 0x16;
constexpr size_t PID_OFFSET = 0x18;
constexpr size_t NATURE_OFFSET = 0x1C;
constexpr size_t FORM_OFFSET = 0x1D;
constexpr size_t EVS_OFFSET = 0x1E;
constexpr size_t CONTEST_STATS_OFFSET = 0x24;
constexpr size_t SHEEN_OFFSET = 0x29;
constexpr size_t GEN6_MARKINGS_OFFSET = 0x2A;
constexpr size_t RESORT_EVENT_STATUS_OFFSET = 0x2A; // Gen 7
constexpr size_t POKERUS_OFFSET = 0x2B;
constexpr size_t REGIMENS_OFFSET = 0x2C;
constexpr size_t RIBBONS_OFFSET = 0x30;
constexpr size_t RIBBON_BYTE_COUNT = 7;
constexpr size_t CONTEST_MEMORY_OFFSET = 0x38;
constexpr size_t BATTLE_MEMORY_OFFSET = 0x39;
constexpr size_t DISTRIBUTION_REGIMENS_OFFSET = 0x3A;
constexpr size_t FORM_ARGUMENT_OFFSET = 0x3C;

constexpr size_t NICKNAME_OFFSET = 0x40;
constexpr size_t MOVES_OFFSET = 0x5A;
constexpr size_t MOVE_PP_OFFSET = 0x62;
constexpr size_t PP_UPS_OFFSET = 0x66;
constexpr size_t RELEARN_MOVES_OFFSET = 0x6A;
constexpr size_t SECRET_TRAINING_OFFSET = 0x72;
constexpr size_t IVS_OFFSET = 0x74;

constexpr size_t LATEST_TRAINER_NAME_OFFSET = 0x78;
constexpr size_t LATEST_TRAINER_GENDER_OFFSET = 0x92;
constexpr size_t CURRENT_TRAINER_OFFSET = 0x93;
constexpr size_t GEO_MEMORIES_OFFSET = 0x94;
constexpr size_t LATEST_TRAINER_BOND_OFFSET = 0xA2;
constexpr size_t LATEST_TRAINER_MEMORY_VARIABLE_OFFSET = 0xA8;
constexpr size_t FULLNESS_OFFSET = 0xAE;
constexpr size_t ENJOYMENT_OFFSET = 0xAF;

constexpr size_t TRAINER_NAME_OFFSET = 0xB0;
constexpr size_t TRAINER_BOND_OFFSET = 0xCA;
constexpr size_t TRAINER_MEMORY_VARIABLE_OFFSET = 0xCE;
constexpr size_t TRAINER_FEELING_OFFSET = 0xD0;
constexpr size_t EGG_DATE_OFFSET = 0xD1;
constexpr size_t MET_DATE_OFFSET = 0xD4;
constexpr size_t EGG_LOCATION_OFFSET = 0xD8;
constexpr size_t MET_LOCATION_OFFSET = 0xDA;
constexpr size_t BALL_OFFSET = 0xDC;
constexpr size_t MET_LEVEL_OFFSET = 0xDD;
constexpr size_t ENCOUNTER_TYPE_OFFSET = 0xDE; // Gen 6
constexpr size_t HYPER_TRAINING_OFFSET = 0xDE; // Gen 7
constexpr size_t VERSION_OFFSET = 0xDF;
constexpr size_t COUNTRY_OFFSET = 0xE0;
constexpr size_t SUBREGION_OFFSET = 0xE1;
constexpr size_t CONSOLE_REGION_OFFSET = 0xE2;
constexpr size_t LANGUAGE_OFFSET = 0xE3;

constexpr size_t PARTY_DATA_OFFSET = 0xE8;
constexpr size_t PARTY_DATA_SIZE = SPEC_3DS_PARTY_RECORD_SIZE - PARTY_DATA_OFFSET;
constexpr size_t STATUS_OFFSET = 0xE8;
constexpr size_t LEVEL_OFFSET = 0xEC;
constexpr size_t FORM_DAYS_REMAINING_OFFSET = 0xED; // Gen 6
constexpr size_t FORM_DAYS_ELAPSED_OFFSET = 0xEE;   // Gen 6
constexpr size_t TRAINING_BAG_EFFECT_OFFSET = 0xEF; // Gen 6
constexpr size_t DIRT_TYPE_OFFSET = 0xED;           // Gen 7
constexpr size_t DIRT_LOCATION_OFFSET = 0xEE;       // Gen 7
constexpr size_t CURRENT_HP_OFFSET = 0xF0;
constexpr size_t STATS_OFFSET = 0xF2;

constexpr unsigned BAD_EGG_BIT = 2;
constexpr unsigned GEN7_ABILITY_NUMBER_BIT_COUNT = 3;
constexpr unsigned GEN6_MARKING_BIT_COUNT = 1;
constexpr unsigned GEN7_MARKING_BIT_COUNT = 2;
constexpr uint8_t GEN7_MARKING_MAX = 2;
constexpr unsigned GEN6_RIBBON_BIT_COUNT = 46;
constexpr unsigned GEN7_RIBBON_BIT_COUNT = 50;

constexpr unsigned FATEFUL_ENCOUNTER_BIT = 0;
constexpr unsigned GENDER_BIT = 1;
constexpr unsigned GENDER_BIT_COUNT = 2;
constexpr unsigned FORM_BIT = 3;
constexpr unsigned FORM_BIT_COUNT = 5;

constexpr unsigned POKERUS_DAYS_BIT = 0;
constexpr unsigned POKERUS_STRAIN_BIT = 4;
constexpr unsigned POKERUS_BIT_COUNT = 4;
constexpr unsigned SECRET_UNLOCKED_BIT = 0;
constexpr unsigned TRAINING_COMPLETE_BIT = 1;

constexpr unsigned IV_BIT_COUNT = 5;
constexpr unsigned IS_EGG_BIT = 30;
constexpr unsigned IS_NICKNAMED_BIT = 31;
constexpr uint8_t HYPER_TRAINED_IV = 31;
// Hyper Training's bits run HP, Attack, Defense, Special Attack, Special Defense, Speed.
constexpr unsigned HYPER_TRAINING_BITS[SPEC_STAT_COUNT] = {
    [SPEC_STAT_HP] = 0,
    [SPEC_STAT_ATTACK] = 1,
    [SPEC_STAT_DEFENSE] = 2,
    [SPEC_STAT_SPECIAL_ATTACK] = 3,
    [SPEC_STAT_SPECIAL_DEFENSE] = 4,
    [SPEC_STAT_SPEED] = 5,
};

constexpr unsigned MET_LEVEL_BIT_COUNT = 7;
constexpr unsigned TRAINER_FEMALE_BIT = 7;
constexpr unsigned SLEEP_TURNS_BIT_COUNT = 3;
constexpr unsigned TOXIC_TURNS_BIT_COUNT = 4;

constexpr uint16_t GEN6_SPECIES_COUNT = 721;
constexpr uint16_t GEN7_SPECIES_COUNT = 807;
constexpr uint16_t SHEDINJA = 292;
constexpr uint16_t END_OF_TEXT = 0x0000;
// Line 0 of each language's species-name list on the carts (X and Y's member 80, Sun and Moon's 55,
// Ultra Sun and Ultra Moon's 60); Chinese is Gen 7's glyphs, with the species names.
static const char8_t *const EGG_NAMES[SPEC_NAME_LANGUAGE_COUNT] = {
    [SPEC_LANGUAGE_JAPANESE] = u8"タマゴ", [SPEC_LANGUAGE_ENGLISH] = u8"Egg",
    [SPEC_LANGUAGE_FRENCH] = u8"Œuf",      [SPEC_LANGUAGE_ITALIAN] = u8"Uovo",
    [SPEC_LANGUAGE_GERMAN] = u8"Ei",       [SPEC_LANGUAGE_SPANISH] = u8"Huevo",
    [SPEC_LANGUAGE_KOREAN] = u8"알",
};
constexpr uint16_t EGG_SPECIES = 0;

static bool is_gen7(const spec_3ds_pokemon_t *pokemon) {
    return pokemon->generation == 7;
}

static bool is_record_size(size_t raw_size) {
    return raw_size == SPEC_3DS_BOX_RECORD_SIZE || raw_size == SPEC_3DS_PARTY_RECORD_SIZE;
}

static bool has_species_data(uint8_t generation, uint16_t species) {
    uint16_t species_count = generation == 7 ? GEN7_SPECIES_COUNT : GEN6_SPECIES_COUNT;
    return (generation == 6 || generation == 7) && species != 0 && species <= species_count;
}

// A record's generation picks the data: Omega Ruby and Alpha Sapphire have every Gen 6 species and
// form, and Ultra Sun and Ultra Moon every Gen 7 one.
static const spec_3ds_species_data_t *species_data_of(uint8_t generation, uint16_t species,
                                                      uint8_t form) {
    if (generation == 7) {
        return spec_3ds_get_species_data(SPEC_GAME_TYPE_ULTRA_SUN_ULTRA_MOON, species, form);
    }
    return spec_3ds_get_species_data(SPEC_GAME_TYPE_OMEGA_RUBY_ALPHA_SAPPHIRE, species, form);
}

static uint16_t stat_of(const spec_3ds_pokemon_t *pokemon, const uint8_t *base_stats,
                        spec_stat_t stat, uint8_t level) {
    if (stat == SPEC_STAT_HP && pokemon->species == SHEDINJA) {
        return 1;
    }
    uint8_t iv = pokemon->hyper_trained[stat] ? HYPER_TRAINED_IV : pokemon->ivs[stat];
    return spec_calculate_stat(stat, base_stats[stat], iv, pokemon->evs[stat], level,
                               pokemon->nature);
}

// TODO: Check Gen 6 and 7's HP after a recalculation; this is Gen 4 and 5's.
static void calculate_stats(spec_3ds_pokemon_t *pokemon) {
    const spec_3ds_species_data_t *species_data =
        species_data_of(pokemon->generation, pokemon->species, pokemon->form);
    uint8_t level = spec_level_for_experience(species_data->growth_rate, pokemon->experience);
    uint16_t old_max_hp = pokemon->party_data.stats[SPEC_STAT_HP];
    uint16_t new_max_hp = stat_of(pokemon, species_data->base_stats, SPEC_STAT_HP, level);
    pokemon->party_data.current_hp = spec_nds_current_hp_after(
        pokemon->party_data.current_hp, old_max_hp, new_max_hp, pokemon->species == SHEDINJA);
    pokemon->party_data.level = level;
    pokemon->party_data.stats[SPEC_STAT_HP] = new_max_hp;
    for (spec_stat_t stat = SPEC_STAT_ATTACK; stat < SPEC_STAT_COUNT; ++stat) {
        pokemon->party_data.stats[stat] = stat_of(pokemon, species_data->base_stats, stat, level);
    }
}

static spec_error_t check_stat_inputs(const spec_3ds_pokemon_t *pokemon) {
    if (!has_species_data(pokemon->generation, pokemon->species)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "species has no species data");
    }
    if (pokemon->nature >= SPEC_NATURE_COUNT) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "nature is not one of the 25");
    }
    return SPEC_OK;
}

// The species name as Gen 6 and 7 store it: Chinese in Gen 7's glyphs, the rest as shown.
static spec_error_t encode_species_name(uint16_t name[static SPEC_3DS_NAME_SIZE], uint16_t species,
                                        spec_language_t language) {
    memset(name, 0, SPEC_3DS_NAME_SIZE * sizeof name[0]);
    bool is_chinese = language == SPEC_LANGUAGE_CHINESE_SIMPLIFIED
                      || language == SPEC_LANGUAGE_CHINESE_TRADITIONAL;
    if (is_chinese && species < SPEC_3DS_SPECIES_COUNT) {
        size_t script = language - SPEC_LANGUAGE_CHINESE_SIMPLIFIED;
        memcpy(name, spec_3ds_chinese_species_names[script][species],
               sizeof spec_3ds_chinese_species_names[script][species]);
        return SPEC_OK;
    }
    const char *utf8 = species == EGG_SPECIES ? (const char *)EGG_NAMES[language]
                                              : spec_species_name(species, language);
    if (utf8 == nullptr) {
        return spec_fail(SPEC_ERROR_UNKNOWN_NAME, "the species has no name in that language");
    }
    return spec_3ds_text_from_utf8(name, SPEC_3DS_NAME_SIZE, (const char8_t *)utf8, language);
}

static size_t block_order_of(uint32_t encryption_constant) {
    return (encryption_constant >> 13) & 31;
}

static spec_3ds_bond_t bond_at(const uint8_t *plain, size_t bond_offset, size_t variable_offset) {
    return (spec_3ds_bond_t){
        .friendship = plain[bond_offset],
        .affection = plain[bond_offset + 1],
        .memory =
            {
                .intensity = plain[bond_offset + 2],
                .memory = plain[bond_offset + 3],
                .variable = spec_read_u16_le(&plain[variable_offset]),
            },
    };
}

static void write_bond(uint8_t *plain, size_t bond_offset, size_t variable_offset,
                       const spec_3ds_bond_t *bond) {
    plain[bond_offset] = bond->friendship;
    plain[bond_offset + 1] = bond->affection;
    plain[bond_offset + 2] = bond->memory.intensity;
    plain[bond_offset + 3] = bond->memory.memory;
    spec_write_u16_le(&plain[variable_offset], bond->memory.variable);
}

static void decode_header(spec_3ds_pokemon_t *pokemon, const uint8_t *plain) {
    pokemon->encryption_constant = spec_read_u32_le(&plain[ENCRYPTION_CONSTANT_OFFSET]);
    pokemon->is_bad_egg = spec_get_flag(spec_read_u16_le(&plain[FLAGS_OFFSET]), BAD_EGG_BIT);
}

static void decode_markings(spec_3ds_pokemon_t *pokemon, const uint8_t *plain) {
    uint32_t markings = is_gen7(pokemon) ? spec_read_u16_le(&plain[GEN7_MARKINGS_OFFSET])
                                         : plain[GEN6_MARKINGS_OFFSET];
    unsigned bit_count = is_gen7(pokemon) ? GEN7_MARKING_BIT_COUNT : GEN6_MARKING_BIT_COUNT;
    for (unsigned mark = 0; mark < SPEC_3DS_MARKING_COUNT; ++mark) {
        pokemon->markings[mark] = (uint8_t)spec_get_bits(markings, mark * bit_count, bit_count);
    }
}

static uint64_t read_ribbons(const uint8_t *plain) {
    uint64_t ribbons = 0;
    for (size_t byte = 0; byte < RIBBON_BYTE_COUNT; ++byte) {
        ribbons |= (uint64_t)plain[RIBBONS_OFFSET + byte] << (byte * 8);
    }
    return ribbons;
}

static void decode_block_a(spec_3ds_pokemon_t *pokemon, const uint8_t *plain) {
    pokemon->species = spec_read_u16_le(&plain[SPECIES_OFFSET]);
    pokemon->held_item = spec_read_u16_le(&plain[HELD_ITEM_OFFSET]);
    pokemon->trainer.id = spec_read_u16_le(&plain[TRAINER_ID_OFFSET]);
    pokemon->trainer.secret_id = spec_read_u16_le(&plain[SECRET_ID_OFFSET]);
    pokemon->experience = spec_read_u32_le(&plain[EXPERIENCE_OFFSET]);
    pokemon->ability = plain[ABILITY_OFFSET];
    pokemon->ability_number = plain[ABILITY_NUMBER_OFFSET];
    if (!is_gen7(pokemon)) {
        pokemon->training.bag_hits = plain[TRAINING_BAG_HITS_OFFSET];
        pokemon->training.bag = plain[TRAINING_BAG_OFFSET];
    }
    pokemon->personality.pid = spec_read_u32_le(&plain[PID_OFFSET]);
    pokemon->nature = (spec_nature_t)plain[NATURE_OFFSET];
    pokemon->is_fateful_encounter = spec_get_flag(plain[FORM_OFFSET], FATEFUL_ENCOUNTER_BIT);
    pokemon->gender =
        (spec_gender_t)spec_get_bits(plain[FORM_OFFSET], GENDER_BIT, GENDER_BIT_COUNT);
    pokemon->form = (uint8_t)spec_get_bits(plain[FORM_OFFSET], FORM_BIT, FORM_BIT_COUNT);
    memcpy(pokemon->evs, &plain[EVS_OFFSET], SPEC_STAT_COUNT);
    memcpy(pokemon->contest.stats, &plain[CONTEST_STATS_OFFSET], SPEC_NDS_CONTEST_CATEGORY_COUNT);
    pokemon->contest.sheen = plain[SHEEN_OFFSET];
    decode_markings(pokemon, plain);
    if (is_gen7(pokemon)) {
        pokemon->resort_event_status = plain[RESORT_EVENT_STATUS_OFFSET];
    }
    pokemon->pokerus.strain =
        (uint8_t)spec_get_bits(plain[POKERUS_OFFSET], POKERUS_STRAIN_BIT, POKERUS_BIT_COUNT);
    pokemon->pokerus.days =
        (uint8_t)spec_get_bits(plain[POKERUS_OFFSET], POKERUS_DAYS_BIT, POKERUS_BIT_COUNT);
    pokemon->training.regimens = spec_read_u32_le(&plain[REGIMENS_OFFSET]);
    pokemon->ribbons = read_ribbons(plain);
    pokemon->contest_memory_ribbon_count = plain[CONTEST_MEMORY_OFFSET];
    pokemon->battle_memory_ribbon_count = plain[BATTLE_MEMORY_OFFSET];
    pokemon->training.distribution_regimens =
        spec_read_u16_le(&plain[DISTRIBUTION_REGIMENS_OFFSET]);
    pokemon->form_argument = spec_read_u32_le(&plain[FORM_ARGUMENT_OFFSET]);
}

static void decode_block_b(spec_3ds_pokemon_t *pokemon, const uint8_t *plain) {
    spec_nds_read_text(pokemon->nickname, &plain[NICKNAME_OFFSET], SPEC_3DS_NAME_SIZE);
    for (size_t move = 0; move < SPEC_3DS_MOVE_COUNT; ++move) {
        pokemon->moves[move].id = spec_read_u16_le(&plain[MOVES_OFFSET + move * 2]);
        pokemon->moves[move].pp = plain[MOVE_PP_OFFSET + move];
        pokemon->moves[move].pp_ups = plain[PP_UPS_OFFSET + move];
        pokemon->relearn_moves[move] = spec_read_u16_le(&plain[RELEARN_MOVES_OFFSET + move * 2]);
    }
    pokemon->training.is_secret_unlocked =
        spec_get_flag(plain[SECRET_TRAINING_OFFSET], SECRET_UNLOCKED_BIT);
    pokemon->training.is_complete =
        spec_get_flag(plain[SECRET_TRAINING_OFFSET], TRAINING_COMPLETE_BIT);
    uint32_t ivs = spec_read_u32_le(&plain[IVS_OFFSET]);
    for (unsigned stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        pokemon->ivs[stat] = (uint8_t)spec_get_bits(ivs, stat * IV_BIT_COUNT, IV_BIT_COUNT);
    }
    pokemon->is_egg = spec_get_flag(ivs, IS_EGG_BIT);
    pokemon->is_nicknamed = spec_get_flag(ivs, IS_NICKNAMED_BIT);
}

static void decode_block_c(spec_3ds_pokemon_t *pokemon, const uint8_t *plain) {
    spec_3ds_latest_trainer_t *latest_trainer = &pokemon->latest_trainer;
    spec_nds_read_text(latest_trainer->name, &plain[LATEST_TRAINER_NAME_OFFSET],
                       SPEC_3DS_NAME_SIZE);
    latest_trainer->gender = (spec_gender_t)plain[LATEST_TRAINER_GENDER_OFFSET];
    pokemon->is_with_latest_trainer = plain[CURRENT_TRAINER_OFFSET] != 0;
    for (size_t index = 0; index < SPEC_3DS_GEO_MEMORY_COUNT; ++index) {
        pokemon->geo_memories[index] = (spec_3ds_geo_memory_t){
            .subregion = plain[GEO_MEMORIES_OFFSET + index * 2],
            .country = plain[GEO_MEMORIES_OFFSET + index * 2 + 1],
        };
    }
    latest_trainer->bond =
        bond_at(plain, LATEST_TRAINER_BOND_OFFSET, LATEST_TRAINER_MEMORY_VARIABLE_OFFSET);
    latest_trainer->bond.memory.feeling = plain[LATEST_TRAINER_BOND_OFFSET + 4];
    pokemon->fullness = plain[FULLNESS_OFFSET];
    pokemon->enjoyment = plain[ENJOYMENT_OFFSET];
}

static void decode_block_d(spec_3ds_pokemon_t *pokemon, const uint8_t *plain) {
    spec_nds_read_text(pokemon->trainer.name, &plain[TRAINER_NAME_OFFSET], SPEC_3DS_NAME_SIZE);
    pokemon->trainer.is_female = spec_get_flag(plain[MET_LEVEL_OFFSET], TRAINER_FEMALE_BIT);
    pokemon->trainer_bond = bond_at(plain, TRAINER_BOND_OFFSET, TRAINER_MEMORY_VARIABLE_OFFSET);
    pokemon->trainer_bond.memory.feeling = plain[TRAINER_FEELING_OFFSET];
    if (is_gen7(pokemon)) {
        for (size_t stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
            pokemon->hyper_trained[stat] =
                spec_get_flag(plain[HYPER_TRAINING_OFFSET], HYPER_TRAINING_BITS[stat]);
        }
    }
    pokemon->region = (spec_3ds_region_t){
        .country = plain[COUNTRY_OFFSET],
        .subregion = plain[SUBREGION_OFFSET],
        .console_region = plain[CONSOLE_REGION_OFFSET],
    };
    pokemon->language = (spec_language_t)plain[LANGUAGE_OFFSET];
}

static spec_3ds_date_t date_at(const uint8_t *plain, size_t offset) {
    return (spec_3ds_date_t){
        .year = plain[offset],
        .month = plain[offset + 1],
        .day = plain[offset + 2],
    };
}

static void decode_origin(spec_3ds_pokemon_t *pokemon, const uint8_t *plain) {
    spec_3ds_origin_t *origin = &pokemon->origin;
    origin->version = (spec_version_t)plain[VERSION_OFFSET];
    origin->ball = (spec_ball_t)plain[BALL_OFFSET];
    origin->met_level = (uint8_t)spec_get_bits(plain[MET_LEVEL_OFFSET], 0, MET_LEVEL_BIT_COUNT);
    origin->met_location = spec_read_u16_le(&plain[MET_LOCATION_OFFSET]);
    origin->met_date = date_at(plain, MET_DATE_OFFSET);
    origin->egg_location = spec_read_u16_le(&plain[EGG_LOCATION_OFFSET]);
    origin->egg_date = date_at(plain, EGG_DATE_OFFSET);
    if (!is_gen7(pokemon)) {
        origin->encounter_type = plain[ENCOUNTER_TYPE_OFFSET];
    }
}

static void decode_party_data(spec_3ds_pokemon_t *pokemon, const uint8_t *plain) {
    spec_3ds_party_data_t *party_data = &pokemon->party_data;
    spec_nds_decode_status(&party_data->status, spec_read_u32_le(&plain[STATUS_OFFSET]));
    party_data->level = plain[LEVEL_OFFSET];
    if (is_gen7(pokemon)) {
        party_data->dirt_type = plain[DIRT_TYPE_OFFSET];
        party_data->dirt_location = plain[DIRT_LOCATION_OFFSET];
    } else {
        party_data->form_days_remaining = plain[FORM_DAYS_REMAINING_OFFSET];
        party_data->form_days_elapsed = plain[FORM_DAYS_ELAPSED_OFFSET];
        party_data->training_bag_effect = plain[TRAINING_BAG_EFFECT_OFFSET];
    }
    party_data->current_hp = spec_read_u16_le(&plain[CURRENT_HP_OFFSET]);
    for (size_t stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        party_data->stats[stat] = spec_read_u16_le(&plain[STATS_OFFSET + stat * 2]);
    }
}

// Unlike Gen 3 to 5, the encryption constant seeds both streams and picks the block order.
void spec_3ds_decode_pokemon(spec_3ds_pokemon_t *pokemon, const uint8_t *record, size_t record_size,
                             uint8_t generation) {
    uint8_t plain[SPEC_3DS_PARTY_RECORD_SIZE] = {};
    memcpy(plain, record, record_size);
    uint32_t encryption_constant = spec_read_u32_le(&plain[ENCRYPTION_CONSTANT_OFFSET]);
    spec_xor_with_random_stream(&plain[BLOCKS_OFFSET], BLOCKS_SIZE, encryption_constant);
    spec_xor_with_random_stream(&plain[PARTY_DATA_OFFSET], PARTY_DATA_SIZE, encryption_constant);
    spec_unshuffle_blocks(&plain[BLOCKS_OFFSET], BLOCK_SIZE, block_order_of(encryption_constant));
    *pokemon = (spec_3ds_pokemon_t){.generation = generation};
    decode_header(pokemon, plain);
    decode_block_a(pokemon, plain);
    decode_block_b(pokemon, plain);
    decode_block_c(pokemon, plain);
    decode_block_d(pokemon, plain);
    decode_origin(pokemon, plain);
    if (record_size == SPEC_3DS_PARTY_RECORD_SIZE) {
        decode_party_data(pokemon, plain);
    }
    // A failed checksum reads as a Bad Egg, as in Gen 4.
    if (spec_sum_u16(&plain[BLOCKS_OFFSET], BLOCKS_SIZE)
        != spec_read_u16_le(&plain[CHECKSUM_OFFSET])) {
        pokemon->is_bad_egg = true;
        pokemon->is_egg = true;
    }
    // A Bad Egg's pid is no Pokémon's, so it decides nothing.
    if (!pokemon->is_bad_egg) {
        pokemon->personality =
            spec_3ds_decode_personality(pokemon->personality.pid, &pokemon->trainer);
    }
}

static const char *unencodable_marking_of(const spec_3ds_pokemon_t *pokemon) {
    uint8_t marking_max = is_gen7(pokemon) ? GEN7_MARKING_MAX : 1;
    for (size_t mark = 0; mark < SPEC_3DS_MARKING_COUNT; ++mark) {
        if (pokemon->markings[mark] > marking_max) {
            return "a marking is beyond the generation's colors";
        }
    }
    return nullptr;
}

// A field the other generation's record holds must be left zero.
static const char *other_generation_field_of(const spec_3ds_pokemon_t *pokemon) {
    if (is_gen7(pokemon)) {
        bool has_gen6_field = pokemon->training.bag != 0 || pokemon->training.bag_hits != 0
                              || pokemon->origin.encounter_type != 0
                              || pokemon->party_data.form_days_remaining != 0
                              || pokemon->party_data.form_days_elapsed != 0
                              || pokemon->party_data.training_bag_effect != 0;
        return has_gen6_field ? "a Gen 7 record holds a Gen 6 field" : nullptr;
    }
    bool is_hyper_trained = false;
    for (size_t stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        is_hyper_trained = is_hyper_trained || pokemon->hyper_trained[stat];
    }
    bool has_gen7_field = is_hyper_trained || pokemon->resort_event_status != 0
                          || pokemon->party_data.dirt_type != 0
                          || pokemon->party_data.dirt_location != 0;
    return has_gen7_field ? "a Gen 6 record holds a Gen 7 field" : nullptr;
}

static const char *unencodable_field_of(const spec_3ds_pokemon_t *pokemon) {
    if (pokemon->generation != 6 && pokemon->generation != 7) {
        return "generation is neither 6 nor 7";
    }
    if (!spec_fits_in_bits(pokemon->form, FORM_BIT_COUNT)) {
        return "form does not fit in 5 bits";
    }
    if (!spec_fits_in_bits(pokemon->gender, GENDER_BIT_COUNT)) {
        return "gender does not fit in 2 bits";
    }
    if (is_gen7(pokemon)
        && !spec_fits_in_bits(pokemon->ability_number, GEN7_ABILITY_NUMBER_BIT_COUNT)) {
        return "ability_number does not fit in 3 bits";
    }
    for (size_t stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        if (!spec_fits_in_bits(pokemon->ivs[stat], IV_BIT_COUNT)) {
            return "ivs do not fit in 5 bits";
        }
    }
    unsigned ribbon_bit_count = is_gen7(pokemon) ? GEN7_RIBBON_BIT_COUNT : GEN6_RIBBON_BIT_COUNT;
    if ((pokemon->ribbons >> ribbon_bit_count) != 0) {
        return "ribbons hold a ribbon beyond the generation's";
    }
    if (!spec_fits_in_bits(pokemon->pokerus.strain, POKERUS_BIT_COUNT)
        || !spec_fits_in_bits(pokemon->pokerus.days, POKERUS_BIT_COUNT)) {
        return "pokerus fields do not fit in 4 bits";
    }
    if (!spec_fits_in_bits(pokemon->origin.met_level, MET_LEVEL_BIT_COUNT)) {
        return "origin.met_level does not fit in 7 bits";
    }
    if (!spec_fits_in_bits(pokemon->party_data.status.sleep_turns, SLEEP_TURNS_BIT_COUNT)
        || !spec_fits_in_bits(pokemon->party_data.status.toxic_turns, TOXIC_TURNS_BIT_COUNT)) {
        return "party_data.status does not fit its bits";
    }
    const char *marking = unencodable_marking_of(pokemon);
    return marking != nullptr ? marking : other_generation_field_of(pokemon);
}

static void encode_header(uint8_t *plain, const spec_3ds_pokemon_t *pokemon) {
    spec_write_u32_le(&plain[ENCRYPTION_CONSTANT_OFFSET], pokemon->encryption_constant);
    spec_write_u16_le(&plain[FLAGS_OFFSET],
                      (uint16_t)spec_set_flag(0, BAD_EGG_BIT, pokemon->is_bad_egg));
}

static void encode_markings(uint8_t *plain, const spec_3ds_pokemon_t *pokemon) {
    unsigned bit_count = is_gen7(pokemon) ? GEN7_MARKING_BIT_COUNT : GEN6_MARKING_BIT_COUNT;
    uint32_t markings = 0;
    for (unsigned mark = 0; mark < SPEC_3DS_MARKING_COUNT; ++mark) {
        markings = spec_set_bits(markings, mark * bit_count, bit_count, pokemon->markings[mark]);
    }
    if (is_gen7(pokemon)) {
        spec_write_u16_le(&plain[GEN7_MARKINGS_OFFSET], (uint16_t)markings);
    } else {
        plain[GEN6_MARKINGS_OFFSET] = (uint8_t)markings;
    }
}

static void write_ribbons(uint8_t *plain, uint64_t ribbons) {
    for (size_t byte = 0; byte < RIBBON_BYTE_COUNT; ++byte) {
        plain[RIBBONS_OFFSET + byte] = (uint8_t)(ribbons >> (byte * 8));
    }
}

static void encode_block_a(uint8_t *plain, const spec_3ds_pokemon_t *pokemon) {
    spec_write_u16_le(&plain[SPECIES_OFFSET], pokemon->species);
    spec_write_u16_le(&plain[HELD_ITEM_OFFSET], pokemon->held_item);
    spec_write_u16_le(&plain[TRAINER_ID_OFFSET], pokemon->trainer.id);
    spec_write_u16_le(&plain[SECRET_ID_OFFSET], pokemon->trainer.secret_id);
    spec_write_u32_le(&plain[EXPERIENCE_OFFSET], pokemon->experience);
    plain[ABILITY_OFFSET] = pokemon->ability;
    plain[ABILITY_NUMBER_OFFSET] = pokemon->ability_number;
    if (!is_gen7(pokemon)) {
        plain[TRAINING_BAG_HITS_OFFSET] = pokemon->training.bag_hits;
        plain[TRAINING_BAG_OFFSET] = pokemon->training.bag;
    }
    spec_write_u32_le(&plain[PID_OFFSET], pokemon->personality.pid);
    plain[NATURE_OFFSET] = pokemon->nature;
    uint32_t form_byte = 0;
    form_byte = spec_set_flag(form_byte, FATEFUL_ENCOUNTER_BIT, pokemon->is_fateful_encounter);
    form_byte = spec_set_bits(form_byte, GENDER_BIT, GENDER_BIT_COUNT, pokemon->gender);
    form_byte = spec_set_bits(form_byte, FORM_BIT, FORM_BIT_COUNT, pokemon->form);
    plain[FORM_OFFSET] = (uint8_t)form_byte;
    memcpy(&plain[EVS_OFFSET], pokemon->evs, SPEC_STAT_COUNT);
    memcpy(&plain[CONTEST_STATS_OFFSET], pokemon->contest.stats, SPEC_NDS_CONTEST_CATEGORY_COUNT);
    plain[SHEEN_OFFSET] = pokemon->contest.sheen;
    encode_markings(plain, pokemon);
    if (is_gen7(pokemon)) {
        plain[RESORT_EVENT_STATUS_OFFSET] = pokemon->resort_event_status;
    }
    uint32_t pokerus = 0;
    pokerus =
        spec_set_bits(pokerus, POKERUS_STRAIN_BIT, POKERUS_BIT_COUNT, pokemon->pokerus.strain);
    pokerus = spec_set_bits(pokerus, POKERUS_DAYS_BIT, POKERUS_BIT_COUNT, pokemon->pokerus.days);
    plain[POKERUS_OFFSET] = (uint8_t)pokerus;
    spec_write_u32_le(&plain[REGIMENS_OFFSET], pokemon->training.regimens);
    write_ribbons(plain, pokemon->ribbons);
    plain[CONTEST_MEMORY_OFFSET] = pokemon->contest_memory_ribbon_count;
    plain[BATTLE_MEMORY_OFFSET] = pokemon->battle_memory_ribbon_count;
    spec_write_u16_le(&plain[DISTRIBUTION_REGIMENS_OFFSET],
                      pokemon->training.distribution_regimens);
    spec_write_u32_le(&plain[FORM_ARGUMENT_OFFSET], pokemon->form_argument);
}

static void encode_block_b(uint8_t *plain, const spec_3ds_pokemon_t *pokemon) {
    spec_nds_write_text(&plain[NICKNAME_OFFSET], pokemon->nickname, SPEC_3DS_NAME_SIZE);
    for (size_t move = 0; move < SPEC_3DS_MOVE_COUNT; ++move) {
        spec_write_u16_le(&plain[MOVES_OFFSET + move * 2], pokemon->moves[move].id);
        plain[MOVE_PP_OFFSET + move] = pokemon->moves[move].pp;
        plain[PP_UPS_OFFSET + move] = pokemon->moves[move].pp_ups;
        spec_write_u16_le(&plain[RELEARN_MOVES_OFFSET + move * 2], pokemon->relearn_moves[move]);
    }
    uint32_t secret_training = 0;
    secret_training =
        spec_set_flag(secret_training, SECRET_UNLOCKED_BIT, pokemon->training.is_secret_unlocked);
    secret_training =
        spec_set_flag(secret_training, TRAINING_COMPLETE_BIT, pokemon->training.is_complete);
    plain[SECRET_TRAINING_OFFSET] = (uint8_t)secret_training;
    uint32_t ivs = 0;
    for (unsigned stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        ivs = spec_set_bits(ivs, stat * IV_BIT_COUNT, IV_BIT_COUNT, pokemon->ivs[stat]);
    }
    ivs = spec_set_flag(ivs, IS_EGG_BIT, pokemon->is_egg);
    ivs = spec_set_flag(ivs, IS_NICKNAMED_BIT, pokemon->is_nicknamed);
    spec_write_u32_le(&plain[IVS_OFFSET], ivs);
}

static void encode_block_c(uint8_t *plain, const spec_3ds_pokemon_t *pokemon) {
    const spec_3ds_latest_trainer_t *latest_trainer = &pokemon->latest_trainer;
    spec_nds_write_text(&plain[LATEST_TRAINER_NAME_OFFSET], latest_trainer->name,
                        SPEC_3DS_NAME_SIZE);
    plain[LATEST_TRAINER_GENDER_OFFSET] = latest_trainer->gender;
    plain[CURRENT_TRAINER_OFFSET] = pokemon->is_with_latest_trainer ? 1 : 0;
    for (size_t index = 0; index < SPEC_3DS_GEO_MEMORY_COUNT; ++index) {
        plain[GEO_MEMORIES_OFFSET + index * 2] = pokemon->geo_memories[index].subregion;
        plain[GEO_MEMORIES_OFFSET + index * 2 + 1] = pokemon->geo_memories[index].country;
    }
    write_bond(plain, LATEST_TRAINER_BOND_OFFSET, LATEST_TRAINER_MEMORY_VARIABLE_OFFSET,
               &latest_trainer->bond);
    plain[LATEST_TRAINER_BOND_OFFSET + 4] = latest_trainer->bond.memory.feeling;
    plain[FULLNESS_OFFSET] = pokemon->fullness;
    plain[ENJOYMENT_OFFSET] = pokemon->enjoyment;
}

static void encode_block_d(uint8_t *plain, const spec_3ds_pokemon_t *pokemon) {
    spec_nds_write_text(&plain[TRAINER_NAME_OFFSET], pokemon->trainer.name, SPEC_3DS_NAME_SIZE);
    write_bond(plain, TRAINER_BOND_OFFSET, TRAINER_MEMORY_VARIABLE_OFFSET, &pokemon->trainer_bond);
    plain[TRAINER_FEELING_OFFSET] = pokemon->trainer_bond.memory.feeling;
    if (is_gen7(pokemon)) {
        uint32_t hyper_training = 0;
        for (size_t stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
            hyper_training = spec_set_flag(hyper_training, HYPER_TRAINING_BITS[stat],
                                           pokemon->hyper_trained[stat]);
        }
        plain[HYPER_TRAINING_OFFSET] = (uint8_t)hyper_training;
    }
    plain[COUNTRY_OFFSET] = pokemon->region.country;
    plain[SUBREGION_OFFSET] = pokemon->region.subregion;
    plain[CONSOLE_REGION_OFFSET] = pokemon->region.console_region;
    plain[LANGUAGE_OFFSET] = pokemon->language;
}

static void write_date(uint8_t *plain, size_t offset, const spec_3ds_date_t *date) {
    plain[offset] = date->year;
    plain[offset + 1] = date->month;
    plain[offset + 2] = date->day;
}

static void encode_origin(uint8_t *plain, const spec_3ds_pokemon_t *pokemon) {
    const spec_3ds_origin_t *origin = &pokemon->origin;
    plain[VERSION_OFFSET] = origin->version;
    plain[BALL_OFFSET] = origin->ball;
    uint32_t met_level = origin->met_level;
    met_level = spec_set_flag(met_level, TRAINER_FEMALE_BIT, pokemon->trainer.is_female);
    plain[MET_LEVEL_OFFSET] = (uint8_t)met_level;
    spec_write_u16_le(&plain[MET_LOCATION_OFFSET], origin->met_location);
    write_date(plain, MET_DATE_OFFSET, &origin->met_date);
    spec_write_u16_le(&plain[EGG_LOCATION_OFFSET], origin->egg_location);
    write_date(plain, EGG_DATE_OFFSET, &origin->egg_date);
    if (!is_gen7(pokemon)) {
        plain[ENCOUNTER_TYPE_OFFSET] = origin->encounter_type;
    }
}

static void encode_party_data(uint8_t *plain, const spec_3ds_pokemon_t *pokemon) {
    const spec_3ds_party_data_t *party_data = &pokemon->party_data;
    spec_write_u32_le(&plain[STATUS_OFFSET], spec_nds_encode_status(&party_data->status));
    plain[LEVEL_OFFSET] = party_data->level;
    if (is_gen7(pokemon)) {
        plain[DIRT_TYPE_OFFSET] = party_data->dirt_type;
        plain[DIRT_LOCATION_OFFSET] = party_data->dirt_location;
    } else {
        plain[FORM_DAYS_REMAINING_OFFSET] = party_data->form_days_remaining;
        plain[FORM_DAYS_ELAPSED_OFFSET] = party_data->form_days_elapsed;
        plain[TRAINING_BAG_EFFECT_OFFSET] = party_data->training_bag_effect;
    }
    spec_write_u16_le(&plain[CURRENT_HP_OFFSET], party_data->current_hp);
    for (size_t stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        spec_write_u16_le(&plain[STATS_OFFSET + stat * 2], party_data->stats[stat]);
    }
}

spec_error_t spec_3ds_encode_pokemon(uint8_t *record, size_t record_size,
                                     const spec_3ds_pokemon_t *pokemon) {
    const char *unencodable_field = unencodable_field_of(pokemon);
    if (unencodable_field != nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, unencodable_field);
    }
    uint8_t plain[SPEC_3DS_PARTY_RECORD_SIZE] = {};
    encode_header(plain, pokemon);
    encode_block_a(plain, pokemon);
    encode_block_b(plain, pokemon);
    encode_block_c(plain, pokemon);
    encode_block_d(plain, pokemon);
    encode_origin(plain, pokemon);
    encode_party_data(plain, pokemon);
    spec_write_u16_le(&plain[CHECKSUM_OFFSET], spec_sum_u16(&plain[BLOCKS_OFFSET], BLOCKS_SIZE));
    uint32_t encryption_constant = pokemon->encryption_constant;
    spec_shuffle_blocks(&plain[BLOCKS_OFFSET], BLOCK_SIZE, block_order_of(encryption_constant));
    spec_xor_with_random_stream(&plain[BLOCKS_OFFSET], BLOCKS_SIZE, encryption_constant);
    spec_xor_with_random_stream(&plain[PARTY_DATA_OFFSET], PARTY_DATA_SIZE, encryption_constant);
    memcpy(record, plain, record_size);
    return SPEC_OK;
}

// As Gen 4's Pokemon_FromBoxPokemon.
void spec_3ds_fill_party_data(spec_3ds_pokemon_t *pokemon) {
    bool has_party_data =
        pokemon->party_data.level != 0 || pokemon->party_data.stats[SPEC_STAT_HP] != 0;
    if (has_party_data || check_stat_inputs(pokemon) != SPEC_OK) {
        return;
    }
    pokemon->party_data = (spec_3ds_party_data_t){};
    calculate_stats(pokemon);
}

spec_error_t spec_3ds_read_pokemon(spec_3ds_pokemon_t *pokemon, const uint8_t *raw, size_t raw_size,
                                   uint8_t generation) {
    if (!is_record_size(raw_size)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "raw_size is neither 232 nor 260");
    }
    if (generation != 6 && generation != 7) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "generation is neither 6 nor 7");
    }
    spec_3ds_decode_pokemon(pokemon, raw, raw_size, generation);
    return SPEC_OK;
}

spec_error_t spec_3ds_write_pokemon(uint8_t *raw, size_t raw_size,
                                    const spec_3ds_pokemon_t *pokemon) {
    if (!is_record_size(raw_size)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "raw_size is neither 232 nor 260");
    }
    return spec_3ds_encode_pokemon(raw, raw_size, pokemon);
}

uint8_t spec_3ds_pokemon_get_level(const spec_3ds_pokemon_t *pokemon) {
    if (!has_species_data(pokemon->generation, pokemon->species)) {
        return 0;
    }
    const spec_3ds_species_data_t *species_data =
        species_data_of(pokemon->generation, pokemon->species, pokemon->form);
    return spec_level_for_experience(species_data->growth_rate, pokemon->experience);
}

spec_error_t spec_3ds_pokemon_get_name(const spec_3ds_pokemon_t *pokemon,
                                       char8_t name[static SPEC_3DS_TEXT_BUFFER_SIZE]) {
    return spec_3ds_text_to_utf8(name, pokemon->nickname, SPEC_3DS_NAME_SIZE);
}

spec_error_t spec_3ds_pokemon_calculate_stats(spec_3ds_pokemon_t *pokemon) {
    spec_error_t error = check_stat_inputs(pokemon);
    if (error != SPEC_OK) {
        return error;
    }
    calculate_stats(pokemon);
    return SPEC_OK;
}

// The game writes an egg's name over the old one, keeping what follows its terminator.
static spec_error_t write_egg_nickname(spec_3ds_pokemon_t *pokemon) {
    uint16_t egg_name[SPEC_3DS_NAME_SIZE];
    spec_error_t error = encode_species_name(egg_name, EGG_SPECIES, pokemon->language);
    if (error != SPEC_OK) {
        return error;
    }
    for (size_t index = 0; index < SPEC_3DS_NAME_SIZE; ++index) {
        pokemon->nickname[index] = egg_name[index];
        if (egg_name[index] == END_OF_TEXT) {
            break;
        }
    }
    pokemon->is_nicknamed = false;
    return SPEC_OK;
}

// A new Pokémon's name: zeros after the terminator.
static spec_error_t write_species_nickname(spec_3ds_pokemon_t *pokemon) {
    spec_error_t error =
        encode_species_name(pokemon->nickname, pokemon->species, pokemon->language);
    if (error == SPEC_OK) {
        pokemon->is_nicknamed = false;
    }
    return error;
}

// As the games name a Pokémon; they show a Bad Egg's name without storing one.
spec_error_t spec_3ds_pokemon_remove_nickname(spec_3ds_pokemon_t *pokemon) {
    if (pokemon->language >= SPEC_NAME_LANGUAGE_COUNT) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "language is not a game language");
    }
    if (pokemon->is_bad_egg) {
        return spec_fail(SPEC_ERROR_UNKNOWN_NAME, "the games store no name for a Bad Egg");
    }
    if (pokemon->is_egg) {
        return write_egg_nickname(pokemon);
    }
    return write_species_nickname(pokemon);
}

spec_error_t spec_3ds_pokemon_set_level(spec_3ds_pokemon_t *pokemon, uint8_t level) {
    spec_error_t error = check_stat_inputs(pokemon);
    if (error != SPEC_OK) {
        return error;
    }
    if (level == 0 || level >= SPEC_LEVEL_COUNT) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "level is not 1 to 100");
    }
    const spec_3ds_species_data_t *species_data =
        species_data_of(pokemon->generation, pokemon->species, pokemon->form);
    pokemon->experience = spec_experience_for_level(species_data->growth_rate, level);
    calculate_stats(pokemon);
    return SPEC_OK;
}

// The games set the flag by comparing the new name with the species name.
static bool is_species_name(const spec_3ds_pokemon_t *pokemon) {
    uint16_t species_name[SPEC_3DS_NAME_SIZE];
    if (pokemon->language >= SPEC_NAME_LANGUAGE_COUNT
        || encode_species_name(species_name, pokemon->species, pokemon->language) != SPEC_OK) {
        return false;
    }
    for (size_t index = 0; index < SPEC_3DS_NAME_SIZE; ++index) {
        if (pokemon->nickname[index] != species_name[index]) {
            return false;
        }
        if (species_name[index] == END_OF_TEXT) {
            return true;
        }
    }
    return true;
}

// Retail saves show a name typed over an old one keeping the old one's units past the new
// terminator.
spec_error_t spec_3ds_pokemon_set_nickname(spec_3ds_pokemon_t *pokemon, const char8_t *nickname,
                                           spec_naming_t naming) {
    if (pokemon->is_egg || pokemon->is_bad_egg) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "the games never name an egg");
    }
    if (naming != SPEC_NAMING_CAUGHT_OR_HATCHED && naming != SPEC_NAMING_NAME_RATER) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "naming is not a known way");
    }
    // TODO: Check the Japanese, Korean and Chinese nickname length.
    // TODO: Check trash bytes on Gen 6 and 7 rename.
    spec_error_t error =
        spec_3ds_text_from_utf8(pokemon->nickname, SPEC_3DS_NAME_SIZE, nickname, pokemon->language);
    if (error != SPEC_OK) {
        return error;
    }
    pokemon->is_nicknamed = !is_species_name(pokemon);
    return SPEC_OK;
}

static bool is_3ds_game(spec_game_type_t type) {
    return type >= SPEC_GAME_TYPE_X_Y && type <= SPEC_GAME_TYPE_ULTRA_SUN_ULTRA_MOON;
}

// Game types count up generation by generation, so the latest row begun by the type is in force.
static const spec_3ds_species_row_t *latest_row(spec_game_type_t type, uint16_t species,
                                                uint8_t form) {
    const spec_3ds_species_row_t *latest = nullptr;
    for (size_t index = 0; index < spec_3ds_species_row_count; ++index) {
        const spec_3ds_species_row_t *row = &spec_3ds_species_rows[index];
        bool applies = row->species == species && row->form == form && row->from_game <= type;
        if (applies && (latest == nullptr || row->from_game >= latest->from_game)) {
            latest = row;
        }
    }
    return latest;
}

const spec_3ds_species_data_t *spec_3ds_get_species_data(spec_game_type_t type, uint16_t species,
                                                         uint8_t form) {
    if (!is_3ds_game(type)) {
        return nullptr;
    }
    const spec_3ds_species_row_t *row = latest_row(type, species, form);
    if (row == nullptr) {
        row = latest_row(type, species, 0);
    }
    if (row == nullptr) {
        return nullptr;
    }
    return &row->data;
}
