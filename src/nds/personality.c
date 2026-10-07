// What a Gen 4 PID decides: nature, gender and shininess.

#include "nds/nds.h"
#include "nds/tables.h"
#include "spec_internal.h"

// Forms share their species' gender ratio, which every Gen 4 game agrees on.
static spec_gender_t gender_of(spec_pid_t pid, uint16_t species) {
    const spec_nds_species_data_t *species_data =
        spec_nds_get_species_data(SPEC_GAME_TYPE_HEARTGOLD_SOULSILVER, species, 0);
    if (species_data == nullptr) {
        return SPEC_GENDER_GENDERLESS;
    }
    return spec_gender_from_ratio(pid, species_data->gender_ratio);
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
