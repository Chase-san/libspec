// What a Gen 3 PID decides: nature, gender, shininess and Unown's letter.

#include "gba/gba.h"
#include "gba/tables.h"
#include "spec_internal.h"

constexpr unsigned UNOWN_FORM_COUNT = 28;

static spec_gender_t gender_of(spec_pid_t pid, spec_gba_species_t species) {
    if (spec_gba_species_to_national(species) == 0) {
        return SPEC_GENDER_GENDERLESS;
    }
    return spec_gender_from_ratio(pid, spec_gba_species_data[species].gender_ratio);
}

// As GET_UNOWN_LETTER.
static uint8_t unown_form_of(spec_pid_t pid) {
    uint32_t form_bits =
        ((pid >> 18) & 0xC0) | ((pid >> 12) & 0x30) | ((pid >> 6) & 0x0C) | (pid & 0x03);
    return (uint8_t)(form_bits % UNOWN_FORM_COUNT);
}

spec_gba_personality_t spec_gba_decode_personality(spec_pid_t pid, spec_gba_species_t species,
                                                   const spec_gba_trainer_t *trainer) {
    spec_gba_personality_t personality = {
        .pid = pid,
        .nature = (spec_nature_t)(pid % SPEC_NATURE_COUNT),
        .gender = gender_of(pid, species),
        .is_shiny = spec_is_shiny(pid, trainer->id, trainer->secret_id),
        .unown_form = unown_form_of(pid),
    };
    return personality;
}
