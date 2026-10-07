// Gen 5 Pokémon records: the encrypted codec, stats, names and species data.

#include <string.h>

#include "nds/nds_internal.h"
#include "ndsi/ndsi.h"
#include "ndsi/ndsi_internal.h"
#include "ndsi/tables.h"
#include "spec_internal.h"

constexpr size_t PERSONALITY_OFFSET = 0x00;
constexpr size_t FLAGS_OFFSET = 0x04;
constexpr size_t CHECKSUM_OFFSET = 0x06;
constexpr size_t BLOCKS_OFFSET = 0x08;
constexpr size_t BLOCK_SIZE = 0x20;
constexpr size_t BLOCKS_SIZE = 4 * BLOCK_SIZE;

constexpr size_t SPECIES_OFFSET = 0x08;
constexpr size_t HELD_ITEM_OFFSET = 0x0A;
constexpr size_t TRAINER_ID_OFFSET = 0x0C;
constexpr size_t SECRET_ID_OFFSET = 0x0E;
constexpr size_t EXPERIENCE_OFFSET = 0x10;
constexpr size_t FRIENDSHIP_OFFSET = 0x14;
constexpr size_t ABILITY_OFFSET = 0x15;
constexpr size_t MARKINGS_OFFSET = 0x16;
constexpr size_t LANGUAGE_OFFSET = 0x17;
constexpr size_t EVS_OFFSET = 0x18;
constexpr size_t CONTEST_STATS_OFFSET = 0x1E;
constexpr size_t SHEEN_OFFSET = 0x23;
constexpr size_t SINNOH_RIBBONS_OFFSET = 0x24;

constexpr size_t MOVES_OFFSET = 0x28;
constexpr size_t MOVE_PP_OFFSET = 0x30;
constexpr size_t PP_UPS_OFFSET = 0x34;
constexpr size_t IVS_OFFSET = 0x38;
constexpr size_t HOENN_RIBBONS_OFFSET = 0x3C;
constexpr size_t FORM_OFFSET = 0x40;
constexpr size_t NATURE_OFFSET = 0x41;
constexpr size_t GEN5_FLAGS_OFFSET = 0x42;

constexpr size_t NICKNAME_OFFSET = 0x48;
constexpr size_t VERSION_OFFSET = 0x5F;
constexpr size_t SUPER_CONTEST_RIBBONS_OFFSET = 0x60;

constexpr size_t TRAINER_NAME_OFFSET = 0x68;
constexpr size_t EGG_DATE_OFFSET = 0x78;
constexpr size_t MET_DATE_OFFSET = 0x7B;
constexpr size_t EGG_LOCATION_OFFSET = 0x7E;
constexpr size_t MET_LOCATION_OFFSET = 0x80;
constexpr size_t POKERUS_OFFSET = 0x82;
constexpr size_t BALL_OFFSET = 0x83;
constexpr size_t MET_LEVEL_OFFSET = 0x84;
constexpr size_t ENCOUNTER_TYPE_OFFSET = 0x85;
constexpr size_t POKESTAR_FAME_OFFSET = 0x87;

constexpr size_t PARTY_DATA_OFFSET = 0x88;
constexpr size_t PARTY_DATA_SIZE = SPEC_NDSI_PARTY_RECORD_SIZE - PARTY_DATA_OFFSET;
constexpr size_t STATUS_OFFSET = 0x88;
constexpr size_t LEVEL_OFFSET = 0x8C;
constexpr size_t CURRENT_HP_OFFSET = 0x8E;
constexpr size_t STATS_OFFSET = 0x90;
constexpr size_t MAIL_OFFSET = 0x9C;

constexpr unsigned BAD_EGG_BIT = 2;
constexpr unsigned MARKINGS_BIT_COUNT = 6;
constexpr unsigned SINNOH_RIBBONS_BIT_COUNT = 28;
constexpr unsigned SUPER_CONTEST_RIBBONS_BIT_COUNT = 20;

constexpr unsigned IV_BIT_COUNT = 5;
constexpr unsigned IS_EGG_BIT = 30;
constexpr unsigned IS_NICKNAMED_BIT = 31;

constexpr unsigned FATEFUL_ENCOUNTER_BIT = 0;
constexpr unsigned GENDER_BIT = 1;
constexpr unsigned GENDER_BIT_COUNT = 2;
constexpr unsigned FORM_BIT = 3;
constexpr unsigned FORM_BIT_COUNT = 5;
constexpr unsigned HIDDEN_ABILITY_BIT = 0;
constexpr unsigned N_POKEMON_BIT = 1;

