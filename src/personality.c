// The Gen 3-4 random number generator, and the gender and shininess a PID decides.

#include <string.h>

#include "spec.h"
#include "spec_internal.h"

constexpr uint32_t RNG_MULTIPLIER = 0x41C64E6D;
constexpr uint32_t RNG_INCREMENT = 0x6073;
constexpr uint32_t SHINY_ODDS = 8;
constexpr uint8_t GENDER_RATIO_ALWAYS_MALE = 0;
constexpr uint8_t GENDER_RATIO_ALWAYS_FEMALE = 254;
constexpr uint8_t GENDER_RATIO_GENDERLESS = 255;
constexpr unsigned IV_BIT_COUNT = 5;
constexpr uint32_t STATE_LOW_BITS_COUNT = 1 << 16;

static uint32_t next_state(uint32_t state) {
    return state * RNG_MULTIPLIER + RNG_INCREMENT;
}

// States after drawing the PID as CreateBoxMon does, low half first.
static size_t find_pid_states(spec_pid_t pid, uint32_t states[static SPEC_EXPECTED_IV_SETS_MAX]) {
    size_t state_count = 0;
    for (uint32_t low_bits = 0;
         low_bits < STATE_LOW_BITS_COUNT && state_count < SPEC_EXPECTED_IV_SETS_MAX; ++low_bits) {
        uint32_t pid_high_state = next_state((pid << 16) | low_bits);
        if ((pid_high_state >> 16) == (pid >> 16)) {
            states[state_count++] = pid_high_state;
        }
    }
    return state_count;
}

static void ivs_from_random(uint8_t ivs[static SPEC_STAT_COUNT], uint16_t first_random,
                            uint16_t second_random) {
    for (unsigned stat = 0; stat < 3; ++stat) {
        ivs[stat] = (uint8_t)spec_get_bits(first_random, stat * IV_BIT_COUNT, IV_BIT_COUNT);
        ivs[stat + 3] = (uint8_t)spec_get_bits(second_random, stat * IV_BIT_COUNT, IV_BIT_COUNT);
    }
}

static void draw_ivs(uint8_t ivs[static SPEC_STAT_COUNT], uint32_t pid_high_state,
                     spec_iv_method_t method) {
    uint32_t first_state = next_state(pid_high_state);
    uint32_t second_state = next_state(first_state);
    uint32_t third_state = next_state(second_state);
    uint16_t first_random = (uint16_t)(first_state >> 16);
    uint16_t second_random = (uint16_t)(second_state >> 16);
    uint16_t third_random = (uint16_t)(third_state >> 16);
    if (method == SPEC_IV_METHOD_STRAIGHT) {
        ivs_from_random(ivs, first_random, second_random);
    } else if (method == SPEC_IV_METHOD_SKIP_BEFORE) {
        ivs_from_random(ivs, second_random, third_random);
    } else {
        ivs_from_random(ivs, first_random, third_random);
    }
}

// As GetGenderFromSpeciesAndPersonality.
spec_gender_t spec_gender_from_ratio(spec_pid_t pid, uint8_t gender_ratio) {
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
bool spec_is_shiny(spec_pid_t pid, uint16_t trainer_id, uint16_t secret_id) {
    uint32_t shiny_value = trainer_id ^ secret_id ^ (pid >> 16) ^ (pid & 0xFFFF);
    return shiny_value < SHINY_ODDS;
}

static bool is_drawing_method(spec_iv_method_t method) {
    return method == SPEC_IV_METHOD_STRAIGHT || method == SPEC_IV_METHOD_SKIP_BEFORE
           || method == SPEC_IV_METHOD_SKIP_BETWEEN;
}

size_t spec_find_expected_ivs(spec_pid_t pid, spec_iv_method_t method,
                              uint8_t iv_sets[static SPEC_EXPECTED_IV_SETS_MAX][SPEC_STAT_COUNT]) {
    if (!is_drawing_method(method)) {
        return 0;
    }
    uint32_t states[SPEC_EXPECTED_IV_SETS_MAX];
    size_t state_count = find_pid_states(pid, states);
    for (size_t state = 0; state < state_count; ++state) {
        draw_ivs(iv_sets[state], states[state], method);
    }
    return state_count;
}

spec_iv_method_t spec_find_iv_method(spec_pid_t pid, const uint8_t ivs[static SPEC_STAT_COUNT]) {
    constexpr spec_iv_method_t METHODS[] = {
        SPEC_IV_METHOD_STRAIGHT,
        SPEC_IV_METHOD_SKIP_BEFORE,
        SPEC_IV_METHOD_SKIP_BETWEEN,
    };
    uint32_t states[SPEC_EXPECTED_IV_SETS_MAX];
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
    return SPEC_IV_METHOD_NONE;
}

uint16_t spec_random(uint32_t *seed) {
    *seed = next_state(*seed);
    return (uint16_t)(*seed >> 16);
}
