// Checks the name tables, and that setting and removing a nickname write what each generation's
// games write.

#include <stdio.h>
#include <string.h>

#include "3ds/3ds.h"
#include "gb/gb.h"
#include "gba/gba.h"
#include "gbc/gbc.h"
#include "nds/nds.h"
#include "ndsi/ndsi.h"

static size_t failure_count;

static void check(bool is_passing, const char *what) {
    if (!is_passing) {
        ++failure_count;
        printf("failed: %s\n", what);
    }
}

static bool is_name(const char *name, const char *expected) {
    return name != nullptr && strcmp(name, expected) == 0;
}

static void check_name_tables(void) {
    check(is_name(spec_move_name(1, SPEC_LANGUAGE_JAPANESE), "はたく")
              && is_name(spec_move_name(728, SPEC_LANGUAGE_CHINESE_TRADITIONAL), "熾魂熱舞烈音爆")
              && spec_move_name(0, SPEC_LANGUAGE_ENGLISH) == nullptr
              && spec_move_name(729, SPEC_LANGUAGE_ENGLISH) == nullptr,
          "moves are named from 1 to 728");
    check(is_name(spec_ability_name(1, SPEC_LANGUAGE_GERMAN), "Duftnote")
              && is_name(spec_nature_name(SPEC_NATURE_HARDY, SPEC_LANGUAGE_FRENCH), "Hardi")
              && is_name(spec_type_name(SPEC_TYPE_FAIRY, SPEC_LANGUAGE_KOREAN), "페어리")
              && is_name(spec_type_name(SPEC_TYPE_MYSTERY, SPEC_LANGUAGE_SPANISH), "¿¿??")
              && is_name(spec_species_name(1, SPEC_LANGUAGE_CHINESE_SIMPLIFIED), "妙蛙种子"),
          "abilities, natures, types and species are named in every language");
    check(is_name(spec_form_name(SPEC_GAME_TYPE_SUN_MOON, 25, 1, SPEC_LANGUAGE_ENGLISH),
                  "Original Cap")
              && is_name(spec_form_name(SPEC_GAME_TYPE_OMEGA_RUBY_ALPHA_SAPPHIRE, 25, 1,
                                        SPEC_LANGUAGE_ENGLISH),
                         "Pikachu Rock Star")
              && spec_form_name(SPEC_GAME_TYPE_X_Y, 25, 1, SPEC_LANGUAGE_ENGLISH) == nullptr
              && is_name(spec_form_name(SPEC_GAME_TYPE_PLATINUM, 479, 1, SPEC_LANGUAGE_ENGLISH),
                         "Heat Rotom")
              && is_name(spec_form_name(SPEC_GAME_TYPE_ULTRA_SUN_ULTRA_MOON, 800, 3,
                                        SPEC_LANGUAGE_ENGLISH),
                         "Ultra Necrozma")
              && spec_form_name(SPEC_GAME_TYPE_PLATINUM, 493, 9, SPEC_LANGUAGE_ENGLISH) == nullptr
              && is_name(spec_form_name(SPEC_GAME_TYPE_PLATINUM, 493, 17, SPEC_LANGUAGE_ENGLISH),
                         "Arceus"),
          "forms are named as each game numbers them");
    check(is_name(spec_gb_item_name(1, SPEC_LANGUAGE_GERMAN), "MEISTERBALL")
              && is_name(spec_gbc_item_name(5, SPEC_LANGUAGE_ENGLISH), "POKé BALL")
              && is_name(spec_gbc_item_name(5, SPEC_LANGUAGE_KOREAN), "몬스터볼")
              && is_name(spec_gba_item_name(259, SPEC_LANGUAGE_JAPANESE), "マッハじてんしゃ")
              && is_name(spec_nds_item_name(1, SPEC_LANGUAGE_JAPANESE), "マスターボール")
              && is_name(spec_ndsi_item_name(1, SPEC_LANGUAGE_ENGLISH), "Master Ball"),
          "each console names its own items");
}