constexpr unsigned POKERUS_DAYS_BIT = 0;
constexpr unsigned POKERUS_STRAIN_BIT = 4;
constexpr unsigned POKERUS_BIT_COUNT = 4;

constexpr unsigned MET_LEVEL_BIT_COUNT = 7;
constexpr unsigned TRAINER_FEMALE_BIT = 7;
constexpr unsigned SLEEP_TURNS_BIT_COUNT = 3;
constexpr unsigned TOXIC_TURNS_BIT_COUNT = 4;

constexpr uint16_t SHEDINJA = 292;
constexpr uint16_t FIRST_MAIL_ITEM = 137;
constexpr uint16_t LAST_MAIL_ITEM = 148;
// Gen 5 names an egg in the game's language; only the English name is on record.
constexpr char8_t ENGLISH_EGG_NICKNAME[] = u8"Egg";
constexpr uint16_t END_OF_TEXT = 0xFFFF;

// As the game orders the blocks: ((personality >> 13) & 31) % 24.
static size_t block_order_of(const uint8_t *record) {
    return (spec_read_u32_le(&record[PERSONALITY_OFFSET]) >> 13) & 31;
}

static bool is_record_size(size_t raw_size) {
    return raw_size == SPEC_NDSI_BOX_RECORD_SIZE || raw_size == SPEC_NDSI_PARTY_RECORD_SIZE;
}

static bool has_species_data(uint16_t species) {
    return species != 0 && species < SPEC_NDSI_SPECIES_COUNT;
}

// The Gen 5 games agree on every species and form they share, and Black 2 and White 2 have them
// all.
static const spec_ndsi_species_data_t *species_data_of(uint16_t species, uint8_t form) {
    return spec_ndsi_get_species_data(SPEC_GAME_TYPE_BLACK2_WHITE2, species, form);
}

static uint16_t stat_of(const spec_ndsi_pokemon_t *pokemon, const uint8_t *base_stats,
                        spec_stat_t stat, uint8_t level) {
    if (stat == SPEC_STAT_HP && pokemon->species == SHEDINJA) {
        return 1;
    }
    return spec_calculate_stat(stat, base_stats[stat], pokemon->ivs[stat], pokemon->evs[stat],
                               level, pokemon->nature);
}

static void calculate_stats(spec_ndsi_pokemon_t *pokemon) {
    const uint8_t *base_stats = species_data_of(pokemon->species, pokemon->form)->base_stats;
    spec_growth_rate_t growth_rate = species_data_of(pokemon->species, 0)->growth_rate;
    uint8_t level = spec_level_for_experience(growth_rate, pokemon->experience);
    uint16_t old_max_hp = pokemon->party_data.stats[SPEC_STAT_HP];
    uint16_t new_max_hp = stat_of(pokemon, base_stats, SPEC_STAT_HP, level);
    pokemon->party_data.current_hp = spec_nds_current_hp_after(
        pokemon->party_data.current_hp, old_max_hp, new_max_hp, pokemon->species == SHEDINJA);
    pokemon->party_data.level = level;
    pokemon->party_data.stats[SPEC_STAT_HP] = new_max_hp;
    for (spec_stat_t stat = SPEC_STAT_ATTACK; stat < SPEC_STAT_COUNT; ++stat) {
        pokemon->party_data.stats[stat] = stat_of(pokemon, base_stats, stat, level);
    }
}

static spec_error_t check_stat_inputs(const spec_ndsi_pokemon_t *pokemon) {
    if (!has_species_data(pokemon->species)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "species has no species data");
    }
    if (pokemon->nature >= SPEC_NATURE_COUNT) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "nature is not one of the 25");
    }
    return SPEC_OK;
}

static spec_error_t write_zero_padded_nickname(spec_ndsi_pokemon_t *pokemon, const char8_t *name) {
    uint16_t nickname[SPEC_NDSI_NICKNAME_SIZE] = {};
    spec_error_t error =
        spec_ndsi_text_from_utf8(nickname, SPEC_NDSI_NICKNAME_SIZE, name, pokemon->language);
    if (error != SPEC_OK) {
        return error;
    }
    memcpy(pokemon->nickname, nickname, sizeof nickname);
    return SPEC_OK;
}

// Only Pokémon from Gen 3 and 4 drew their IVs right after their pid.
static bool is_from_gen3_or_gen4(spec_version_t version) {
    return version < SPEC_VERSION_WHITE;
}

