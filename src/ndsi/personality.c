// What a Gen 5 PID decides: gender and shininess, and the nature it gave in Gen 3 and 4.

#include "ndsi/ndsi.h"
#include "ndsi/tables.h"
#include "spec_internal.h"

// Forms share their species' gender ratio, which every Gen 5 game agrees on.
static spec_gender_t gender_of(spec_pid_t pid, uint16_t species) {
    const spec_ndsi_species_data_t *species_data =
        spec_ndsi_get_species_data(SPEC_GAME_TYPE_BLACK2_WHITE2, species, 0);
    if (species_data == nullptr) {
        return SPEC_GENDER_GENDERLESS;
    }
    return spec_gender_from_ratio(pid, species_data->gender_ratio);
}

spec_ndsi_personality_t spec_ndsi_decode_personality(spec_pid_t pid, uint16_t species,
                                                     const spec_ndsi_trainer_t *trainer) {
    spec_ndsi_personality_t personality = {
        .pid = pid,
        .nature = (spec_nature_t)(pid % SPEC_NATURE_COUNT),
        .gender = gender_of(pid, species),
        .is_shiny = spec_is_shiny(pid, trainer->id, trainer->secret_id),
    };
    return personality;
}
