#include "spec.h"
#include "spec_tables.h"

const char *spec_species_name(uint16_t national_number) {
    if (national_number >= SPEC_SPECIES_NAME_COUNT) {
        return nullptr;
    }
    return spec_species_names[national_number];
}