static void decode_header(spec_ndsi_pokemon_t *pokemon, const uint8_t *plain) {
    pokemon->personality.pid = spec_read_u32_le(&plain[PERSONALITY_OFFSET]);
    pokemon->is_bad_egg = spec_get_flag(spec_read_u16_le(&plain[FLAGS_OFFSET]), BAD_EGG_BIT);
}

static void decode_block_a(spec_ndsi_pokemon_t *pokemon, const uint8_t *plain) {
    pokemon->species = spec_read_u16_le(&plain[SPECIES_OFFSET]);
    pokemon->held_item = spec_read_u16_le(&plain[HELD_ITEM_OFFSET]);
    pokemon->trainer.id = spec_read_u16_le(&plain[TRAINER_ID_OFFSET]);
    pokemon->trainer.secret_id = spec_read_u16_le(&plain[SECRET_ID_OFFSET]);
    pokemon->experience = spec_read_u32_le(&plain[EXPERIENCE_OFFSET]);
    pokemon->friendship = plain[FRIENDSHIP_OFFSET];
    pokemon->ability = plain[ABILITY_OFFSET];
    pokemon->markings = (uint8_t)spec_get_bits(plain[MARKINGS_OFFSET], 0, MARKINGS_BIT_COUNT);
    pokemon->language = (spec_language_t)plain[LANGUAGE_OFFSET];
    memcpy(pokemon->evs, &plain[EVS_OFFSET], SPEC_STAT_COUNT);
    memcpy(pokemon->contest.stats, &plain[CONTEST_STATS_OFFSET], SPEC_NDS_CONTEST_CATEGORY_COUNT);
    pokemon->contest.sheen = plain[SHEEN_OFFSET];
    pokemon->sinnoh_ribbons =
        spec_get_bits(spec_read_u32_le(&plain[SINNOH_RIBBONS_OFFSET]), 0, SINNOH_RIBBONS_BIT_COUNT);
}

// The gender bits are derived on write.
static void decode_block_b(spec_ndsi_pokemon_t *pokemon, const uint8_t *plain) {
    for (size_t move = 0; move < SPEC_NDSI_MOVE_COUNT; ++move) {
        pokemon->moves[move].id = spec_read_u16_le(&plain[MOVES_OFFSET + move * 2]);
        pokemon->moves[move].pp = plain[MOVE_PP_OFFSET + move];
        pokemon->moves[move].pp_ups = plain[PP_UPS_OFFSET + move];
    }
    uint32_t ivs = spec_read_u32_le(&plain[IVS_OFFSET]);
    for (unsigned stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        pokemon->ivs[stat] = (uint8_t)spec_get_bits(ivs, stat * IV_BIT_COUNT, IV_BIT_COUNT);
    }
    pokemon->is_egg = spec_get_flag(ivs, IS_EGG_BIT);
    pokemon->is_nicknamed = spec_get_flag(ivs, IS_NICKNAMED_BIT);
    pokemon->hoenn_ribbons = spec_read_u32_le(&plain[HOENN_RIBBONS_OFFSET]);
    pokemon->is_fateful_encounter = spec_get_flag(plain[FORM_OFFSET], FATEFUL_ENCOUNTER_BIT);
    pokemon->form = (uint8_t)spec_get_bits(plain[FORM_OFFSET], FORM_BIT, FORM_BIT_COUNT);
    pokemon->nature = (spec_nature_t)plain[NATURE_OFFSET];
    pokemon->has_hidden_ability = spec_get_flag(plain[GEN5_FLAGS_OFFSET], HIDDEN_ABILITY_BIT);
    pokemon->is_n_pokemon = spec_get_flag(plain[GEN5_FLAGS_OFFSET], N_POKEMON_BIT);
}

static void decode_block_c(spec_ndsi_pokemon_t *pokemon, const uint8_t *plain) {
    spec_nds_read_text(pokemon->nickname, &plain[NICKNAME_OFFSET], SPEC_NDSI_NICKNAME_SIZE);
    pokemon->super_contest_ribbons = spec_get_bits(
        spec_read_u32_le(&plain[SUPER_CONTEST_RIBBONS_OFFSET]), 0, SUPER_CONTEST_RIBBONS_BIT_COUNT);
}

