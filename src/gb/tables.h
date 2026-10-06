// Gen 1 tables, which tools/generate_gb_tables.py generates from data/gb/.

#ifndef SPEC_GB_TABLES_H
#define SPEC_GB_TABLES_H

#include <stddef.h>
#include <stdint.h>

#include "gb/gb.h"
#include "spec.h"
#include "spec_tables.h"

constexpr size_t SPEC_GB_SPECIES_INDEX_COUNT = 191;
constexpr size_t SPEC_GB_CHARMAP_SIZE = 256;
constexpr size_t SPEC_GB_ITEM_COUNT = 256;

// Species

extern const uint16_t spec_gb_national_of_species[SPEC_GB_SPECIES_INDEX_COUNT];
extern const spec_gb_species_t spec_gb_species_of_national[SPEC_GB_POKEDEX_SIZE];

struct spec_gb_species_data {
    uint8_t base_stats[SPEC_GB_STAT_COUNT];
    spec_growth_rate_t growth_rate;
};
typedef struct spec_gb_species_data spec_gb_species_data_t;

extern const spec_gb_species_data_t spec_gb_species_data[SPEC_GB_SPECIES_INDEX_COUNT];

// Text

// A ligature's second code point, else 0. hiragana is the other reading of a tile both kana
// share; a read-only character is written by another byte.
struct spec_gb_character {
    uint16_t code_points[2];
    uint16_t hiragana;
    bool is_read_only;
};
typedef struct spec_gb_character spec_gb_character_t;

extern const spec_gb_character_t spec_gb_charmap_english[SPEC_GB_CHARMAP_SIZE];
extern const spec_gb_character_t spec_gb_charmap_french_german[SPEC_GB_CHARMAP_SIZE];
extern const spec_gb_character_t spec_gb_charmap_italian_spanish[SPEC_GB_CHARMAP_SIZE];
extern const spec_gb_character_t spec_gb_charmap_japanese[SPEC_GB_CHARMAP_SIZE];

// Items

extern const char *const spec_gb_item_names[SPEC_NAME_LANGUAGE_COUNT][SPEC_GB_ITEM_COUNT];

#endif
