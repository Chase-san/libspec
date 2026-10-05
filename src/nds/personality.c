// What a Gen 4 PID decides: nature, gender and shininess.

#include "nds/nds.h"
#include "nds/tables.h"
#include "spec_internal.h"

// Forms share their species' gender ratio.
static spec_gender_t gender_of(spec_pid_t pid, uint16_t species) {
    if (species == 0 || species >= SPEC_NDS_SPECIES_COUNT) {
        return SPEC_GENDER_GENDERLESS;
    }
    return spec_gender_from_ratio(pid, spec_nds_species_data[species].gender_ratio);
}

spec_nds_personality_t spec_nds_decode_personality(spec_pid_t pid, uint16_t species,
                                                   const spec_nds_trainer_t *trainer) {
    spec_nds_personality_t personality = {
        .pid = pid,
        .nature = (spec_nature_t)(pid % SPEC_NATURE_COUNT),
        .gender = gender_of(pid, species),
        .is_shiny = spec_is_shiny(pid, trainer->id, trainer->secret_id),
    };
    return personality;
}