static void decode_block_d(spec_ndsi_pokemon_t *pokemon, const uint8_t *plain) {
    spec_nds_read_text(pokemon->trainer.name, &plain[TRAINER_NAME_OFFSET],
                       SPEC_NDSI_TRAINER_NAME_SIZE);
    pokemon->trainer.is_female = spec_get_flag(plain[MET_LEVEL_OFFSET], TRAINER_FEMALE_BIT);
    pokemon->pokerus.strain =
        (uint8_t)spec_get_bits(plain[POKERUS_OFFSET], POKERUS_STRAIN_BIT, POKERUS_BIT_COUNT);
    pokemon->pokerus.days =
        (uint8_t)spec_get_bits(plain[POKERUS_OFFSET], POKERUS_DAYS_BIT, POKERUS_BIT_COUNT);
    pokemon->pokestar_fame = plain[POKESTAR_FAME_OFFSET];
}

static spec_ndsi_date_t date_at(const uint8_t *plain, size_t offset) {
    return (spec_ndsi_date_t){
        .year = plain[offset],
        .month = plain[offset + 1],
        .day = plain[offset + 2],
    };
}

// The origin spreads over blocks C and D.
static void decode_origin(spec_ndsi_origin_t *origin, const uint8_t *plain) {
    origin->version = (spec_version_t)plain[VERSION_OFFSET];
    origin->ball = (spec_ball_t)plain[BALL_OFFSET];
    origin->met_level = (uint8_t)spec_get_bits(plain[MET_LEVEL_OFFSET], 0, MET_LEVEL_BIT_COUNT);
    origin->met_location = spec_read_u16_le(&plain[MET_LOCATION_OFFSET]);
    origin->met_date = date_at(plain, MET_DATE_OFFSET);
    origin->egg_location = spec_read_u16_le(&plain[EGG_LOCATION_OFFSET]);
    origin->egg_date = date_at(plain, EGG_DATE_OFFSET);
    origin->encounter_type = plain[ENCOUNTER_TYPE_OFFSET];
}

static void decode_party_data(spec_ndsi_party_data_t *party_data, const uint8_t *plain) {
    spec_nds_decode_status(&party_data->status, spec_read_u32_le(&plain[STATUS_OFFSET]));
    party_data->level = plain[LEVEL_OFFSET];
    party_data->current_hp = spec_read_u16_le(&plain[CURRENT_HP_OFFSET]);
    for (size_t stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        party_data->stats[stat] = spec_read_u16_le(&plain[STATS_OFFSET + stat * 2]);
    }
    spec_ndsi_decode_mail(&party_data->mail, &plain[MAIL_OFFSET]);
}

void spec_ndsi_decode_pokemon(spec_ndsi_pokemon_t *pokemon, const uint8_t *record,
                              size_t record_size) {
    uint8_t plain[SPEC_NDSI_PARTY_RECORD_SIZE] = {};
    memcpy(plain, record, record_size);
    uint16_t stored_checksum = spec_read_u16_le(&plain[CHECKSUM_OFFSET]);
    spec_xor_with_random_stream(&plain[BLOCKS_OFFSET], BLOCKS_SIZE, stored_checksum);
    spec_xor_with_random_stream(&plain[PARTY_DATA_OFFSET], PARTY_DATA_SIZE,
                                spec_read_u32_le(&plain[PERSONALITY_OFFSET]));
    spec_unshuffle_blocks(&plain[BLOCKS_OFFSET], BLOCK_SIZE, block_order_of(plain));
    *pokemon = (spec_ndsi_pokemon_t){};
    decode_header(pokemon, plain);
    decode_block_a(pokemon, plain);
    decode_block_b(pokemon, plain);
    decode_block_c(pokemon, plain);
    decode_block_d(pokemon, plain);
    decode_origin(&pokemon->origin, plain);
    if (record_size == SPEC_NDSI_PARTY_RECORD_SIZE) {
        decode_party_data(&pokemon->party_data, plain);
    }
    // A failed checksum reads as a Bad Egg, as in Gen 4.
    if (spec_sum_u16(&plain[BLOCKS_OFFSET], BLOCKS_SIZE) != stored_checksum) {
        pokemon->is_bad_egg = true;
        pokemon->is_egg = true;
    }
    if (pokemon->species != 0) {
        pokemon->personality = spec_ndsi_decode_personality(pokemon->personality.pid,
                                                            pokemon->species, &pokemon->trainer);
    }
    if (pokemon->species != 0 && is_from_gen3_or_gen4(pokemon->origin.version)) {
        pokemon->iv_method = spec_find_iv_method(pokemon->personality.pid, pokemon->ivs);
    }
}

