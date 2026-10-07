// Tables every console shares, which tools/generate_spec_tables.py generates from data/.

#ifndef SPEC_TABLES_H
#define SPEC_TABLES_H

#include <stddef.h>
#include <stdint.h>

#include "spec.h"

constexpr size_t SPEC_NAME_LANGUAGE_COUNT = SPEC_LANGUAGE_CHINESE_TRADITIONAL + 1;
constexpr size_t SPEC_SPECIES_NAME_COUNT = 808;
constexpr size_t SPEC_MOVE_NAME_COUNT = 729;
constexpr size_t SPEC_ABILITY_NAME_COUNT = 234;
constexpr size_t SPEC_ITEM_NAME_COUNT = 960;
constexpr size_t SPEC_LEVEL_COUNT = 101;

// Species names

// As the 3DS games show them, then as Gen 1 and 2, Gen 3 and 4, and Gen 5 store them.
extern const char *const spec_species_names[SPEC_NAME_LANGUAGE_COUNT][SPEC_SPECIES_NAME_COUNT];
extern const char
    *const spec_game_boy_species_names[SPEC_NAME_LANGUAGE_COUNT][SPEC_SPECIES_NAME_COUNT];
extern const char
    *const spec_upper_case_species_names[SPEC_NAME_LANGUAGE_COUNT][SPEC_SPECIES_NAME_COUNT];
extern const char *const spec_gen5_species_names[SPEC_NAME_LANGUAGE_COUNT][SPEC_SPECIES_NAME_COUNT];

// Form names

// A row names its form from from_game on, until a later row for the same form takes over.
struct spec_form_names {
    uint16_t national_number;
    uint8_t form;
    spec_game_type_t from_game;
    const char *names[SPEC_NAME_LANGUAGE_COUNT];
};
typedef struct spec_form_names spec_form_names_t;

extern const spec_form_names_t spec_form_names[];
extern const size_t spec_form_names_count;

// Other names, by the number each generation from Gen 4 shares

extern const char *const spec_move_names[SPEC_NAME_LANGUAGE_COUNT][SPEC_MOVE_NAME_COUNT];
extern const char *const spec_ability_names[SPEC_NAME_LANGUAGE_COUNT][SPEC_ABILITY_NAME_COUNT];
extern const char *const spec_item_names[SPEC_NAME_LANGUAGE_COUNT][SPEC_ITEM_NAME_COUNT];
extern const char *const spec_nature_names[SPEC_NAME_LANGUAGE_COUNT][SPEC_NATURE_COUNT];
extern const char *const spec_type_names[SPEC_NAME_LANGUAGE_COUNT][SPEC_TYPE_COUNT];

// Experience

enum spec_growth_rate : uint8_t {
    SPEC_GROWTH_RATE_MEDIUM_FAST,
    SPEC_GROWTH_RATE_ERRATIC,
    SPEC_GROWTH_RATE_FLUCTUATING,
    SPEC_GROWTH_RATE_MEDIUM_SLOW,
    SPEC_GROWTH_RATE_FAST,
    SPEC_GROWTH_RATE_SLOW,
    SPEC_GROWTH_RATE_COUNT,
};
typedef enum spec_growth_rate spec_growth_rate_t;

extern const uint32_t spec_experience[SPEC_GROWTH_RATE_COUNT][SPEC_LEVEL_COUNT];

#endif
