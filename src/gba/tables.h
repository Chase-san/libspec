// Gen 3 tables, which tools/generate_gba_tables.py generates from data/gba/.

#ifndef SPEC_GBA_TABLES_H
#define SPEC_GBA_TABLES_H

#include <stddef.h>
#include <stdint.h>

#include "gba/gba.h"
#include "spec.h"
#include "spec_tables.h"

constexpr size_t SPEC_GBA_SPECIES_INDEX_COUNT = 412;
constexpr size_t SPEC_GBA_NATIONAL_COUNT = 387;
constexpr size_t SPEC_GBA_CHARMAP_SIZE = 0xF7;
constexpr size_t SPEC_GBA_ITEM_COUNT = 377;
constexpr uint8_t SPEC_GBA_NO_POCKET = 0xFF;

struct spec_gba_species_data {
    uint8_t base_stats[SPEC_STAT_COUNT];
    spec_growth_rate_t growth_rate;
    uint8_t gender_ratio;
};
typedef struct spec_gba_species_data spec_gba_species_data_t;

struct spec_gba_item_data {
    const char *english_name;
    const char *german_name;
    uint8_t ruby_sapphire_pocket;
    uint8_t emerald_pocket;
    uint8_t firered_leafgreen_pocket;
    bool is_important;
};
typedef struct spec_gba_item_data spec_gba_item_data_t;

extern const uint16_t spec_gba_national_of_species[SPEC_GBA_SPECIES_INDEX_COUNT];
extern const uint16_t spec_gba_species_of_national[SPEC_GBA_NATIONAL_COUNT];

extern const spec_gba_species_data_t spec_gba_species_data[SPEC_GBA_SPECIES_INDEX_COUNT];

extern const uint16_t spec_gba_charmap_japanese[SPEC_GBA_CHARMAP_SIZE];
extern const uint16_t spec_gba_charmap_international[SPEC_GBA_CHARMAP_SIZE];
extern const uint16_t spec_gba_charmap_french[SPEC_GBA_CHARMAP_SIZE];
extern const uint16_t spec_gba_charmap_german[SPEC_GBA_CHARMAP_SIZE];

extern const spec_gba_item_data_t spec_gba_items[SPEC_GBA_ITEM_COUNT];

#endif
