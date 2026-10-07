// Checks the lookups an editor builds its lists from: box geometry, game limits, species and move
// data, and save identification.

#include <stdio.h>
#include <string.h>

#include "3ds/3ds.h"
#include "gb/gb.h"
#include "gba/gba.h"
#include "gbc/gbc.h"
#include "nds/nds.h"
#include "ndsi/ndsi.h"
#include "spec_internal.h"

static size_t failure_count;

static void check(bool is_passing, const char *what) {
    if (!is_passing) {
        ++failure_count;
        printf("failed: %s\n", what);
    }
}

static void check_box_geometry(void) {
    check(spec_gb_box_count(SPEC_LANGUAGE_JAPANESE) == 8
              && spec_gb_box_capacity(SPEC_LANGUAGE_JAPANESE) == 30
              && spec_gb_box_count(SPEC_LANGUAGE_ITALIAN) == 12
              && spec_gb_box_capacity(SPEC_LANGUAGE_ITALIAN) == 20
              && spec_gb_box_count(SPEC_LANGUAGE_KOREAN) == 0,
          "Gen 1 boxes are 8 of 30 in Japanese, 12 of 20 elsewhere");
    check(spec_gbc_box_count(SPEC_LANGUAGE_JAPANESE) == 9
              && spec_gbc_box_capacity(SPEC_LANGUAGE_JAPANESE) == 30
              && spec_gbc_box_count(SPEC_LANGUAGE_GERMAN) == 14
              && spec_gbc_box_capacity(SPEC_LANGUAGE_GERMAN) == 20,
          "Gen 2 boxes are 9 of 30 in Japanese, 14 of 20 elsewhere");
    check(spec_3ds_box_count(SPEC_GAME_TYPE_OMEGA_RUBY_ALPHA_SAPPHIRE) == 31
              && spec_3ds_box_count(SPEC_GAME_TYPE_ULTRA_SUN_ULTRA_MOON) == 32
              && spec_3ds_daycare_count(SPEC_GAME_TYPE_X_Y) == 1
              && spec_3ds_daycare_count(SPEC_GAME_TYPE_OMEGA_RUBY_ALPHA_SAPPHIRE) == 2
              && spec_3ds_box_count(SPEC_GAME_TYPE_PLATINUM) == 0,
          "3DS games have 31 or 32 boxes, and one daycare but Omega Ruby and Alpha Sapphire's two");
}

static void check_game_limits(void) {
    check(spec_last_species(SPEC_GAME_TYPE_YELLOW) == 151
              && spec_last_species(SPEC_GAME_TYPE_SUN_MOON) == 802
              && spec_last_species(SPEC_GAME_TYPE_ULTRA_SUN_ULTRA_MOON) == 807,
          "species end at Mew, Marshadow and Zeraora");
    check(spec_last_move(SPEC_GAME_TYPE_X_Y) == 617
              && spec_last_move(SPEC_GAME_TYPE_OMEGA_RUBY_ALPHA_SAPPHIRE) == 621,
          "X and Y's moves end at Light of Ruin, Omega Ruby's at Hyperspace Fury");
    check(spec_last_ability(SPEC_GAME_TYPE_CRYSTAL) == 0
              && spec_last_ability(SPEC_GAME_TYPE_EMERALD) == 76
              && spec_last_ability(SPEC_GAME_TYPE_COUNT) == 0,
          "Game Boy games have no abilities, and Gen 3's end at Air Lock");
}

static bool has_types(const spec_type_t types[static SPEC_SPECIES_TYPE_COUNT], spec_type_t first,
                      spec_type_t second) {
    return types[0] == first && types[1] == second;
}

static void check_game_boy_species_data(void) {
    constexpr uint16_t DRAGONITE = 149;
    constexpr uint8_t MAGNEMITE = 81;
    spec_gb_species_t dragonite = spec_gb_species_from_national(DRAGONITE);
    const spec_gb_species_data_t *red_blue =
        spec_gb_get_species_data(SPEC_GAME_TYPE_RED_BLUE, dragonite);
    const spec_gb_species_data_t *yellow =
        spec_gb_get_species_data(SPEC_GAME_TYPE_YELLOW, dragonite);
    check(red_blue != nullptr && yellow != nullptr && red_blue->catch_rate == 45
              && yellow->catch_rate == 9
              && has_types(yellow->types, SPEC_TYPE_DRAGON, SPEC_TYPE_FLYING)
              && spec_gb_get_species_data(SPEC_GAME_TYPE_GOLD_SILVER, dragonite) == nullptr,
          "Yellow alone gives Dragonite a catch rate of 9");
    const spec_gbc_species_data_t *magnemite =
        spec_gbc_get_species_data(SPEC_GAME_TYPE_CRYSTAL, MAGNEMITE);
    check(magnemite != nullptr && has_types(magnemite->types, SPEC_TYPE_ELECTRIC, SPEC_TYPE_STEEL)
              && magnemite->egg_cycles == 20
              && spec_gbc_get_species_data(SPEC_GAME_TYPE_CRYSTAL, 0) == nullptr,
          "Gen 2 makes Magnemite Electric and Steel");
}

