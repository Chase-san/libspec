// Checks that setting and removing a nickname write what each generation's games write.

#include <stdio.h>
#include <string.h>

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

int main(void) {
    check_gen1();
    check_gen2();
    check_gen3();
    check_gen4();
    check_gen5();
    printf("%zu failures\n", failure_count);
    return failure_count == 0 ? 0 : 1;
}
