// NDS tables, which tools/generate_nds_tables.py generates from data/nds/.

#ifndef SPEC_NDS_TABLES_H
#define SPEC_NDS_TABLES_H

#include <stddef.h>
#include <stdint.h>

#include "nds/nds.h"
#include "spec.h"
#include "spec_tables.h"

constexpr size_t SPEC_NDS_SPECIES_COUNT = 494;
constexpr size_t SPEC_NDS_CHARMAP_SIZE = 0xD66;
constexpr uint8_t SPEC_NDS_NO_POCKET = 0xFF;

// Species

// A row describes its species or form from from_game on, until a later row for the same one takes
// over.
struct spec_nds_species_row {
    uint16_t species;
    uint8_t form;
    spec_game_type_t from_game;
    spec_nds_species_data_t data;
};
typedef struct spec_nds_species_row spec_nds_species_row_t;

extern const spec_nds_species_row_t spec_nds_species_rows[];
extern const size_t spec_nds_species_row_count;

// Text

extern const uint16_t spec_nds_charmap[SPEC_NDS_CHARMAP_SIZE];

// Items

struct spec_nds_item_data {
    const char *english_name;
    uint8_t diamond_pearl_pocket;
    uint8_t platinum_pocket;
    uint8_t heartgold_soulsilver_pocket;
};
typedef struct spec_nds_item_data spec_nds_item_data_t;

extern const spec_nds_item_data_t spec_nds_items[SPEC_NDS_ITEM_COUNT];

#endif
