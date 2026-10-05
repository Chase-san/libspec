// Gen 4 Pokémon records: the encrypted codec, stats and names.

#include <string.h>

#include "nds/nds.h"
#include "nds/nds_internal.h"
#include "nds/tables.h"
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
constexpr size_t SHINY_LEAVES_OFFSET = 0x41;
constexpr size_t PLATINUM_EGG_LOCATION_OFFSET = 0x44;
constexpr size_t PLATINUM_MET_LOCATION_OFFSET = 0x46;

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
constexpr size_t HEARTGOLD_SOULSILVER_BALL_OFFSET = 0x86;
constexpr size_t WALKING_MOOD_OFFSET = 0x87;

constexpr size_t PARTY_DATA_OFFSET = 0x88;
constexpr size_t PARTY_DATA_SIZE = SPEC_NDS_PARTY_RECORD_SIZE - PARTY_DATA_OFFSET;
constexpr size_t STATUS_OFFSET = 0x88;
constexpr size_t LEVEL_OFFSET = 0x8C;
constexpr size_t BALL_CAPSULE_OFFSET = 0x8D;
constexpr size_t CURRENT_HP_OFFSET = 0x8E;
constexpr size_t STATS_OFFSET = 0x90;
constexpr size_t MAIL_OFFSET = 0x9C;
constexpr size_t SEALS_OFFSET = 0xD4;
constexpr size_t SEAL_SIZE = 3;

constexpr unsigned BAD_EGG_BIT = 2;
constexpr unsigned MARKINGS_BIT_COUNT = 6;
constexpr unsigned SINNOH_RIBBONS_BIT_COUNT = 28;
constexpr unsigned SUPER_CONTEST_RIBBONS_BIT_COUNT = 20;
constexpr unsigned SHINY_LEAVES_BIT_COUNT = 6;

constexpr unsigned IV_BIT_COUNT = 5;
constexpr unsigned IS_EGG_BIT = 30;
constexpr unsigned IS_NICKNAMED_BIT = 31;

constexpr unsigned FATEFUL_ENCOUNTER_BIT = 0;
constexpr unsigned GENDER_BIT = 1;
constexpr unsigned GENDER_BIT_COUNT = 2;
constexpr unsigned FORM_BIT = 3;
constexpr unsigned FORM_BIT_COUNT = 5;

constexpr unsigned POKERUS_DAYS_BIT = 0;
constexpr unsigned POKERUS_STRAIN_BIT = 4;
constexpr unsigned POKERUS_BIT_COUNT = 4;

constexpr unsigned MET_LEVEL_BIT_COUNT = 7;
constexpr unsigned TRAINER_FEMALE_BIT = 7;

constexpr unsigned SLEEP_TURNS_BIT = 0;
constexpr unsigned SLEEP_TURNS_BIT_COUNT = 3;
constexpr unsigned POISONED_BIT = 3;
constexpr unsigned BURNED_BIT = 4;
constexpr unsigned FROZEN_BIT = 5;
constexpr unsigned PARALYZED_BIT = 6;
constexpr unsigned BADLY_POISONED_BIT = 7;
constexpr unsigned TOXIC_TURNS_BIT = 8;
constexpr unsigned TOXIC_TURNS_BIT_COUNT = 4;

constexpr uint16_t SHEDINJA = 292;
constexpr uint16_t FIRST_MAIL_ITEM = 137;
constexpr uint16_t LAST_MAIL_ITEM = 148;
constexpr uint16_t FARAWAY_PLACE = 3002;

static bool get_flag(uint32_t word, unsigned bit) {
    return spec_get_bits(word, bit, 1) != 0;
}

static uint32_t set_flag(uint32_t word, unsigned bit, bool is_set) {
    return spec_set_bits(word, bit, 1, is_set);
}

// As Pokemon_EncryptData: the generator's draws, seeded by the key, mask each u16.
static void xor_with_random_stream(uint8_t *bytes, size_t size, uint32_t seed) {
    for (size_t offset = 0; offset < size; offset += 2) {
        uint16_t mask = spec_random(&seed);
        spec_write_u16_le(&bytes[offset], (uint16_t)(spec_read_u16_le(&bytes[offset]) ^ mask));
    }
}