static void check_set_species(void) {
    constexpr uint16_t MAGIKARP = 129;
    constexpr uint16_t GYARADOS = 130;
    spec_gb_pokemon_t pokemon = {.species = spec_gb_species_from_national(MAGIKARP),
                                 .types = {SPEC_TYPE_WATER, SPEC_TYPE_WATER},
                                 .catch_rate = 255};
    bool is_set =
        spec_gb_pokemon_set_species(&pokemon, spec_gb_species_from_national(GYARADOS)) == SPEC_OK;
    check(is_set && has_types(pokemon.types, SPEC_TYPE_WATER, SPEC_TYPE_FLYING)
              && pokemon.catch_rate == 255,
          "setting the species changes the types and keeps the catch rate");
    check(spec_gb_pokemon_set_species(&pokemon, 0) != SPEC_OK, "Gen 1 refuses species it lacks");
}

static void check_gen3_to_gen5_species_data(void) {
    constexpr uint16_t RAYQUAZA = 384;
    constexpr uint16_t AIR_LOCK = 76;
    constexpr uint16_t GIRATINA = 487;
    constexpr uint16_t LEVITATE = 26;
    constexpr uint16_t PRESSURE = 46;
    constexpr uint16_t KYUREM = 646;
    constexpr uint16_t TURBOBLAZE = 163;
    const spec_gba_species_data_t *rayquaza =
        spec_gba_get_species_data(SPEC_GAME_TYPE_EMERALD, spec_gba_species_from_national(RAYQUAZA));
    check(rayquaza != nullptr && rayquaza->abilities[0] == AIR_LOCK && rayquaza->abilities[1] == 0
              && rayquaza->base_friendship == 0,
          "Gen 3 abilities are numbered as later games number them");
    const spec_nds_species_data_t *diamond_pearl =
        spec_nds_get_species_data(SPEC_GAME_TYPE_DIAMOND_PEARL, GIRATINA, 1);
    const spec_nds_species_data_t *platinum =
        spec_nds_get_species_data(SPEC_GAME_TYPE_PLATINUM, GIRATINA, 1);
    check(diamond_pearl != nullptr && platinum != nullptr && diamond_pearl->abilities[0] == PRESSURE
              && platinum->abilities[0] == LEVITATE
              && has_types(platinum->types, SPEC_TYPE_GHOST, SPEC_TYPE_DRAGON),
          "Giratina's Origin Forme begins in Platinum");
    const spec_ndsi_species_data_t *black_white =
        spec_ndsi_get_species_data(SPEC_GAME_TYPE_BLACK_WHITE, KYUREM, 1);
    const spec_ndsi_species_data_t *black2_white2 =
        spec_ndsi_get_species_data(SPEC_GAME_TYPE_BLACK2_WHITE2, KYUREM, 1);
    check(black_white != nullptr && black2_white2 != nullptr
              && black_white->abilities[0] == PRESSURE && black2_white2->abilities[0] == TURBOBLAZE
              && spec_ndsi_get_species_data(SPEC_GAME_TYPE_PLATINUM, KYUREM, 0) == nullptr,
          "White Kyurem begins in Black 2 and White 2");
}

