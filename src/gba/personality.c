#include <string.h>

#include "gba/gba.h"
#include "gba/tables.h"
#include "spec_internal.h"

constexpr uint32_t RNG_MULTIPLIER = 0x41C64E6D;
constexpr uint32_t RNG_INCREMENT = 0x6073;
constexpr uint32_t SHINY_ODDS = 8;
constexpr uint8_t GENDER_RATIO_ALWAYS_MALE = 0;
constexpr uint8_t GENDER_RATIO_ALWAYS_FEMALE = 254;
constexpr uint8_t GENDER_RATIO_GENDERLESS = 255;
constexpr unsigned UNOWN_FORM_COUNT = 28;
constexpr unsigned IV_BIT_COUNT = 5;
constexpr uint32_t STATE_LOW_BITS_COUNT = 1 << 16;

static uint32_t next_state(uint32_t state) {
    return state * RNG_MULTIPLIER + RNG_INCREMENT;
}

// As GetGenderFromSpeciesAndPersonality.
static spec_gender_t gender_of(spec_gba_pid_t pid, uint16_t species) {
    if (spec_gba_species_to_national(species) == 0) {
        return SPEC_GENDER_GENDERLESS;
    }
    uint8_t gender_ratio = spec_gba_species_data[species].gender_ratio;
    switch (gender_ratio) {
        case GENDER_RATIO_ALWAYS_MALE:
            return SPEC_GENDER_MALE;
        case GENDER_RATIO_ALWAYS_FEMALE:
            return SPEC_GENDER_FEMALE;
        case GENDER_RATIO_GENDERLESS:
            return SPEC_GENDER_GENDERLESS;
    }
    return gender_ratio > (pid & 0xFF) ? SPEC_GENDER_FEMALE : SPEC_GENDER_MALE;
}

// As IsShinyOtIdPersonality.
static bool is_shiny_for(spec_gba_pid_t pid, const spec_gba_trainer_t *trainer) {
    uint32_t shiny_value = trainer->id ^ trainer->secret_id ^ (pid >> 16) ^ (pid & 0xFFFF);
    return shiny_value < SHINY_ODDS;
}

// As GET_UNOWN_LETTER.
static uint8_t unown_form_of(spec_gba_pid_t pid) {
    uint32_t form_bits =
        ((pid >> 18) & 0xC0) | ((pid >> 12) & 0x30) | ((pid >> 6) & 0x0C) | (pid & 0x03);
    return (uint8_t)(form_bits % UNOWN_FORM_COUNT);
}

static void ivs_from_random(uint8_t ivs[static SPEC_STAT_COUNT], uint16_t first_random,
                            uint16_t second_random) {
    for (unsigned stat = 0; stat < 3; ++stat) {
        ivs[stat] = (uint8_t)spec_get_bits(first_random, stat * IV_BIT_COUNT, IV_BIT_COUNT);
        ivs[stat + 3] = (uint8_t)spec_get_bits(second_random, stat * IV_BIT_COUNT, IV_BIT_COUNT);
    }
}

// States after drawing the PID as CreateBoxMon does, low half first.
static size_t find_pid_states(spec_gba_pid_t pid,
                              uint32_t states[static SPEC_GBA_EXPECTED_IV_SETS_MAX]) {
    size_t state_count = 0;
    for (uint32_t low_bits = 0;
         low_bits < STATE_LOW_BITS_COUNT && state_count < SPEC_GBA_EXPECTED_IV_SETS_MAX;
         ++low_bits) {
        uint32_t pid_high_state = next_state((pid << 16) | low_bits);
        if ((pid_high_state >> 16) == (pid >> 16)) {
            states[state_count++] = pid_high_state;
        }
    }
    return state_count;
}

static bool is_drawing_method(spec_gba_iv_method_t method) {
    return method == SPEC_GBA_IV_METHOD_STRAIGHT || method == SPEC_GBA_IV_METHOD_SKIP_BEFORE
           || method == SPEC_GBA_IV_METHOD_SKIP_BETWEEN;
}

static void draw_ivs(uint8_t ivs[static SPEC_STAT_COUNT], uint32_t pid_high_state,
                     spec_gba_iv_method_t method) {
    uint32_t first_state = next_state(pid_high_state);
    uint32_t second_state = next_state(first_state);
    uint32_t third_state = next_state(second_state);
    uint16_t first_random = (uint16_t)(first_state >> 16);
    uint16_t second_random = (uint16_t)(second_state >> 16);
    uint16_t third_random = (uint16_t)(third_state >> 16);
    if (method == SPEC_GBA_IV_METHOD_STRAIGHT) {
        ivs_from_random(ivs, first_random, second_random);
    } else if (method == SPEC_GBA_IV_METHOD_SKIP_BEFORE) {
        ivs_from_random(ivs, second_random, third_random);
    } else {
        ivs_from_random(ivs, first_random, third_random);
    }
}

spec_gba_personality_t spec_gba_decode_personality(spec_gba_pid_t pid, uint16_t species,
                                                   const spec_gba_trainer_t *trainer) {
    spec_gba_personality_t personality = {
        .pid = pid,
        .nature = (spec_nature_t)(pid % SPEC_NATURE_COUNT),
        .gender = gender_of(pid, species),
        .is_shiny = is_shiny_for(pid, trainer),
        .unown_form = unown_form_of(pid),
    };
    return personality;
}

spec_gba_iv_method_t spec_gba_find_iv_method(spec_gba_pid_t pid,
                                             const uint8_t ivs[static SPEC_STAT_COUNT]) {
    constexpr spec_gba_iv_method_t METHODS[] = {
        SPEC_GBA_IV_METHOD_STRAIGHT,
        SPEC_GBA_IV_METHOD_SKIP_BEFORE,
        SPEC_GBA_IV_METHOD_SKIP_BETWEEN,
    };
    uint32_t states[SPEC_GBA_EXPECTED_IV_SETS_MAX];
    size_t state_count = find_pid_states(pid, states);
    for (size_t state = 0; state < state_count; ++state) {
        for (size_t method = 0; method < sizeof METHODS / sizeof METHODS[0]; ++method) {
            uint8_t drawn_ivs[SPEC_STAT_COUNT];
            draw_ivs(drawn_ivs, states[state], METHODS[method]);
            if (memcmp(drawn_ivs, ivs, SPEC_STAT_COUNT) == 0) {
                return METHODS[method];
            }
        }
    }
    return SPEC_GBA_IV_METHOD_NONE;
}

size_t
spec_gba_find_expected_ivs(spec_gba_pid_t pid, spec_gba_iv_method_t method,
                           uint8_t iv_sets[static SPEC_GBA_EXPECTED_IV_SETS_MAX][SPEC_STAT_COUNT]) {
    if (!is_drawing_method(method)) {
        return 0;
    }
    uint32_t states[SPEC_GBA_EXPECTED_IV_SETS_MAX];
    size_t state_count = find_pid_states(pid, states);
    for (size_t state = 0; state < state_count; ++state) {
        draw_ivs(iv_sets[state], states[state], method);
    }
    return state_count;
}

void spec_gba_pid_next(spec_gba_pid_t *pid) {
    *pid = next_state(*pid);
}

uint16_t spec_gba_random(uint32_t *seed) {
    *seed = next_state(*seed);
    return (uint16_t)(*seed >> 16);
}
