// Levels from experience, and the stat formula of Gen 3 and later.

#include "spec.h"
#include "spec_internal.h"
#include "spec_tables.h"

constexpr uint8_t MAX_LEVEL = 100;

// Nature n raises stat n / 5 and lowers n % 5, from Attack; 16-bit math as in the games.
static uint16_t apply_nature(uint16_t value, spec_stat_t stat, spec_nature_t nature) {
    unsigned raised_stat = SPEC_STAT_ATTACK + nature / 5;
    unsigned lowered_stat = SPEC_STAT_ATTACK + nature % 5;
    if (raised_stat == lowered_stat) {
        return value;
    }
    if (stat == raised_stat) {
        return (uint16_t)((uint16_t)(value * 110) / 100);
    }
    if (stat == lowered_stat) {
        return (uint16_t)((uint16_t)(value * 90) / 100);
    }
    return value;
}

uint16_t spec_calculate_stat(spec_stat_t stat, uint8_t base_stat, uint8_t iv, uint8_t ev,
                             uint8_t level, spec_nature_t nature) {
    unsigned level_share = (2U * base_stat + iv + ev / 4U) * level / 100;
    if (stat == SPEC_STAT_HP) {
        return (uint16_t)(level_share + level + 10);
    }
    return apply_nature((uint16_t)(level_share + 5), stat, nature);
}

uint32_t spec_experience_for_level(spec_growth_rate_t growth_rate, uint8_t level) {
    return level == 1 ? 0 : spec_experience[growth_rate][level];
}

// As Gen 3's GetLevelFromMonExp.
uint8_t spec_gen3_level_for_experience(spec_growth_rate_t growth_rate, uint32_t experience) {
    uint8_t level = 1;
    while (level <= MAX_LEVEL && spec_experience[growth_rate][level] <= experience) {
        ++level;
    }
    return (uint8_t)(level - 1);
}

uint8_t spec_level_for_experience(spec_growth_rate_t growth_rate, uint32_t experience) {
    uint8_t level = spec_gen3_level_for_experience(growth_rate, experience);
    return level == 0 ? 1 : level;
}
