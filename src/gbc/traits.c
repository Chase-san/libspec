// Gen 2 traits: the gender, shininess and Unown form the DVs decide.

#include "gbc/gbc.h"
#include "gbc/tables.h"
#include "spec_internal.h"

constexpr unsigned DV_BIT_COUNT = 4;
constexpr uint8_t UNOWN = 201;
constexpr uint8_t GENDER_ALWAYS_MALE = 0;
constexpr uint8_t GENDER_ALWAYS_FEMALE = 254;
constexpr uint8_t GENDERLESS = 255;
// CheckShininess: Defense, Speed and Special all 10, and Attack with bit 1 set.
constexpr uint8_t SHINY_DV = 10;
constexpr unsigned SHINY_ATTACK_DV_BIT = 1;
// GetUnownLetter divides its byte by 255 / 26 + 1.
constexpr unsigned UNOWN_LETTER_DIVISOR = 10;

// As GetGender: female when Attack and Speed's DVs, as a byte, are at most the ratio.
static spec_gender_t gender_of(const uint8_t dvs[static SPEC_GB_STAT_COUNT], uint8_t species) {
    uint8_t gender_ratio = spec_gbc_species_data[species].gender_ratio;
    if (gender_ratio == GENDERLESS) {
        return SPEC_GENDER_GENDERLESS;
    }
    if (gender_ratio == GENDER_ALWAYS_MALE) {
        return SPEC_GENDER_MALE;
    }
    if (gender_ratio == GENDER_ALWAYS_FEMALE) {
        return SPEC_GENDER_FEMALE;
    }
    unsigned gender_byte =
        (unsigned)dvs[SPEC_GB_STAT_ATTACK] << DV_BIT_COUNT | dvs[SPEC_GB_STAT_SPEED];
    return gender_byte <= gender_ratio ? SPEC_GENDER_FEMALE : SPEC_GENDER_MALE;
}

static bool is_shiny(const uint8_t dvs[static SPEC_GB_STAT_COUNT]) {
    return dvs[SPEC_GB_STAT_DEFENSE] == SHINY_DV && dvs[SPEC_GB_STAT_SPEED] == SHINY_DV
           && dvs[SPEC_GB_STAT_SPECIAL] == SHINY_DV
           && spec_get_bits(dvs[SPEC_GB_STAT_ATTACK], SHINY_ATTACK_DV_BIT, 1) != 0;
}

// As GetUnownLetter: bits 1-2 of each DV but HP, Attack's highest, divided by 10.
static uint8_t unown_form_of(const uint8_t dvs[static SPEC_GB_STAT_COUNT]) {
    unsigned letter_byte = 0;
    for (size_t stat = SPEC_GB_STAT_ATTACK; stat < SPEC_GB_STAT_COUNT; ++stat) {
        letter_byte = letter_byte << 2 | spec_get_bits(dvs[stat], 1, 2);
    }
    return (uint8_t)(letter_byte / UNOWN_LETTER_DIVISOR);
}

spec_gbc_traits_t spec_gbc_decode_traits(const uint8_t dvs[static SPEC_GB_STAT_COUNT],
                                         uint8_t species) {
    spec_gbc_traits_t traits = {.is_shiny = is_shiny(dvs)};
    if (species != 0 && species < SPEC_GBC_POKEDEX_SIZE) {
        traits.gender = gender_of(dvs, species);
    }
    if (species == UNOWN) {
        traits.unown_form = unown_form_of(dvs);
    }
    return traits;
}
