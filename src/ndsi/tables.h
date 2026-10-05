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
constexpr size_t SPEC_NDSI_FORM_DATA_COUNT = 24;
constexpr size_t SPEC_NDSI_ITEM_COUNT = 639;
constexpr uint8_t SPEC_NDSI_NO_POCKET = 0xFF;

// Species

typedef spec_nds_species_data_t spec_ndsi_species_data_t;
typedef spec_nds_form_data_t spec_ndsi_form_data_t;

extern const spec_ndsi_species_data_t spec_ndsi_species_data[SPEC_NDSI_SPECIES_COUNT];
extern const spec_ndsi_form_data_t spec_ndsi_form_data[SPEC_NDSI_FORM_DATA_COUNT];

// Items

struct spec_ndsi_item_data {
    const char *english_name;
    uint8_t black_white_pocket;
    uint8_t black2_white2_pocket;
};
typedef struct spec_ndsi_item_data spec_ndsi_item_data_t;

extern const spec_ndsi_item_data_t spec_ndsi_items[SPEC_NDSI_ITEM_COUNT];

#endif
