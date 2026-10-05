// Tables every console shares, which tools/generate_spec_tables.py generates from data/.

#ifndef SPEC_TABLES_H
#define SPEC_TABLES_H

#include <stddef.h>
#include <stdint.h>

#include "spec.h"

constexpr size_t SPEC_SPECIES_NAME_COUNT = 808;
constexpr size_t SPEC_NAME_LANGUAGE_COUNT = SPEC_LANGUAGE_KOREAN + 1;
constexpr size_t SPEC_LEVEL_COUNT = 101;

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

extern const char *const spec_species_names[SPEC_NAME_LANGUAGE_COUNT][SPEC_SPECIES_NAME_COUNT];

extern const uint32_t spec_experience[SPEC_GROWTH_RATE_COUNT][SPEC_LEVEL_COUNT];

#endif
