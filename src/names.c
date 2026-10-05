// Species names by National Dex number and language.

#include "spec.h"
#include "spec_internal.h"
#include "spec_tables.h"

static bool has_name(uint16_t national_number, spec_language_t language) {
    return national_number < SPEC_SPECIES_NAME_COUNT && language < SPEC_NAME_LANGUAGE_COUNT;
}

const char *spec_species_name(uint16_t national_number, spec_language_t language) {
    if (!has_name(national_number, language)) {
        return nullptr;
    }
    return spec_species_names[language][national_number];
}

const char8_t *spec_upper_case_species_name(uint16_t national_number, spec_language_t language) {
    if (!has_name(national_number, language)) {
        return nullptr;
    }
    return (const char8_t *)spec_upper_case_species_names[language][national_number];
}

const char8_t *spec_gen5_species_name(uint16_t national_number, spec_language_t language) {
    if (!has_name(national_number, language)) {
        return nullptr;
    }
    return (const char8_t *)spec_gen5_species_names[language][national_number];
}
