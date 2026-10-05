// NDS tables, which tools/generate_nds_tables.py generates from data/nds/.

#ifndef SPEC_NDS_TABLES_H
#define SPEC_NDS_TABLES_H

#include <stddef.h>
#include <stdint.h>

#include "nds/nds.h"
#include "spec.h"
#include "spec_tables.h"

constexpr size_t SPEC_NDS_SPECIES_COUNT = 494;
constexpr size_t SPEC_NDS_FORM_DATA_COUNT = 12;
constexpr size_t SPEC_NDS_CHARMAP_SIZE = 0xD66;
constexpr size_t SPEC_NDS_ITEM_COUNT = 537;
constexpr uint8_t SPEC_NDS_NO_POCKET = 0xFF;

// Species

struct spec_nds_species_data {
    uint8_t base_stats[SPEC_STAT_COUNT];
    spec_growth_rate_t growth_rate;
    uint8_t gender_ratio;
};
typedef struct spec_nds_species_data spec_nds_species_data_t;

// The forms whose base stats differ from their species'.
struct spec_nds_form_data {
    uint16_t species;
    uint8_t form;
    uint8_t base_stats[SPEC_STAT_COUNT];
};
typedef struct spec_nds_form_data spec_nds_form_data_t;

extern const spec_nds_species_data_t spec_nds_species_data[SPEC_NDS_SPECIES_COUNT];
extern const spec_nds_form_data_t spec_nds_form_data[SPEC_NDS_FORM_DATA_COUNT];

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