static const char *unencodable_field_of(const spec_ndsi_pokemon_t *pokemon) {
    if (!spec_fits_in_bits(pokemon->form, FORM_BIT_COUNT)) {
        return "form does not fit in 5 bits";
    }
    if (!spec_fits_in_bits(pokemon->markings, MARKINGS_BIT_COUNT)) {
        return "markings do not fit in 6 bits";
    }
    for (size_t stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        if (!spec_fits_in_bits(pokemon->ivs[stat], IV_BIT_COUNT)) {
            return "ivs do not fit in 5 bits";
        }
    }
    if (!spec_fits_in_bits(pokemon->sinnoh_ribbons, SINNOH_RIBBONS_BIT_COUNT)) {
        return "sinnoh_ribbons do not fit in 28 bits";
    }
    if (!spec_fits_in_bits(pokemon->super_contest_ribbons, SUPER_CONTEST_RIBBONS_BIT_COUNT)) {
        return "super_contest_ribbons do not fit in 20 bits";
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
    if (!spec_fits_in_bits(pokemon->party_data.status.sleep_turns, SLEEP_TURNS_BIT_COUNT)) {
        return "party_data.status.sleep_turns does not fit in 3 bits";
    }
    if (!spec_fits_in_bits(pokemon->party_data.status.toxic_turns, TOXIC_TURNS_BIT_COUNT)) {
        return "party_data.status.toxic_turns does not fit in 4 bits";
    }
    return nullptr;
}

static void encode_header(uint8_t *plain, const spec_ndsi_pokemon_t *pokemon) {
    spec_write_u32_le(&plain[PERSONALITY_OFFSET], pokemon->personality.pid);
    spec_write_u16_le(&plain[FLAGS_OFFSET],
                      (uint16_t)spec_set_flag(0, BAD_EGG_BIT, pokemon->is_bad_egg));
}

static void encode_block_a(uint8_t *plain, const spec_ndsi_pokemon_t *pokemon) {
    spec_write_u16_le(&plain[SPECIES_OFFSET], pokemon->species);
    spec_write_u16_le(&plain[HELD_ITEM_OFFSET], pokemon->held_item);
    spec_write_u16_le(&plain[TRAINER_ID_OFFSET], pokemon->trainer.id);
    spec_write_u16_le(&plain[SECRET_ID_OFFSET], pokemon->trainer.secret_id);
    spec_write_u32_le(&plain[EXPERIENCE_OFFSET], pokemon->experience);
    plain[FRIENDSHIP_OFFSET] = pokemon->friendship;
    plain[ABILITY_OFFSET] = pokemon->ability;
    plain[MARKINGS_OFFSET] = pokemon->markings;
    plain[LANGUAGE_OFFSET] = pokemon->language;
    memcpy(&plain[EVS_OFFSET], pokemon->evs, SPEC_STAT_COUNT);
    memcpy(&plain[CONTEST_STATS_OFFSET], pokemon->contest.stats, SPEC_NDS_CONTEST_CATEGORY_COUNT);
    plain[SHEEN_OFFSET] = pokemon->contest.sheen;
    spec_write_u32_le(&plain[SINNOH_RIBBONS_OFFSET], pokemon->sinnoh_ribbons);
}

// The empty record is all zero, gender included.
static uint8_t stored_gender_of(const spec_ndsi_pokemon_t *pokemon) {
    if (pokemon->species == 0) {
        return 0;
    }
    return spec_ndsi_decode_personality(pokemon->personality.pid, pokemon->species,
                                        &pokemon->trainer)
        .gender;
}

static void encode_block_b(uint8_t *plain, const spec_ndsi_pokemon_t *pokemon) {
    for (size_t move = 0; move < SPEC_NDSI_MOVE_COUNT; ++move) {
        spec_write_u16_le(&plain[MOVES_OFFSET + move * 2], pokemon->moves[move].id);
        plain[MOVE_PP_OFFSET + move] = pokemon->moves[move].pp;
        plain[PP_UPS_OFFSET + move] = pokemon->moves[move].pp_ups;
    }
    uint32_t ivs = 0;
    for (unsigned stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        ivs = spec_set_bits(ivs, stat * IV_BIT_COUNT, IV_BIT_COUNT, pokemon->ivs[stat]);
    }
    ivs = spec_set_flag(ivs, IS_EGG_BIT, pokemon->is_egg);
    ivs = spec_set_flag(ivs, IS_NICKNAMED_BIT, pokemon->is_nicknamed);
    uint32_t form_byte = 0;
    form_byte = spec_set_flag(form_byte, FATEFUL_ENCOUNTER_BIT, pokemon->is_fateful_encounter);
    form_byte = spec_set_bits(form_byte, GENDER_BIT, GENDER_BIT_COUNT, stored_gender_of(pokemon));
    form_byte = spec_set_bits(form_byte, FORM_BIT, FORM_BIT_COUNT, pokemon->form);
    uint32_t gen5_flags = 0;
    gen5_flags = spec_set_flag(gen5_flags, HIDDEN_ABILITY_BIT, pokemon->has_hidden_ability);
    gen5_flags = spec_set_flag(gen5_flags, N_POKEMON_BIT, pokemon->is_n_pokemon);
    spec_write_u32_le(&plain[IVS_OFFSET], ivs);
    spec_write_u32_le(&plain[HOENN_RIBBONS_OFFSET], pokemon->hoenn_ribbons);
    plain[FORM_OFFSET] = (uint8_t)form_byte;
    plain[NATURE_OFFSET] = pokemon->nature;
    plain[GEN5_FLAGS_OFFSET] = (uint8_t)gen5_flags;
}

static void encode_block_c(uint8_t *plain, const spec_ndsi_pokemon_t *pokemon) {
    spec_nds_write_text(&plain[NICKNAME_OFFSET], pokemon->nickname, SPEC_NDSI_NICKNAME_SIZE);
    spec_write_u32_le(&plain[SUPER_CONTEST_RIBBONS_OFFSET], pokemon->super_contest_ribbons);
}

static void encode_block_d(uint8_t *plain, const spec_ndsi_pokemon_t *pokemon) {
    spec_nds_write_text(&plain[TRAINER_NAME_OFFSET], pokemon->trainer.name,
                        SPEC_NDSI_TRAINER_NAME_SIZE);
    uint32_t pokerus = 0;
    pokerus =
        spec_set_bits(pokerus, POKERUS_STRAIN_BIT, POKERUS_BIT_COUNT, pokemon->pokerus.strain);
    pokerus = spec_set_bits(pokerus, POKERUS_DAYS_BIT, POKERUS_BIT_COUNT, pokemon->pokerus.days);
    plain[POKERUS_OFFSET] = (uint8_t)pokerus;
    plain[POKESTAR_FAME_OFFSET] = pokemon->pokestar_fame;
}

static void write_date(uint8_t *plain, size_t offset, const spec_ndsi_date_t *date) {
    plain[offset] = date->year;
    plain[offset + 1] = date->month;
    plain[offset + 2] = date->day;
}

static void encode_origin(uint8_t *plain, const spec_ndsi_pokemon_t *pokemon) {
    const spec_ndsi_origin_t *origin = &pokemon->origin;
    plain[VERSION_OFFSET] = origin->version;
    plain[BALL_OFFSET] = origin->ball;
    uint32_t met_level = origin->met_level;
    met_level = spec_set_flag(met_level, TRAINER_FEMALE_BIT, pokemon->trainer.is_female);
    plain[MET_LEVEL_OFFSET] = (uint8_t)met_level;
    spec_write_u16_le(&plain[MET_LOCATION_OFFSET], origin->met_location);
    write_date(plain, MET_DATE_OFFSET, &origin->met_date);
    spec_write_u16_le(&plain[EGG_LOCATION_OFFSET], origin->egg_location);
    write_date(plain, EGG_DATE_OFFSET, &origin->egg_date);
    plain[ENCOUNTER_TYPE_OFFSET] = origin->encounter_type;
}

static void encode_party_data(uint8_t *plain, const spec_ndsi_party_data_t *party_data) {
    spec_write_u32_le(&plain[STATUS_OFFSET], spec_nds_encode_status(&party_data->status));
    plain[LEVEL_OFFSET] = party_data->level;
    spec_write_u16_le(&plain[CURRENT_HP_OFFSET], party_data->current_hp);
    for (size_t stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        spec_write_u16_le(&plain[STATS_OFFSET + stat * 2], party_data->stats[stat]);
    }
    spec_ndsi_encode_mail(&plain[MAIL_OFFSET], &party_data->mail);
}

spec_error_t spec_ndsi_encode_pokemon(uint8_t *record, size_t record_size,
                                      const spec_ndsi_pokemon_t *pokemon) {
    const char *unencodable_field = unencodable_field_of(pokemon);
    if (unencodable_field != nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, unencodable_field);
    }
    uint8_t plain[SPEC_NDSI_PARTY_RECORD_SIZE] = {};
    encode_header(plain, pokemon);
    encode_block_a(plain, pokemon);
    encode_block_b(plain, pokemon);
    encode_block_c(plain, pokemon);
    encode_block_d(plain, pokemon);
    encode_origin(plain, pokemon);
    encode_party_data(plain, &pokemon->party_data);
    uint16_t checksum = spec_sum_u16(&plain[BLOCKS_OFFSET], BLOCKS_SIZE);
    spec_write_u16_le(&plain[CHECKSUM_OFFSET], checksum);
    spec_shuffle_blocks(&plain[BLOCKS_OFFSET], BLOCK_SIZE, block_order_of(plain));
    spec_xor_with_random_stream(&plain[BLOCKS_OFFSET], BLOCKS_SIZE, checksum);
    spec_xor_with_random_stream(&plain[PARTY_DATA_OFFSET], PARTY_DATA_SIZE,
                                pokemon->personality.pid);
    memcpy(record, plain, record_size);
    return SPEC_OK;
}