// As the game orders the blocks: ((personality >> 13) & 31) % 24.
static size_t block_order_of(const uint8_t *record) {
    return (spec_read_u32_le(&record[PERSONALITY_OFFSET]) >> 13) & 31;
}

static uint16_t checksum_of(const uint8_t *plain) {
    uint16_t checksum = 0;
    for (size_t offset = BLOCKS_OFFSET; offset < BLOCKS_OFFSET + BLOCKS_SIZE; offset += 2) {
        checksum = (uint16_t)(checksum + spec_read_u16_le(&plain[offset]));
    }
    return checksum;
}

// As LocationIsDiamondPearlCompatible.
static bool is_diamond_pearl_location(uint16_t location) {
    return (location >= 1 && location <= 111) || (location >= 2000 && location <= 2010)
           || (location >= 3000 && location <= 3076);
}

// As Platinum's GetBoxMonData: a faraway place may hold a location only Platinum's field names.
static uint16_t location_at(const uint8_t *plain, size_t diamond_pearl_offset,
                            size_t platinum_offset) {
    uint16_t diamond_pearl_location = spec_read_u16_le(&plain[diamond_pearl_offset]);
    uint16_t platinum_location = spec_read_u16_le(&plain[platinum_offset]);
    if (diamond_pearl_location == FARAWAY_PLACE && platinum_location != 0) {
        return platinum_location;
    }
    return diamond_pearl_location;
}

// As Platinum's SetBoxMonData: Diamond and Pearl see their own locations, and a faraway place.
static void write_location(uint8_t *plain, size_t diamond_pearl_offset, size_t platinum_offset,
                           uint16_t location) {
    bool can_diamond_pearl_name = location == 0 || is_diamond_pearl_location(location);
    spec_write_u16_le(&plain[diamond_pearl_offset],
                      can_diamond_pearl_name ? location : FARAWAY_PLACE);
    spec_write_u16_le(&plain[platinum_offset], location);
}

static bool is_heartgold_soulsilver_origin(spec_version_t version) {
    return version == SPEC_VERSION_HEARTGOLD || version == SPEC_VERSION_SOULSILVER;
}

// As HeartGold and SoulSilver's SetBoxMonData: the other games see their balls as Poké Balls.
static spec_ball_t diamond_pearl_ball_of(spec_ball_t ball) {
    bool is_heartgold_soulsilver_ball = ball >= SPEC_BALL_FAST && ball <= SPEC_BALL_SPORT;
    return is_heartgold_soulsilver_ball ? SPEC_BALL_POKE : ball;
}

static spec_nds_date_t date_at(const uint8_t *plain, size_t offset) {
    return (spec_nds_date_t){
        .year = plain[offset],
        .month = plain[offset + 1],
        .day = plain[offset + 2],
    };
}

static void write_date(uint8_t *plain, size_t offset, const spec_nds_date_t *date) {
    plain[offset] = date->year;
    plain[offset + 1] = date->month;
    plain[offset + 2] = date->day;
}

static void decode_header(spec_nds_pokemon_t *pokemon, const uint8_t *plain) {
    pokemon->personality.pid = spec_read_u32_le(&plain[PERSONALITY_OFFSET]);
    pokemon->is_bad_egg = get_flag(spec_read_u16_le(&plain[FLAGS_OFFSET]), BAD_EGG_BIT);
}