static bool is_nds_name(const spec_nds_pokemon_t *pokemon, const char *expected) {
    char8_t name[SPEC_NDS_TEXT_BUFFER_SIZE];
    return spec_nds_pokemon_get_name(pokemon, name) == SPEC_OK
           && strcmp((const char *)name, expected) == 0;
}

static bool is_gb_text(const uint8_t *text, size_t text_size, spec_language_t language,
                       const char *expected) {
    char8_t utf8[SPEC_GB_TEXT_BUFFER_SIZE];
    return spec_gb_text_to_utf8(utf8, text, text_size, language) == SPEC_OK
           && strcmp((const char *)utf8, expected) == 0;
}

static void check_gen1(void) {
    constexpr uint8_t BULBASAUR[SPEC_GB_NAME_SIZE] = {
        0x81, 0x94, 0x8B, 0x81, 0x80, 0x92, 0x80, 0x94, 0x91, 0x50, 0x50,
    };
    spec_gb_pokemon_t bulbasaur = {.species = spec_gb_species_from_national(1)};
    check(spec_gb_pokemon_remove_nickname(&bulbasaur, SPEC_LANGUAGE_ENGLISH) == SPEC_OK
              && memcmp(bulbasaur.nickname, BULBASAUR, sizeof BULBASAUR) == 0,
          "a Gen 1 name is upper case, padded with terminators");
    spec_gb_pokemon_t mr_mime = {.species = spec_gb_species_from_national(122)};
    check(spec_gb_pokemon_remove_nickname(&mr_mime, SPEC_LANGUAGE_FRENCH) == SPEC_OK
              && is_gb_text(mr_mime.nickname, SPEC_GB_NAME_SIZE, SPEC_LANGUAGE_FRENCH, "M.MIME")
              && mr_mime.nickname[1] == 0xE8,
          "Gen 1 writes Mr. Mime without a space, with the period");
    check(spec_gb_pokemon_set_nickname(&mr_mime, u8"MR.X", SPEC_LANGUAGE_ENGLISH) == SPEC_OK
              && mr_mime.nickname[2] == 0xF2 && mr_mime.nickname[4] == 0x50,
          "the Gen 1 keyboard types the decimal point");
    check(spec_gb_pokemon_remove_nickname(&bulbasaur, SPEC_LANGUAGE_JAPANESE) == SPEC_OK
              && is_gb_text(bulbasaur.nickname, SPEC_GB_JAPANESE_NAME_SIZE, SPEC_LANGUAGE_JAPANESE,
                            "フシギダネ")
              && bulbasaur.nickname[5] == 0x50,
          "a Japanese Gen 1 name fills 6 bytes");
    // As the verified Japanese Green save holds a nickname typed in full.
    constexpr uint8_t PIPIPIPIPI[] = {0x41, 0x41, 0x41, 0x41, 0x41, 0x50};
    check(spec_gb_pokemon_set_nickname(&bulbasaur, u8"ピピピピピ", SPEC_LANGUAGE_JAPANESE)
                  == SPEC_OK
              && memcmp(bulbasaur.nickname, PIPIPIPIPI, sizeof PIPIPIPIPI) == 0
              && spec_gb_pokemon_set_nickname(&bulbasaur, u8"ピピピピピピ", SPEC_LANGUAGE_JAPANESE)
                     == SPEC_ERROR_NAME_TOO_LONG,
          "a Japanese Gen 1 name holds 5 characters, then the terminator");
    constexpr uint16_t POKE_BALL = 4;
    // As the verified Red and Green saves hold Pokémon named on being caught.
    constexpr uint8_t JET[SPEC_GB_NAME_SIZE] = {
        0x89, 0x84, 0x93, 0x50, 0x7F, 0x81, 0x80, 0x8B, 0x8B, 0x50, 0x00,
    };
    constexpr uint8_t A_IN_KATAKANA[SPEC_GB_JAPANESE_NAME_SIZE] = {0x80, 0x50, 0x8C,
                                                                   0x8F, 0xE3, 0x1C};
    spec_gb_pokemon_t caught = {};
    spec_gb_pokemon_t caught_in_japan = {};
    check(spec_gb_pokemon_set_nickname_ext(&caught, u8"JET", POKE_BALL, SPEC_LANGUAGE_ENGLISH)
                  == SPEC_OK
              && memcmp(caught.nickname, JET, sizeof JET) == 0
              && spec_gb_pokemon_set_nickname_ext(&caught_in_japan, u8"ア", POKE_BALL,
                                                  SPEC_LANGUAGE_JAPANESE)
                     == SPEC_OK
              && memcmp(caught_in_japan.nickname, A_IN_KATAKANA, sizeof A_IN_KATAKANA) == 0,
          "a Gen 1 name typed when caught leaves the ball's name after it");
    constexpr uint8_t BO_OVER_JET[SPEC_GB_NAME_SIZE] = {
        0x81, 0x8E, 0x50, 0x50, 0x7F, 0x81, 0x80, 0x8B, 0x8B, 0x50, 0x00,
    };
    check(spec_gb_pokemon_set_nickname(&caught, u8"BO", SPEC_LANGUAGE_ENGLISH) == SPEC_OK
              && memcmp(caught.nickname, BO_OVER_JET, sizeof BO_OVER_JET) == 0
              && spec_gb_pokemon_set_nickname_ext(&caught, u8"X", 0xFF, SPEC_LANGUAGE_ENGLISH)
                     == SPEC_ERROR_INVALID_ITEM
              && memcmp(caught.nickname, BO_OVER_JET, sizeof BO_OVER_JET) == 0,
          "a Gen 1 name is typed over the old one, and a refused name changes nothing");
    constexpr uint8_t HEBI[] = {0xCD, 0x3B, 0x50};
    constexpr uint8_t HEBI_IN_KATAKANA[] = {0xCD, 0x1A, 0x50};
    check(is_gb_text(HEBI, sizeof HEBI, SPEC_LANGUAGE_JAPANESE, "へび")
              && is_gb_text(HEBI_IN_KATAKANA, sizeof HEBI_IN_KATAKANA, SPEC_LANGUAGE_JAPANESE,
                            "ヘビ"),
          "a kana both scripts share reads as the name's other kana");
    uint8_t ligature[4];
    check(spec_gb_text_from_utf8(ligature, sizeof ligature, u8"I’d", SPEC_LANGUAGE_ENGLISH)
                  == SPEC_OK
              && ligature[0] == 0x88 && ligature[1] == 0xBB && ligature[2] == 0x50,
          "Gen 1 writes ’d as its ligature");
}