// As Gen 4's Pokemon_FromBoxPokemon.
void spec_ndsi_fill_party_data(spec_ndsi_pokemon_t *pokemon) {
    bool has_party_data =
        pokemon->party_data.level != 0 || pokemon->party_data.stats[SPEC_STAT_HP] != 0;
    if (has_party_data || !has_species_data(pokemon->species)
        || pokemon->nature >= SPEC_NATURE_COUNT) {
        return;
    }
    pokemon->party_data = (spec_ndsi_party_data_t){};
    spec_ndsi_init_mail(&pokemon->party_data.mail);
    calculate_stats(pokemon);
}

spec_error_t spec_ndsi_read_pokemon(spec_ndsi_pokemon_t *pokemon, const uint8_t *raw,
                                    size_t raw_size) {
    if (!is_record_size(raw_size)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "raw_size is neither 136 nor 220");
    }
    spec_ndsi_decode_pokemon(pokemon, raw, raw_size);
    return SPEC_OK;
}

spec_error_t spec_ndsi_write_pokemon(uint8_t *raw, size_t raw_size,
                                     const spec_ndsi_pokemon_t *pokemon) {
    if (!is_record_size(raw_size)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "raw_size is neither 136 nor 220");
    }
    return spec_ndsi_encode_pokemon(raw, raw_size, pokemon);
}