static void decode_block_a(spec_nds_pokemon_t *pokemon, const uint8_t *plain) {
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
static void decode_block_b(spec_nds_pokemon_t *pokemon, const uint8_t *plain) {
    for (size_t move = 0; move < SPEC_NDS_MOVE_COUNT; ++move) {
        pokemon->moves[move].id = spec_read_u16_le(&plain[MOVES_OFFSET + move * 2]);
        pokemon->moves[move].pp = plain[MOVE_PP_OFFSET + move];
        pokemon->moves[move].pp_ups = plain[PP_UPS_OFFSET + move];
    }
    uint32_t ivs = spec_read_u32_le(&plain[IVS_OFFSET]);
    for (unsigned stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        pokemon->ivs[stat] = (uint8_t)spec_get_bits(ivs, stat * IV_BIT_COUNT, IV_BIT_COUNT);
    }
    pokemon->is_egg = get_flag(ivs, IS_EGG_BIT);
    pokemon->is_nicknamed = get_flag(ivs, IS_NICKNAMED_BIT);
    pokemon->hoenn_ribbons = spec_read_u32_le(&plain[HOENN_RIBBONS_OFFSET]);
    pokemon->is_fateful_encounter = get_flag(plain[FORM_OFFSET], FATEFUL_ENCOUNTER_BIT);
    pokemon->form = (uint8_t)spec_get_bits(plain[FORM_OFFSET], FORM_BIT, FORM_BIT_COUNT);
    pokemon->shiny_leaves =
        (uint8_t)spec_get_bits(plain[SHINY_LEAVES_OFFSET], 0, SHINY_LEAVES_BIT_COUNT);
}

static void decode_block_c(spec_nds_pokemon_t *pokemon, const uint8_t *plain) {
    spec_nds_read_text(pokemon->nickname, &plain[NICKNAME_OFFSET], SPEC_NDS_NICKNAME_SIZE);
    pokemon->super_contest_ribbons = spec_get_bits(
        spec_read_u32_le(&plain[SUPER_CONTEST_RIBBONS_OFFSET]), 0, SUPER_CONTEST_RIBBONS_BIT_COUNT);
}

static void decode_block_d(spec_nds_pokemon_t *pokemon, const uint8_t *plain) {
    spec_nds_read_text(pokemon->trainer.name, &plain[TRAINER_NAME_OFFSET],
                       SPEC_NDS_TRAINER_NAME_SIZE);
    pokemon->trainer.is_female = get_flag(plain[MET_LEVEL_OFFSET], TRAINER_FEMALE_BIT);
    pokemon->pokerus.strain =
        (uint8_t)spec_get_bits(plain[POKERUS_OFFSET], POKERUS_STRAIN_BIT, POKERUS_BIT_COUNT);
    pokemon->pokerus.days =
        (uint8_t)spec_get_bits(plain[POKERUS_OFFSET], POKERUS_DAYS_BIT, POKERUS_BIT_COUNT);
    pokemon->walking_mood = (int8_t)plain[WALKING_MOOD_OFFSET];
}

// The origin spreads over blocks B, C and D.
static void decode_origin(spec_nds_origin_t *origin, const uint8_t *plain) {
    origin->version = (spec_version_t)plain[VERSION_OFFSET];
    origin->ball = (spec_ball_t)plain[BALL_OFFSET];
    // As HeartGold and SoulSilver's GetBoxMonData.
    uint8_t heartgold_soulsilver_ball = plain[HEARTGOLD_SOULSILVER_BALL_OFFSET];
    if (is_heartgold_soulsilver_origin(origin->version) && heartgold_soulsilver_ball != 0) {
        origin->ball = (spec_ball_t)heartgold_soulsilver_ball;
    }
    origin->met_level = (uint8_t)spec_get_bits(plain[MET_LEVEL_OFFSET], 0, MET_LEVEL_BIT_COUNT);
    origin->met_location = location_at(plain, MET_LOCATION_OFFSET, PLATINUM_MET_LOCATION_OFFSET);
    origin->met_date = date_at(plain, MET_DATE_OFFSET);
    origin->egg_location = location_at(plain, EGG_LOCATION_OFFSET, PLATINUM_EGG_LOCATION_OFFSET);
    origin->egg_date = date_at(plain, EGG_DATE_OFFSET);
    origin->encounter_type = plain[ENCOUNTER_TYPE_OFFSET];
}

static void decode_status(spec_nds_status_t *status, uint32_t word) {
    status->sleep_turns = (uint8_t)spec_get_bits(word, SLEEP_TURNS_BIT, SLEEP_TURNS_BIT_COUNT);
    status->is_poisoned = get_flag(word, POISONED_BIT);
    status->is_burned = get_flag(word, BURNED_BIT);
    status->is_frozen = get_flag(word, FROZEN_BIT);
    status->is_paralyzed = get_flag(word, PARALYZED_BIT);
    status->is_badly_poisoned = get_flag(word, BADLY_POISONED_BIT);
    status->toxic_turns = (uint8_t)spec_get_bits(word, TOXIC_TURNS_BIT, TOXIC_TURNS_BIT_COUNT);
}

static void decode_party_data(spec_nds_party_data_t *party_data, const uint8_t *plain) {
    decode_status(&party_data->status, spec_read_u32_le(&plain[STATUS_OFFSET]));
    party_data->level = plain[LEVEL_OFFSET];
    party_data->ball_capsule = plain[BALL_CAPSULE_OFFSET];
    party_data->current_hp = spec_read_u16_le(&plain[CURRENT_HP_OFFSET]);
    for (size_t stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        party_data->stats[stat] = spec_read_u16_le(&plain[STATS_OFFSET + stat * 2]);
    }
    spec_nds_decode_mail(&party_data->mail, &plain[MAIL_OFFSET]);
    for (size_t seal = 0; seal < SPEC_NDS_SEAL_COUNT; ++seal) {
        const uint8_t *seal_bytes = &plain[SEALS_OFFSET + seal * SEAL_SIZE];
        party_data->seals[seal] = (spec_nds_seal_t){seal_bytes[0], seal_bytes[1], seal_bytes[2]};
    }
}

static const char *unencodable_field_of(const spec_nds_pokemon_t *pokemon) {
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
    if (!spec_fits_in_bits(pokemon->shiny_leaves, SHINY_LEAVES_BIT_COUNT)) {
        return "shiny_leaves do not fit in 6 bits";
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
    return spec_nds_unencodable_mail_field_of(&pokemon->party_data.mail);
}

static void encode_header(uint8_t *plain, const spec_nds_pokemon_t *pokemon) {
    spec_write_u32_le(&plain[PERSONALITY_OFFSET], pokemon->personality.pid);
    spec_write_u16_le(&plain[FLAGS_OFFSET],
                      (uint16_t)set_flag(0, BAD_EGG_BIT, pokemon->is_bad_egg));
}

static void encode_block_a(uint8_t *plain, const spec_nds_pokemon_t *pokemon) {
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
static uint8_t stored_gender_of(const spec_nds_pokemon_t *pokemon) {
    if (pokemon->species == 0) {
        return 0;
    }
    return spec_nds_decode_personality(pokemon->personality.pid, pokemon->species,
                                       &pokemon->trainer)
        .gender;
}

static void encode_block_b(uint8_t *plain, const spec_nds_pokemon_t *pokemon) {
    for (size_t move = 0; move < SPEC_NDS_MOVE_COUNT; ++move) {
        spec_write_u16_le(&plain[MOVES_OFFSET + move * 2], pokemon->moves[move].id);
        plain[MOVE_PP_OFFSET + move] = pokemon->moves[move].pp;
        plain[PP_UPS_OFFSET + move] = pokemon->moves[move].pp_ups;
    }
    uint32_t ivs = 0;
    for (unsigned stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        ivs = spec_set_bits(ivs, stat * IV_BIT_COUNT, IV_BIT_COUNT, pokemon->ivs[stat]);
    }
    ivs = set_flag(ivs, IS_EGG_BIT, pokemon->is_egg);
    ivs = set_flag(ivs, IS_NICKNAMED_BIT, pokemon->is_nicknamed);
    uint32_t form_byte = 0;
    form_byte = set_flag(form_byte, FATEFUL_ENCOUNTER_BIT, pokemon->is_fateful_encounter);
    form_byte = spec_set_bits(form_byte, GENDER_BIT, GENDER_BIT_COUNT, stored_gender_of(pokemon));
    form_byte = spec_set_bits(form_byte, FORM_BIT, FORM_BIT_COUNT, pokemon->form);
    spec_write_u32_le(&plain[IVS_OFFSET], ivs);
    spec_write_u32_le(&plain[HOENN_RIBBONS_OFFSET], pokemon->hoenn_ribbons);
    plain[FORM_OFFSET] = (uint8_t)form_byte;
    plain[SHINY_LEAVES_OFFSET] = pokemon->shiny_leaves;
}

static void encode_block_c(uint8_t *plain, const spec_nds_pokemon_t *pokemon) {
    spec_nds_write_text(&plain[NICKNAME_OFFSET], pokemon->nickname, SPEC_NDS_NICKNAME_SIZE);
    spec_write_u32_le(&plain[SUPER_CONTEST_RIBBONS_OFFSET], pokemon->super_contest_ribbons);
}

static void encode_block_d(uint8_t *plain, const spec_nds_pokemon_t *pokemon) {
    spec_nds_write_text(&plain[TRAINER_NAME_OFFSET], pokemon->trainer.name,
                        SPEC_NDS_TRAINER_NAME_SIZE);
    uint32_t pokerus = 0;
    pokerus =
        spec_set_bits(pokerus, POKERUS_STRAIN_BIT, POKERUS_BIT_COUNT, pokemon->pokerus.strain);
    pokerus = spec_set_bits(pokerus, POKERUS_DAYS_BIT, POKERUS_BIT_COUNT, pokemon->pokerus.days);
    plain[POKERUS_OFFSET] = (uint8_t)pokerus;
    plain[WALKING_MOOD_OFFSET] = (uint8_t)pokemon->walking_mood;
}

static void encode_origin(uint8_t *plain, const spec_nds_pokemon_t *pokemon) {
    const spec_nds_origin_t *origin = &pokemon->origin;
    plain[VERSION_OFFSET] = origin->version;
    plain[BALL_OFFSET] = diamond_pearl_ball_of(origin->ball);
    if (is_heartgold_soulsilver_origin(origin->version)) {
        plain[HEARTGOLD_SOULSILVER_BALL_OFFSET] = origin->ball;
    }
    uint32_t met_level = origin->met_level;
    met_level = set_flag(met_level, TRAINER_FEMALE_BIT, pokemon->trainer.is_female);
    plain[MET_LEVEL_OFFSET] = (uint8_t)met_level;
    write_location(plain, MET_LOCATION_OFFSET, PLATINUM_MET_LOCATION_OFFSET, origin->met_location);
    write_date(plain, MET_DATE_OFFSET, &origin->met_date);
    write_location(plain, EGG_LOCATION_OFFSET, PLATINUM_EGG_LOCATION_OFFSET, origin->egg_location);
    write_date(plain, EGG_DATE_OFFSET, &origin->egg_date);
    plain[ENCOUNTER_TYPE_OFFSET] = origin->encounter_type;
}

static uint32_t encode_status(const spec_nds_status_t *status) {
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

static void encode_party_data(uint8_t *plain, const spec_nds_party_data_t *party_data) {
    spec_write_u32_le(&plain[STATUS_OFFSET], encode_status(&party_data->status));
    plain[LEVEL_OFFSET] = party_data->level;
    plain[BALL_CAPSULE_OFFSET] = party_data->ball_capsule;
    spec_write_u16_le(&plain[CURRENT_HP_OFFSET], party_data->current_hp);
    for (size_t stat = 0; stat < SPEC_STAT_COUNT; ++stat) {
        spec_write_u16_le(&plain[STATS_OFFSET + stat * 2], party_data->stats[stat]);
    }
    spec_nds_encode_mail(&plain[MAIL_OFFSET], &party_data->mail);
    for (size_t seal = 0; seal < SPEC_NDS_SEAL_COUNT; ++seal) {
        uint8_t *seal_bytes = &plain[SEALS_OFFSET + seal * SEAL_SIZE];
        seal_bytes[0] = party_data->seals[seal].type;
        seal_bytes[1] = party_data->seals[seal].x;
        seal_bytes[2] = party_data->seals[seal].y;
    }
}

void spec_nds_decode_pokemon(spec_nds_pokemon_t *pokemon, const uint8_t *record,
                             size_t record_size) {
    uint8_t plain[SPEC_NDS_PARTY_RECORD_SIZE] = {};
    memcpy(plain, record, record_size);
    uint16_t stored_checksum = spec_read_u16_le(&plain[CHECKSUM_OFFSET]);
    xor_with_random_stream(&plain[BLOCKS_OFFSET], BLOCKS_SIZE, stored_checksum);
    xor_with_random_stream(&plain[PARTY_DATA_OFFSET], PARTY_DATA_SIZE,
                           spec_read_u32_le(&plain[PERSONALITY_OFFSET]));
    spec_unshuffle_blocks(&plain[BLOCKS_OFFSET], BLOCK_SIZE, block_order_of(plain));
    *pokemon = (spec_nds_pokemon_t){};
    decode_header(pokemon, plain);
    decode_block_a(pokemon, plain);
    decode_block_b(pokemon, plain);
    decode_block_c(pokemon, plain);
    decode_block_d(pokemon, plain);
    decode_origin(&pokemon->origin, plain);
    if (record_size == SPEC_NDS_PARTY_RECORD_SIZE) {
        decode_party_data(&pokemon->party_data, plain);
    }
    // A failed checksum reads as a Bad Egg, as in the game.
    if (checksum_of(plain) != stored_checksum) {
        pokemon->is_bad_egg = true;
        pokemon->is_egg = true;
    }
    if (pokemon->species != 0) {
        pokemon->personality = spec_nds_decode_personality(pokemon->personality.pid,
                                                           pokemon->species, &pokemon->trainer);
        pokemon->iv_method = spec_find_iv_method(pokemon->personality.pid, pokemon->ivs);
    }
}

spec_error_t spec_nds_encode_pokemon(uint8_t *record, size_t record_size,
                                     const spec_nds_pokemon_t *pokemon) {
    const char *unencodable_field = unencodable_field_of(pokemon);
    if (unencodable_field != nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, unencodable_field);
    }
    uint8_t plain[SPEC_NDS_PARTY_RECORD_SIZE] = {};
    encode_header(plain, pokemon);
    encode_block_a(plain, pokemon);
    encode_block_b(plain, pokemon);
    encode_block_c(plain, pokemon);
    encode_block_d(plain, pokemon);
    encode_origin(plain, pokemon);
    encode_party_data(plain, &pokemon->party_data);
    uint16_t checksum = checksum_of(plain);
    spec_write_u16_le(&plain[CHECKSUM_OFFSET], checksum);
    spec_shuffle_blocks(&plain[BLOCKS_OFFSET], BLOCK_SIZE, block_order_of(plain));
    xor_with_random_stream(&plain[BLOCKS_OFFSET], BLOCKS_SIZE, checksum);
    xor_with_random_stream(&plain[PARTY_DATA_OFFSET], PARTY_DATA_SIZE, pokemon->personality.pid);
    memcpy(record, plain, record_size);
    return SPEC_OK;
}

static bool is_record_size(size_t raw_size) {
    return raw_size == SPEC_NDS_BOX_RECORD_SIZE || raw_size == SPEC_NDS_PARTY_RECORD_SIZE;
}

spec_error_t spec_nds_read_pokemon(spec_nds_pokemon_t *pokemon, const uint8_t *raw,
                                   size_t raw_size) {
    if (!is_record_size(raw_size)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "raw_size is neither 136 nor 236");
    }
    spec_nds_decode_pokemon(pokemon, raw, raw_size);
    return SPEC_OK;
}

spec_error_t spec_nds_write_pokemon(uint8_t *raw, size_t raw_size,
                                    const spec_nds_pokemon_t *pokemon) {
    if (!is_record_size(raw_size)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "raw_size is neither 136 nor 236");
    }
    return spec_nds_encode_pokemon(raw, raw_size, pokemon);
}

static bool has_species_data(uint16_t species) {
    return species != 0 && species < SPEC_NDS_SPECIES_COUNT;
}

// As Platinum and HeartGold and SoulSilver's alternate forms; Diamond and Pearl lack some.
static const uint8_t *base_stats_of(uint16_t species, uint8_t form) {
    for (size_t index = 0; index < SPEC_NDS_FORM_DATA_COUNT; ++index) {
        const spec_nds_form_data_t *form_data = &spec_nds_form_data[index];
        if (form_data->species == species && form_data->form == form) {
            return form_data->base_stats;
        }
    }
    return spec_nds_species_data[species].base_stats;
}

static uint16_t stat_of(const spec_nds_pokemon_t *pokemon, const uint8_t *base_stats,
                        spec_stat_t stat, uint8_t level) {
    if (stat == SPEC_STAT_HP && pokemon->species == SHEDINJA) {
        return 1;
    }
    // The nature comes from the pid, since personality.nature is ignored on write.
    spec_nature_t nature = (spec_nature_t)(pokemon->personality.pid % SPEC_NATURE_COUNT);
    return spec_calculate_stat(stat, base_stats[stat], pokemon->ivs[stat], pokemon->evs[stat],
                               level, nature);
}

// As HeartGold and SoulSilver's CalcMonStats, which clamps where the others subtract.
static uint16_t current_hp_after(const spec_nds_pokemon_t *pokemon, uint16_t old_max_hp,
                                 uint16_t new_max_hp) {
    uint16_t current_hp = pokemon->party_data.current_hp;
    bool is_fainted = current_hp == 0 && old_max_hp != 0;
    if (is_fainted) {
        return 0;
    }
    if (pokemon->species == SHEDINJA) {
        return 1;
    }
    if (current_hp == 0) {
        return new_max_hp;
    }
    if (new_max_hp < old_max_hp) {
        return current_hp < new_max_hp ? current_hp : new_max_hp;
    }
    return (uint16_t)(current_hp + new_max_hp - old_max_hp);
}

static void calculate_stats(spec_nds_pokemon_t *pokemon) {
    const spec_nds_species_data_t *species_data = &spec_nds_species_data[pokemon->species];
    const uint8_t *base_stats = base_stats_of(pokemon->species, pokemon->form);
    uint8_t level = spec_level_for_experience(species_data->growth_rate, pokemon->experience);
    uint16_t old_max_hp = pokemon->party_data.stats[SPEC_STAT_HP];
    uint16_t new_max_hp = stat_of(pokemon, base_stats, SPEC_STAT_HP, level);
    pokemon->party_data.current_hp = current_hp_after(pokemon, old_max_hp, new_max_hp);
    pokemon->party_data.level = level;
    pokemon->party_data.stats[SPEC_STAT_HP] = new_max_hp;
    for (spec_stat_t stat = SPEC_STAT_ATTACK; stat < SPEC_STAT_COUNT; ++stat) {
        pokemon->party_data.stats[stat] = stat_of(pokemon, base_stats, stat, level);
    }
}

spec_error_t spec_nds_pokemon_calculate_stats(spec_nds_pokemon_t *pokemon) {
    if (!has_species_data(pokemon->species)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "species has no species data");
    }
    calculate_stats(pokemon);
    return SPEC_OK;
}