static void check_3ds_species_data(void) {
    constexpr uint16_t CLEFAIRY = 35;
    constexpr uint16_t GENGAR = 94;
    constexpr uint16_t LEVITATE = 26;
    constexpr uint16_t CURSED_BODY = 130;
    constexpr uint16_t MARSHADOW = 802;
    constexpr uint16_t POIPOLE = 803;
    const spec_3ds_species_data_t *clefairy =
        spec_3ds_get_species_data(SPEC_GAME_TYPE_X_Y, CLEFAIRY, 0);
    check(clefairy != nullptr && has_types(clefairy->types, SPEC_TYPE_FAIRY, SPEC_TYPE_FAIRY),
          "Gen 6 makes Clefairy Fairy");
    const spec_3ds_species_data_t *x_y = spec_3ds_get_species_data(SPEC_GAME_TYPE_X_Y, GENGAR, 0);
    const spec_3ds_species_data_t *sun_moon =
        spec_3ds_get_species_data(SPEC_GAME_TYPE_SUN_MOON, GENGAR, 0);
    check(x_y != nullptr && sun_moon != nullptr && x_y->abilities[0] == LEVITATE
              && sun_moon->abilities[0] == CURSED_BODY,
          "Gen 7 gives Gengar Cursed Body");
    check(spec_3ds_get_species_data(SPEC_GAME_TYPE_SUN_MOON, MARSHADOW, 0) != nullptr
              && spec_3ds_get_species_data(SPEC_GAME_TYPE_SUN_MOON, POIPOLE, 0) == nullptr
              && spec_3ds_get_species_data(SPEC_GAME_TYPE_ULTRA_SUN_ULTRA_MOON, POIPOLE, 0)
                     != nullptr,
          "Poipole begins in Ultra Sun and Ultra Moon");
}

static void check_migration_ids(void) {
    constexpr uint16_t GB_BICYCLE = 6;
    constexpr uint16_t GB_TM01 = 0xC9;
    constexpr uint16_t GBC_KINGS_ROCK = 82;
    constexpr uint16_t GBA_MASTER_BALL = 1;
    // Gen 4 to 7's numbers, in which Bicycle is Bike from the 3DS games on.
    constexpr uint16_t BIKE = 450;
    constexpr uint16_t KINGS_ROCK = 221;
    constexpr uint16_t MASTER_BALL = 1;
    check(spec_gb_item_get_migration_id(GB_BICYCLE) == BIKE
              && spec_gb_item_get_migration_id(GB_TM01) == 0
              && spec_gbc_item_get_migration_id(GBC_KINGS_ROCK) == KINGS_ROCK
              && spec_gba_item_get_migration_id(GBA_MASTER_BALL) == MASTER_BALL
              && spec_gba_item_get_migration_id(0) == 0,
          "Game Boy and Gen 3 items have later numbers, but for TMs and HMs");
}

static bool is_move(const spec_move_data_t *move_data, spec_type_t type, uint8_t pp) {
    return move_data != nullptr && move_data->type == type && move_data->pp == pp;
}

static void check_move_data(void) {
    constexpr uint16_t STRUGGLE = 165;
    constexpr uint16_t CURSE = 174;
    constexpr uint16_t CHARM = 204;
    constexpr uint16_t LIGHT_OF_RUIN = 617;
    check(is_move(spec_get_move_data(SPEC_GAME_TYPE_RED_BLUE, STRUGGLE), SPEC_TYPE_NORMAL, 10)
              && is_move(spec_get_move_data(SPEC_GAME_TYPE_GOLD_SILVER, STRUGGLE), SPEC_TYPE_NORMAL,
                         1),
          "Struggle's PP is 10 in Gen 1, 1 after");
    check(
        spec_get_move_data(SPEC_GAME_TYPE_YELLOW, CURSE) == nullptr
            && is_move(spec_get_move_data(SPEC_GAME_TYPE_HEARTGOLD_SOULSILVER, CURSE),
                       SPEC_TYPE_MYSTERY, 10)
            && is_move(spec_get_move_data(SPEC_GAME_TYPE_BLACK_WHITE, CURSE), SPEC_TYPE_GHOST, 10),
        "Curse is ??? until Gen 5 makes it Ghost");
    check(is_move(spec_get_move_data(SPEC_GAME_TYPE_BLACK2_WHITE2, CHARM), SPEC_TYPE_NORMAL, 20)
              && is_move(spec_get_move_data(SPEC_GAME_TYPE_X_Y, CHARM), SPEC_TYPE_FAIRY, 20),
          "Gen 6 makes Charm Fairy");
    check(spec_get_move_data(SPEC_GAME_TYPE_BLACK2_WHITE2, LIGHT_OF_RUIN) == nullptr
              && spec_get_move_data(SPEC_GAME_TYPE_X_Y, LIGHT_OF_RUIN) != nullptr
              && spec_get_move_data(SPEC_GAME_TYPE_X_Y, 0) == nullptr,
          "a move is in a game only from the game that brings it");
}