uint8_t spec_ndsi_pokemon_get_level(const spec_ndsi_pokemon_t *pokemon) {
    if (!has_species_data(pokemon->species)) {
        return 0;
    }
    return spec_level_for_experience(species_data_of(pokemon->species, 0)->growth_rate,
                                     pokemon->experience);
}

spec_error_t spec_ndsi_pokemon_get_name(const spec_ndsi_pokemon_t *pokemon,
                                        char8_t name[static SPEC_NDSI_TEXT_BUFFER_SIZE]) {
    return spec_ndsi_text_to_utf8(name, pokemon->nickname, SPEC_NDSI_NICKNAME_SIZE);
}

// The game's PC refuses Pokémon holding mail.
bool spec_ndsi_pokemon_is_safe_to_box(const spec_ndsi_pokemon_t *pokemon) {
    bool is_holding_mail =
        pokemon->held_item >= FIRST_MAIL_ITEM && pokemon->held_item <= LAST_MAIL_ITEM;
    return !is_holding_mail;
}

spec_error_t spec_ndsi_pokemon_calculate_stats(spec_ndsi_pokemon_t *pokemon) {
    spec_error_t error = check_stat_inputs(pokemon);
    if (error != SPEC_OK) {
        return error;
    }
    calculate_stats(pokemon);
    return SPEC_OK;
}

// The game writes an egg's name over the old one, keeping what follows its terminator.
static spec_error_t write_egg_nickname(spec_ndsi_pokemon_t *pokemon) {
    // TODO: Determine egg names for languages other than English.
    spec_error_t error = spec_ndsi_text_from_utf8(pokemon->nickname, SPEC_NDSI_NICKNAME_SIZE,
                                                  ENGLISH_EGG_NICKNAME, pokemon->language);
    if (error == SPEC_OK) {
        pokemon->is_nicknamed = false;
    }
    return error;
}

// A new Pokémon's name: zeros after the terminator, and one more terminator in the last unit.
static spec_error_t write_species_nickname(spec_ndsi_pokemon_t *pokemon) {
    const char8_t *name = spec_gen5_species_name(pokemon->species, pokemon->language);
    if (name == nullptr) {
        // This should never happen.
        return spec_fail(SPEC_ERROR_UNKNOWN_NAME, "the species has no name in that language");
    }
    spec_error_t error = write_zero_padded_nickname(pokemon, name);
    if (error != SPEC_OK) {
        return error;
    }
    pokemon->nickname[SPEC_NDSI_NICKNAME_SIZE - 1] = END_OF_TEXT;
    pokemon->is_nicknamed = false;
    return SPEC_OK;
}

