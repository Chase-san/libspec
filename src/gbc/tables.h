// Gen 2 tables, which tools/generate_gbc_tables.py generates from data/gbc/.

#ifndef SPEC_GBC_TABLES_H
#define SPEC_GBC_TABLES_H

#include <stddef.h>
#include <stdint.h>

#include "gb/tables.h"
#include "gbc/gbc.h"
#include "spec.h"
#include "spec_tables.h"

constexpr uint8_t SPEC_GBC_NO_POCKET = 0xFF;
// TMs 1-50, then HMs as 51-57.
constexpr size_t SPEC_GBC_MACHINE_COUNT = 57;

// Species

// Gold, Silver and Crystal have the same species data.
extern const spec_gbc_species_data_t spec_gbc_species_data[SPEC_GBC_POKEDEX_SIZE];

// Text

extern const spec_gb_character_t spec_gbc_charmap_english[SPEC_GB_CHARMAP_SIZE];
extern const spec_gb_character_t spec_gbc_charmap_french_german[SPEC_GB_CHARMAP_SIZE];
extern const spec_gb_character_t spec_gbc_charmap_italian_spanish[SPEC_GB_CHARMAP_SIZE];
extern const spec_gb_character_t spec_gbc_charmap_japanese[SPEC_GB_CHARMAP_SIZE];

// Items

// machine is 0 for an item that is no TM or HM, and migration_id for one with no Gen 4 to 7 number.
struct spec_gbc_item_data {
    uint8_t gold_silver_pocket;
    uint8_t crystal_pocket;
    uint8_t machine;
    bool is_mail;
    uint16_t migration_id;
};
typedef struct spec_gbc_item_data spec_gbc_item_data_t;

extern const spec_gbc_item_data_t spec_gbc_items[SPEC_GBC_ITEM_COUNT];
extern const char *const spec_gbc_item_names[SPEC_NAME_LANGUAGE_COUNT][SPEC_GBC_ITEM_COUNT];

#endif