static void check_moves_end_at_last_move(void) {
    bool is_matching = true;
    for (spec_game_type_t type = 0; type < SPEC_GAME_TYPE_COUNT; ++type) {
        uint16_t last_move = spec_last_move(type);
        if (spec_get_move_data(type, last_move) == nullptr
            || spec_get_move_data(type, last_move + 1) != nullptr) {
            is_matching = false;
        }
    }
    check(is_matching, "each game type's moves end at its last move");
}

static void check_max_pp(void) {
    constexpr uint16_t POUND = 1;
    constexpr uint16_t GROWL = 45;
    constexpr uint16_t TRANSFORM = 144;
    check(spec_move_max_pp(SPEC_GAME_TYPE_RED_BLUE, GROWL, 3) == 61
              && spec_move_max_pp(SPEC_GAME_TYPE_EMERALD, GROWL, 3) == 64,
          "three PP Ups give 40 PP 61 in Gen 1 and 2, 64 after");
    check(spec_move_max_pp(SPEC_GAME_TYPE_CRYSTAL, TRANSFORM, 3) == 16
              && spec_move_max_pp(SPEC_GAME_TYPE_PLATINUM, TRANSFORM, 3) == 16
              && spec_move_max_pp(SPEC_GAME_TYPE_SUN_MOON, POUND, 0) == 35,
          "each PP Up adds a fifth of the base PP");
    check(spec_move_max_pp(SPEC_GAME_TYPE_SUN_MOON, POUND, 4) == 0
              && spec_move_max_pp(SPEC_GAME_TYPE_RED_BLUE, 166, 0) == 0,
          "more than three PP Ups, or a move the game lacks, gives 0");
}

static void check_identify_of_blank_data(void) {
    static uint8_t data[SPEC_GBC_JAPANESE_CRYSTAL_SAVE_SIZE];
    spec_gb_identity_t gb_identities[SPEC_GB_IDENTITY_MAX_COUNT];
    spec_gbc_identity_t gbc_identities[SPEC_GBC_IDENTITY_MAX_COUNT];
    bool is_nothing_identified = true;
    for (int fill = 0x00; fill <= 0xFF; fill += 0xFF) {
        memset(data, fill, sizeof data);
        if (spec_gb_identify_save(gb_identities, data) != 0
            || spec_gbc_identify_save(gbc_identities, data, SPEC_GBC_SAVE_SIZE) != 0
            || spec_gbc_identify_save(gbc_identities, data, sizeof data) != 0) {
            is_nothing_identified = false;
        }
    }
    check(is_nothing_identified, "erased data is no game's save");
}

// A Diamond and Pearl block footer: the size, the magic, the block's id, then its CRC.
static void stamp_diamond_pearl_block(uint8_t *block, size_t size, uint8_t block_id,
                                      uint32_t magic) {
    uint8_t *footer = &block[size - 0x14];
    spec_write_u32_le(&footer[0x8], (uint32_t)size);
    spec_write_u32_le(&footer[0xC], magic);
    footer[0x10] = block_id;
    spec_write_u16_le(&footer[0x12], spec_crc16(block, size - 0x14));
}

static void check_korean_diamond_pearl_magic(void) {
    constexpr size_t GENERAL_SIZE = 0xC100;
    constexpr size_t STORAGE_SIZE = 0x121E0;
    static uint8_t data[SPEC_NDS_SAVE_SIZE];
    static spec_nds_save_t save;
    memset(data, 0, sizeof data);
    stamp_diamond_pearl_block(data, GENERAL_SIZE, 0, 0x20070903);
    stamp_diamond_pearl_block(&data[GENERAL_SIZE], STORAGE_SIZE, 1, 0x20070903);
    check(spec_nds_read_save(&save, data) == SPEC_OK && save.type == SPEC_GAME_TYPE_DIAMOND_PEARL,
          "Korean Pearl's block magic reads as Diamond and Pearl");
}

int main(void) {
    check_box_geometry();
    check_game_limits();
    check_game_boy_species_data();
    check_set_species();
    check_gen3_to_gen5_species_data();
    check_3ds_species_data();
    check_migration_ids();
    check_move_data();
    check_moves_end_at_last_move();
    check_max_pp();
    check_identify_of_blank_data();
    check_korean_diamond_pearl_magic();
    printf("%zu failures\n", failure_count);
    if (failure_count != 0) {
        return 1;
    }
    return 0;
}
