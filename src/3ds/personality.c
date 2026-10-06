// What a Gen 6 or 7 PID decides: only shininess, by a wider margin than Gen 3 to 5's.

#include "3ds/3ds.h"

constexpr uint32_t SHINY_ODDS = 16;

spec_3ds_personality_t spec_3ds_decode_personality(spec_pid_t pid,
                                                   const spec_3ds_trainer_t *trainer) {
    uint32_t mixed = (uint32_t)(trainer->id ^ trainer->secret_id) ^ (pid >> 16) ^ (pid & 0xFFFF);
    spec_3ds_personality_t personality = {
        .pid = pid,
        .is_shiny = mixed < SHINY_ODDS,
    };
    return personality;
}