// As the games name a Pokémon; they show a Bad Egg's name without storing one.
spec_error_t spec_ndsi_pokemon_remove_nickname(spec_ndsi_pokemon_t *pokemon) {
    if (pokemon->is_bad_egg) {
        return spec_fail(SPEC_ERROR_UNKNOWN_NAME, "the games store no name for a Bad Egg");
    }
    if (pokemon->is_egg) {
        return write_egg_nickname(pokemon);
    }
    return write_species_nickname(pokemon);
}

spec_error_t spec_ndsi_pokemon_set_level(spec_ndsi_pokemon_t *pokemon, uint8_t level) {
    spec_error_t error = check_stat_inputs(pokemon);
    if (error != SPEC_OK) {
        return error;
    }
    if (level == 0 || level >= SPEC_LEVEL_COUNT) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "level is not 1 to 100");
    }
    spec_growth_rate_t growth_rate = species_data_of(pokemon->species, 0)->growth_rate;
    pokemon->experience = spec_experience_for_level(growth_rate, level);
    calculate_stats(pokemon);
    return SPEC_OK;
}

// The games set the flag by comparing the new name with the species name.
static bool is_species_name(const spec_ndsi_pokemon_t *pokemon) {
    const char8_t *name = spec_gen5_species_name(pokemon->species, pokemon->language);
    uint16_t species_name[SPEC_NDSI_NICKNAME_SIZE];
    if (name == nullptr
        || spec_ndsi_text_from_utf8(species_name, SPEC_NDSI_NICKNAME_SIZE, name, pokemon->language)
               != SPEC_OK) {
        return false;
    }
    for (size_t index = 0; index < SPEC_NDSI_NICKNAME_SIZE; ++index) {
        if (pokemon->nickname[index] != species_name[index]) {
            return false;
        }
        if (species_name[index] == END_OF_TEXT) {
            return true;
        }
    }
    return true;
}

// Retail saves show the field naming screen copying its whole buffer, zeros after the name.
spec_error_t spec_ndsi_pokemon_set_nickname(spec_ndsi_pokemon_t *pokemon, const char8_t *nickname,
                                            spec_naming_t naming) {
    if (pokemon->is_egg || pokemon->is_bad_egg) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "the games never name an egg");
    }
    // TODO: Check the Japanese nickname length.
    spec_error_t error = SPEC_OK;
    switch (naming) {
        case SPEC_NAMING_CAUGHT_OR_HATCHED:
            // TODO: Verify there are no other trash bytes.
            error = spec_ndsi_text_from_utf8(pokemon->nickname, SPEC_NDSI_NICKNAME_SIZE, nickname,
                                             pokemon->language);
            break;
        case SPEC_NAMING_NAME_RATER:
            // TODO: Check trash bytes on Gen 5 rename.
            error = write_zero_padded_nickname(pokemon, nickname);
            break;
        default:
            return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "naming is not a known way");
    }
    if (error != SPEC_OK) {
        return error;
    }
    pokemon->is_nicknamed = !is_species_name(pokemon);
    return SPEC_OK;
}

static bool is_gen5_game(spec_game_type_t type) {
    return type == SPEC_GAME_TYPE_BLACK_WHITE || type == SPEC_GAME_TYPE_BLACK2_WHITE2;
}

// Game types count up generation by generation, so the latest row begun by the type is in force.
static const spec_ndsi_species_row_t *latest_row(spec_game_type_t type, uint16_t species,
                                                 uint8_t form) {
    const spec_ndsi_species_row_t *latest = nullptr;
    for (size_t index = 0; index < spec_ndsi_species_row_count; ++index) {
        const spec_ndsi_species_row_t *row = &spec_ndsi_species_rows[index];
        bool applies = row->species == species && row->form == form && row->from_game <= type;
        if (applies && (latest == nullptr || row->from_game >= latest->from_game)) {
            latest = row;
        }
    }
    return latest;
}

const spec_ndsi_species_data_t *spec_ndsi_get_species_data(spec_game_type_t type, uint16_t species,
                                                           uint8_t form) {
    if (!is_gen5_game(type)) {
        return nullptr;
    }
    const spec_ndsi_species_row_t *row = latest_row(type, species, form);
    if (row == nullptr) {
        row = latest_row(type, species, 0);
    }
    if (row == nullptr) {
        return nullptr;
    }
    return &row->data;
}
