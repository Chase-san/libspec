#include "spec.h"
#include "spec_tables.h"

const char *spec_species_name(uint16_t national_number, spec_language_t language) {
    if (national_number >= SPEC_SPECIES_NAME_COUNT || language >= SPEC_NAME_LANGUAGE_COUNT) {
        return nullptr;
    }
    return spec_species_names[language][national_number];
}