static void check_gen2(void) {
    constexpr uint8_t CHIKORITA[SPEC_GBC_NAME_SIZE] = {
        0x82, 0x87, 0x88, 0x8A, 0x8E, 0x91, 0x88, 0x93, 0x80, 0x50, 0x50,
    };
    spec_gbc_pokemon_t chikorita = {.species = 152};
    check(spec_gbc_pokemon_remove_nickname(&chikorita, SPEC_LANGUAGE_ENGLISH) == SPEC_OK
              && memcmp(chikorita.nickname, CHIKORITA, sizeof CHIKORITA) == 0,
          "a Gen 2 name is upper case, padded with terminators");
    constexpr uint8_t EGG_OVER_CHIKORITA[SPEC_GBC_NAME_SIZE] = {
        0x84, 0x86, 0x86, 0x50, 0x8E, 0x91, 0x88, 0x93, 0x80, 0x50, 0x50,
    };
    spec_gbc_pokemon_t egg = chikorita;
    egg.is_egg = true;
    check(spec_gbc_pokemon_remove_nickname(&egg, SPEC_LANGUAGE_ENGLISH) == SPEC_OK
              && memcmp(egg.nickname, EGG_OVER_CHIKORITA, sizeof EGG_OVER_CHIKORITA) == 0,
          "a Gen 2 egg's name is written over the old one");
    check(spec_gbc_pokemon_set_nickname(&egg, u8"LEAF", SPEC_LANGUAGE_ENGLISH)
              == SPEC_ERROR_VALUE_OUT_OF_RANGE,
          "a Gen 2 egg cannot be nicknamed");
    check(spec_gbc_pokemon_set_nickname(&chikorita, u8"Leaf’s", SPEC_LANGUAGE_ENGLISH) == SPEC_OK
              && chikorita.nickname[4] == 0xD4 && chikorita.nickname[5] == 0x50
              && chikorita.nickname[10] == 0x50,
          "a Gen 2 nickname writes ’s as its ligature, then terminators to the end");
    spec_gbc_traits_t traits = spec_gbc_decode_traits((uint8_t[]){0, 10, 10, 10, 10}, 201);
    check(traits.is_shiny && traits.unown_form == 8,
          "Gen 2 decides shininess and the Unown form from the DVs: a shiny Unown is I or V");
}

