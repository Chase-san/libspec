// The Gen 2 Pokédex: seen and caught flags, whether the player has it, and the Unown Dex.

#include <string.h>

#include "gbc/gbc.h"
#include "gbc/gbc_internal.h"
#include "spec_internal.h"

constexpr size_t POKEDEX_FLAGS_SIZE = 32;
constexpr unsigned HAS_POKEDEX_BIT = 0;
constexpr unsigned HAS_UNOWN_DEX_BIT = 1;
// The puzzles' unlocked letters come between the two, and are left as they are.
constexpr size_t FIRST_UNOWN_SEEN_DISTANCE = SPEC_GBC_UNOWN_FORM_COUNT + 1;

static bool is_flag_set(const uint8_t *flags, size_t flag) {
    return spec_get_bits(flags[flag / 8], (unsigned)(flag % 8), 1) != 0;
}

static void set_flag(uint8_t *flags, size_t flag, bool is_set) {
    flags[flag / 8] = (uint8_t)spec_set_bits(flags[flag / 8], (unsigned)(flag % 8), 1, is_set);
}

void spec_gbc_decode_pokedex(spec_gbc_pokedex_t *pokedex, const uint8_t *data,
                             const spec_gbc_layout_t *layout) {
    const uint8_t *caught = &data[layout->pokedex_caught_offset];
    const uint8_t *seen = &data[layout->pokedex_seen_offset];
    for (size_t national_number = 1; national_number < SPEC_GBC_POKEDEX_SIZE; ++national_number) {
        pokedex->is_caught[national_number] = is_flag_set(caught, national_number - 1);
        pokedex->is_seen[national_number] = is_flag_set(seen, national_number - 1);
    }
    uint8_t status_flags = data[layout->status_flags_offset];
    pokedex->is_obtained = spec_get_bits(status_flags, HAS_POKEDEX_BIT, 1) != 0;
    pokedex->has_unown_dex = spec_get_bits(status_flags, HAS_UNOWN_DEX_BIT, 1) != 0;
    memcpy(pokedex->unown_caught_order, &data[layout->unown_dex_offset], SPEC_GBC_UNOWN_FORM_COUNT);
    pokedex->first_unown_seen = data[layout->unown_dex_offset + FIRST_UNOWN_SEEN_DISTANCE];
}

spec_error_t spec_gbc_check_pokedex(const spec_gbc_pokedex_t *pokedex) {
    for (size_t index = 0; index < SPEC_GBC_UNOWN_FORM_COUNT; ++index) {
        if (pokedex->unown_caught_order[index] > SPEC_GBC_UNOWN_FORM_COUNT) {
            return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                             "unown_caught_order holds no Unown form");
        }
    }
    if (pokedex->first_unown_seen > SPEC_GBC_UNOWN_FORM_COUNT) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "first_unown_seen is no Unown form");
    }
    return SPEC_OK;
}

// As catching does, a caught species is marked seen too.
void spec_gbc_encode_pokedex(uint8_t *data, const spec_gbc_layout_t *layout,
                             const spec_gbc_pokedex_t *pokedex) {
    uint8_t caught[POKEDEX_FLAGS_SIZE] = {};
    uint8_t seen[POKEDEX_FLAGS_SIZE] = {};
    for (size_t national_number = 1; national_number < SPEC_GBC_POKEDEX_SIZE; ++national_number) {
        bool is_caught = pokedex->is_caught[national_number];
        set_flag(caught, national_number - 1, is_caught);
        set_flag(seen, national_number - 1, is_caught || pokedex->is_seen[national_number]);
    }
    memcpy(&data[layout->pokedex_caught_offset], caught, sizeof caught);
    memcpy(&data[layout->pokedex_seen_offset], seen, sizeof seen);
    uint32_t status_flags = data[layout->status_flags_offset];
    status_flags = spec_set_bits(status_flags, HAS_POKEDEX_BIT, 1, pokedex->is_obtained);
    status_flags = spec_set_bits(status_flags, HAS_UNOWN_DEX_BIT, 1, pokedex->has_unown_dex);
    data[layout->status_flags_offset] = (uint8_t)status_flags;
    memcpy(&data[layout->unown_dex_offset], pokedex->unown_caught_order, SPEC_GBC_UNOWN_FORM_COUNT);
    data[layout->unown_dex_offset + FIRST_UNOWN_SEEN_DISTANCE] = pokedex->first_unown_seen;
}