// As Pokemon_FromBoxPokemon.
void spec_nds_fill_party_data(spec_nds_pokemon_t *pokemon) {
    bool has_party_data =
        pokemon->party_data.level != 0 || pokemon->party_data.stats[SPEC_STAT_HP] != 0;
    if (has_party_data || !has_species_data(pokemon->species)) {
        return;
    }
    pokemon->party_data = (spec_nds_party_data_t){.mail = spec_nds_no_mail()};
    calculate_stats(pokemon);
}

spec_error_t spec_nds_pokemon_get_name(const spec_nds_pokemon_t *pokemon,
                                       char8_t name[static SPEC_NDS_TEXT_BUFFER_SIZE]) {
    return spec_nds_text_to_utf8(name, pokemon->nickname, SPEC_NDS_NICKNAME_SIZE);
}

spec_error_t spec_nds_pokemon_set_name(spec_nds_pokemon_t *pokemon, const char8_t *name) {
    return spec_nds_text_from_utf8(pokemon->nickname, SPEC_NDS_NICKNAME_SIZE, name,
                                   pokemon->language);
}

// The game's PC refuses Pokémon holding mail.
bool spec_nds_is_safe_to_box(const spec_nds_pokemon_t *pokemon) {
    bool is_holding_mail =
        pokemon->held_item >= FIRST_MAIL_ITEM && pokemon->held_item <= LAST_MAIL_ITEM;
    return !is_holding_mail;
}