static void check_gen3(void) {
    spec_gba_pokemon_t egg = {.species = 1, .language = SPEC_LANGUAGE_ENGLISH, .is_egg = true};
    char8_t name[SPEC_GBA_TEXT_BUFFER_SIZE];
    check(spec_gba_pokemon_remove_nickname(&egg) == SPEC_OK
              && spec_gba_text_to_utf8(name, egg.nickname, SPEC_GBA_NICKNAME_SIZE,
                                       SPEC_LANGUAGE_JAPANESE)
                     == SPEC_OK
              && strcmp((const char *)name, "タマゴ") == 0,
          "a Gen 3 egg is named in Japanese");
    constexpr uint8_t BOB_OVER_BULBASAUR[SPEC_GBA_NICKNAME_SIZE] = {
        0xBC, 0xC9, 0xBC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    };
    spec_gba_pokemon_t bulbasaur = {.species = 1, .language = SPEC_LANGUAGE_ENGLISH};
    check(spec_gba_pokemon_remove_nickname(&bulbasaur) == SPEC_OK
              && spec_gba_pokemon_set_nickname(&bulbasaur, u8"BOB") == SPEC_OK
              && memcmp(bulbasaur.nickname, BOB_OVER_BULBASAUR, sizeof BOB_OVER_BULBASAUR) == 0,
          "a Gen 3 nickname is followed by 0xFF to the end");
    check(spec_gba_pokemon_set_nickname(&egg, u8"BOB") == SPEC_ERROR_VALUE_OUT_OF_RANGE,
          "a Gen 3 egg cannot be nicknamed");
}

static void check_gen4(void) {
    spec_nds_pokemon_t turtwig = {
        .species = 387, .language = SPEC_LANGUAGE_ENGLISH, .is_nicknamed = true};
    check(spec_nds_pokemon_remove_nickname(&turtwig) == SPEC_OK && is_nds_name(&turtwig, "TURTWIG")
              && !turtwig.is_nicknamed && turtwig.nickname[7] == 0xFFFF && turtwig.nickname[8] == 0
              && turtwig.nickname[10] == 0,
          "a Gen 4 name is upper case, then the terminator and zeros");
    spec_nds_pokemon_t charmander = {.species = 4, .language = SPEC_LANGUAGE_FRENCH};
    check(spec_nds_pokemon_remove_nickname(&charmander) == SPEC_OK
              && is_nds_name(&charmander, "SALAMECHE"),
          "a Gen 4 French name drops its accents");
    spec_nds_pokemon_t bad_egg = {.species = 4, .is_egg = true, .is_bad_egg = true};
    check(spec_nds_pokemon_remove_nickname(&bad_egg) == SPEC_ERROR_UNKNOWN_NAME,
          "a Bad Egg has no stored name");

    spec_nds_pokemon_t caught = turtwig;
    check(spec_nds_pokemon_set_nickname(&caught, u8"Ted", SPEC_NAMING_CAUGHT_OR_HATCHED) == SPEC_OK
              && is_nds_name(&caught, "Ted") && caught.is_nicknamed
              && memcmp(&caught.nickname[4], &turtwig.nickname[4], 7 * sizeof(uint16_t)) == 0,
          "a Gen 4 nickname given when caught keeps the old name's tail");
    spec_nds_pokemon_t renamed = turtwig;
    renamed.nickname[9] = 0x1234;
    check(spec_nds_pokemon_set_nickname(&renamed, u8"Ted", SPEC_NAMING_NAME_RATER) == SPEC_OK
              && is_nds_name(&renamed, "Ted") && renamed.nickname[3] == 0xFFFF
              && renamed.nickname[4] == 0 && renamed.nickname[9] == 0 && renamed.nickname[10] == 0,
          "a Gen 4 nickname from the Name Rater is followed by zeros");
    check(spec_nds_pokemon_set_nickname(&caught, u8"TURTWIG", SPEC_NAMING_NAME_RATER) == SPEC_OK
              && !caught.is_nicknamed,
          "a Gen 4 nickname matching the species name clears the flag");
    check(spec_nds_pokemon_set_nickname(&bad_egg, u8"Ted", SPEC_NAMING_NAME_RATER)
              == SPEC_ERROR_VALUE_OUT_OF_RANGE,
          "a Gen 4 egg cannot be nicknamed");
}

