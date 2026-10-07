// The Gen 1 Pokédex: seen and caught flags, and whether the player has it.

#include "gb/gb.h"
#include "gb/gb_internal.h"
#include "spec_internal.h"

constexpr size_t POKEDEX_FLAGS_SIZE = (SPEC_GB_POKEDEX_SIZE - 1 + 7) / 8;
constexpr size_t GOT_POKEDEX_EVENT = 0x25; // pret/pokered EVENT_GOT_POKEDEX

void spec_gb_decode_pokedex(spec_gb_pokedex_t *pokedex, const uint8_t *data,
                            const spec_gb_layout_t *layout) {
    const uint8_t *caught = &data[layout->pokedex_caught_offset];
    const uint8_t *seen = &data[layout->pokedex_seen_offset];
    for (size_t national_number = 1; national_number < SPEC_GB_POKEDEX_SIZE; ++national_number) {
        pokedex->is_caught[national_number] = spec_get_array_flag(caught, national_number - 1);
        pokedex->is_seen[national_number] = spec_get_array_flag(seen, national_number - 1);
    }
    pokedex->is_obtained =
        spec_get_array_flag(&data[layout->event_flags_offset], GOT_POKEDEX_EVENT);
}

// As catching does, a caught species is marked seen too.
void spec_gb_encode_pokedex(uint8_t *data, const spec_gb_layout_t *layout,
                            const spec_gb_pokedex_t *pokedex) {
    uint8_t caught[POKEDEX_FLAGS_SIZE] = {};
    uint8_t seen[POKEDEX_FLAGS_SIZE] = {};
    for (size_t national_number = 1; national_number < SPEC_GB_POKEDEX_SIZE; ++national_number) {
        bool is_caught = pokedex->is_caught[national_number];
        spec_set_array_flag(caught, national_number - 1, is_caught);
        spec_set_array_flag(seen, national_number - 1,
                            is_caught || pokedex->is_seen[national_number]);
    }
    for (size_t index = 0; index < POKEDEX_FLAGS_SIZE; ++index) {
        data[layout->pokedex_caught_offset + index] = caught[index];
        data[layout->pokedex_seen_offset + index] = seen[index];
    }
    spec_set_array_flag(&data[layout->event_flags_offset], GOT_POKEDEX_EVENT, pokedex->is_obtained);
}
