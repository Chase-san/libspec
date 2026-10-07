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
constexpr uint8_t SPEC_GBA_NO_POCKET = 0xFF;

// Species

extern const uint16_t spec_gba_national_of_species[SPEC_GBA_SPECIES_INDEX_COUNT];
extern const spec_gba_species_t spec_gba_species_of_national[SPEC_GBA_NATIONAL_COUNT];

// Every Gen 3 game has the same species data.
extern const spec_gba_species_data_t spec_gba_species_data[SPEC_GBA_SPECIES_INDEX_COUNT];

// Text

extern const uint16_t spec_gba_charmap_japanese[SPEC_GBA_CHARMAP_SIZE];
extern const uint16_t spec_gba_charmap_international[SPEC_GBA_CHARMAP_SIZE];
extern const uint16_t spec_gba_charmap_french[SPEC_GBA_CHARMAP_SIZE];
extern const uint16_t spec_gba_charmap_german[SPEC_GBA_CHARMAP_SIZE];

// Items

struct spec_gba_item_data {
    const char *english_name;
    const char *german_name;
    uint8_t ruby_sapphire_pocket;
    uint8_t emerald_pocket;
    uint8_t firered_leafgreen_pocket;
    bool is_important;
    // The item's number from Gen 4 on, which names it in the other languages; 0 for none.
    uint16_t migration_id;
};
typedef struct spec_gba_item_data spec_gba_item_data_t;

extern const spec_gba_item_data_t spec_gba_items[SPEC_GBA_ITEM_COUNT];

#endif