static void check_gen5(void) {
    constexpr uint16_t TEPIG[SPEC_NDSI_NICKNAME_SIZE] = {
        'T', 'e', 'p', 'i', 'g', 0xFFFF, 0, 0, 0, 0, 0xFFFF,
    };
    spec_ndsi_pokemon_t tepig = {.species = 498, .language = SPEC_LANGUAGE_ENGLISH};
    check(spec_ndsi_pokemon_remove_nickname(&tepig) == SPEC_OK
              && memcmp(tepig.nickname, TEPIG, sizeof TEPIG) == 0,
          "a Gen 5 name is mixed case, padded as a new Pokémon's");
    spec_ndsi_pokemon_t farfetchd = {.species = 83, .language = SPEC_LANGUAGE_ENGLISH};
    check(spec_ndsi_pokemon_remove_nickname(&farfetchd) == SPEC_OK && farfetchd.nickname[8] == '\'',
          "Gen 5 writes Farfetch'd with a plain apostrophe");
    constexpr uint16_t EGG_OVER_HAPPINY[SPEC_NDSI_NICKNAME_SIZE] = {
        'E', 'g', 'g', 0xFFFF, 'i', 'n', 'y', 0xFFFF, 0, 0, 0xFFFF,
    };
    spec_ndsi_pokemon_t egg = {
        .species = 440,
        .language = SPEC_LANGUAGE_ENGLISH,
        .is_egg = true,
        .nickname = {'H', 'a', 'p', 'p', 'i', 'n', 'y', 0xFFFF, 0, 0, 0xFFFF},
    };
    check(spec_ndsi_pokemon_remove_nickname(&egg) == SPEC_OK
              && memcmp(egg.nickname, EGG_OVER_HAPPINY, sizeof EGG_OVER_HAPPINY) == 0,
          "a Gen 5 egg's name is written over the old one");

    constexpr uint16_t PIG_OVER_TEPIG[SPEC_NDSI_NICKNAME_SIZE] = {
        'P', 'i', 'g', 0xFFFF, 'g', 0xFFFF, 0, 0, 0, 0, 0xFFFF,
    };
    spec_ndsi_pokemon_t caught = tepig;
    check(spec_ndsi_pokemon_set_nickname(&caught, u8"Pig", SPEC_NAMING_CAUGHT_OR_HATCHED) == SPEC_OK
              && memcmp(caught.nickname, PIG_OVER_TEPIG, sizeof PIG_OVER_TEPIG) == 0
              && caught.is_nicknamed,
          "a Gen 5 nickname given when caught keeps the old name's tail");
    constexpr uint16_t RENAMED_PIG[SPEC_NDSI_NICKNAME_SIZE] = {
        'P', 'i', 'g', 0xFFFF, 0, 0, 0, 0, 0, 0, 0,
    };
    spec_ndsi_pokemon_t renamed = tepig;
    check(spec_ndsi_pokemon_set_nickname(&renamed, u8"Pig", SPEC_NAMING_NAME_RATER) == SPEC_OK
              && memcmp(renamed.nickname, RENAMED_PIG, sizeof RENAMED_PIG) == 0,
          "a Gen 5 nickname from the Name Rater is followed by zeros, the last unit included");
    check(spec_ndsi_pokemon_set_nickname(&caught, u8"Tepig", SPEC_NAMING_CAUGHT_OR_HATCHED)
                  == SPEC_OK
              && !caught.is_nicknamed,
          "a Gen 5 nickname matching the species name clears the flag");
    check(spec_ndsi_pokemon_set_nickname(&egg, u8"Pig", SPEC_NAMING_CAUGHT_OR_HATCHED)
              == SPEC_ERROR_VALUE_OUT_OF_RANGE,
          "a Gen 5 egg cannot be nicknamed");
}

