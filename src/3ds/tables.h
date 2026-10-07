// Gen 6 and 7 tables, which tools/generate_3ds_tables.py generates from data/3ds/.

#ifndef SPEC_3DS_TABLES_H
#define SPEC_3DS_TABLES_H

#include <stddef.h>
#include <stdint.h>

#include "3ds/3ds.h"
#include "spec.h"
#include "spec_tables.h"

constexpr size_t SPEC_3DS_SPECIES_COUNT = 808;
constexpr uint8_t SPEC_3DS_NO_POCKET = 0xFF;
// The game types in spec_game_type_t's order: X and Y to Ultra Sun and Ultra Moon.
constexpr size_t SPEC_3DS_GAME_TYPE_COUNT = 4;
// Gen 7's glyphs for Chinese species names.
constexpr char16_t SPEC_3DS_FIRST_CHINESE_GLYPH = 0xE800;
constexpr size_t SPEC_3DS_CHINESE_GLYPH_COUNT = 0xEE27 - 0xE800;
// The longest Chinese species name has 5 units, then 0x0000.
constexpr size_t SPEC_3DS_CHINESE_NAME_SIZE = 6;

// Species

// A row describes its species or form from from_game on, until a later row for the same one takes
// over.
struct spec_3ds_species_row {
    uint16_t species;
    uint8_t form;
    spec_game_type_t from_game;
    spec_3ds_species_data_t data;
};
typedef struct spec_3ds_species_row spec_3ds_species_row_t;

extern const spec_3ds_species_row_t spec_3ds_species_rows[];
extern const size_t spec_3ds_species_row_count;

// Text

extern const char16_t spec_3ds_chinese_glyphs[SPEC_3DS_CHINESE_GLYPH_COUNT];
// Simplified, then Traditional; species 0 is the Egg.
extern const uint16_t spec_3ds_chinese_species_names[2][SPEC_3DS_SPECIES_COUNT]
                                                    [SPEC_3DS_CHINESE_NAME_SIZE];

// Items

// The pocket in each game type, SPEC_3DS_NO_POCKET for none.
struct spec_3ds_item_data {
    uint8_t pockets[SPEC_3DS_GAME_TYPE_COUNT];
};
typedef struct spec_3ds_item_data spec_3ds_item_data_t;

extern const spec_3ds_item_data_t spec_3ds_items[SPEC_3DS_ITEM_COUNT];

#endif
