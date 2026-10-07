// NDSi tables, which tools/generate_ndsi_tables.py generates from data/ndsi/.

#ifndef SPEC_NDSI_TABLES_H
#define SPEC_NDSI_TABLES_H

#include <stddef.h>
#include <stdint.h>

#include "nds/tables.h"
#include "ndsi/ndsi.h"
#include "spec.h"
#include "spec_tables.h"

constexpr size_t SPEC_NDSI_SPECIES_COUNT = 650;
constexpr uint8_t SPEC_NDSI_NO_POCKET = 0xFF;

// Species

// A row describes its species or form from from_game on, until a later row for the same one takes
// over.
struct spec_ndsi_species_row {
    uint16_t species;
    uint8_t form;
    spec_game_type_t from_game;
    spec_ndsi_species_data_t data;
};
typedef struct spec_ndsi_species_row spec_ndsi_species_row_t;

extern const spec_ndsi_species_row_t spec_ndsi_species_rows[];
extern const size_t spec_ndsi_species_row_count;

// Items

struct spec_ndsi_item_data {
    const char *english_name;
    uint8_t black_white_pocket;
    uint8_t black2_white2_pocket;
};
typedef struct spec_ndsi_item_data spec_ndsi_item_data_t;

extern const spec_ndsi_item_data_t spec_ndsi_items[SPEC_NDSI_ITEM_COUNT];

#endif