static void check_gen6_and_gen7(void) {
    constexpr uint16_t NIDORAN[SPEC_3DS_NAME_SIZE] = {'N', 'i', 'd', 'o', 'r', 'a', 'n', 0xE08E};
    spec_3ds_pokemon_t nidoran = {
        .generation = 6,
        .species = 32,
        .language = SPEC_LANGUAGE_ENGLISH,
        .is_nicknamed = true,
    };
    check(spec_3ds_pokemon_remove_nickname(&nidoran) == SPEC_OK
              && memcmp(nidoran.nickname, NIDORAN, sizeof NIDORAN) == 0 && !nidoran.is_nicknamed,
          "a Gen 6 name writes the half-width ♂, zeros after it");
    nidoran.language = SPEC_LANGUAGE_JAPANESE;
    check(spec_3ds_pokemon_remove_nickname(&nidoran) == SPEC_OK && nidoran.nickname[4] == 0x2642,
          "a Japanese Gen 6 name writes the full-width ♂");
    constexpr uint16_t PIKACHU_IN_GLYPHS[SPEC_3DS_NAME_SIZE] = {0xE82D, 0xE80F, 0xE82E};
    spec_3ds_pokemon_t pikachu = {
        .generation = 7,
        .species = 25,
        .language = SPEC_LANGUAGE_CHINESE_SIMPLIFIED,
    };
    char8_t name[SPEC_3DS_TEXT_BUFFER_SIZE];
    check(spec_3ds_pokemon_remove_nickname(&pikachu) == SPEC_OK
              && memcmp(pikachu.nickname, PIKACHU_IN_GLYPHS, sizeof PIKACHU_IN_GLYPHS) == 0
              && spec_3ds_pokemon_get_name(&pikachu, name) == SPEC_OK
              && strcmp((const char *)name, "皮卡丘") == 0,
          "Gen 7 writes Chinese species names in its own glyphs, which read back as characters");
    spec_3ds_pokemon_t egg = {
        .generation = 6,
        .species = 25,
        .language = SPEC_LANGUAGE_FRENCH,
        .is_egg = true,
    };
    check(spec_3ds_pokemon_remove_nickname(&egg) == SPEC_OK
              && spec_3ds_pokemon_get_name(&egg, name) == SPEC_OK
              && strcmp((const char *)name, "Œuf") == 0,
          "a Gen 6 egg is named in its language");
    spec_3ds_pokemon_t renamed = pikachu;
    check(spec_3ds_pokemon_set_nickname(&renamed, u8"Sparky", SPEC_NAMING_CAUGHT_OR_HATCHED)
                  == SPEC_OK
              && renamed.is_nicknamed,
          "a name other than the species' marks a nickname");
}

int main(void) {
    check_name_tables();
    check_gen1();
    check_gen2();
    check_gen3();
    check_gen4();
    check_gen5();
    check_gen6_and_gen7();
    printf("%zu failures\n", failure_count);
    return failure_count == 0 ? 0 : 1;
}
