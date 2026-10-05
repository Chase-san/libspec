// The Gen 3 Pokédex: seen and caught flags, and the National Dex.

#include "gba/gba.h"
#include "gba/gba_internal.h"
#include "spec_internal.h"

constexpr size_t POKEDEX_FLAGS_SIZE = 0x34;
constexpr uint8_t POKEDEX_MODE_NATIONAL = 1;
constexpr uint8_t POKEDEX_ORDER_FIRST = 0;

static bool is_national_dex_enabled(const uint8_t *data, const spec_gba_slot_t *slot,
                                    const spec_gba_layout_t *layout) {
    const spec_gba_national_dex_layout_t *national_dex = &layout->national_dex;
    return spec_gba_read_slot_u8(data, slot, national_dex->magic_offset) == national_dex->magic
           && spec_gba_read_slot_u16(data, slot, national_dex->var_offset)
                  == national_dex->var_value
           && spec_gba_read_slot_flag(data, slot, layout->flags_offset, national_dex->flag);
}

// As EnableNationalPokedex and DisableNationalPokedex.
static void set_national_dex(uint8_t *data, const spec_gba_slot_t *slot,
                             const spec_gba_layout_t *layout, bool is_enabled) {
    const spec_gba_national_dex_layout_t *national_dex = &layout->national_dex;
    spec_gba_write_slot_u8(data, slot, national_dex->magic_offset,
                           is_enabled ? national_dex->magic : 0);
    spec_gba_write_slot_u16(data, slot, national_dex->var_offset,
                            is_enabled ? national_dex->var_value : 0);
    spec_gba_write_slot_flag(data, slot, layout->flags_offset, national_dex->flag, is_enabled);
    if (is_enabled && national_dex->does_enabling_pick_national_mode) {
        spec_gba_write_slot_u8(data, slot, layout->pokedex_mode_offset, POKEDEX_MODE_NATIONAL);
        spec_gba_write_slot_u8(data, slot, layout->pokedex_order_offset, POKEDEX_ORDER_FIRST);
    }
}

static bool is_pokedex_bit_set(const uint8_t *flags, size_t national_number) {
    return spec_get_bits(flags[(national_number - 1) / 8], (national_number - 1) % 8, 1) != 0;
}

static void set_pokedex_bit(uint8_t *flags, size_t national_number) {
    flags[(national_number - 1) / 8] |= (uint8_t)(1 << ((national_number - 1) % 8));
}

// As GetSetPokedexFlag: seen needs all three copies.
void spec_gba_decode_pokedex(spec_gba_pokedex_t *pokedex, const uint8_t *data,
                             const spec_gba_slot_t *slot, const spec_gba_layout_t *layout) {
    uint8_t caught[POKEDEX_FLAGS_SIZE];
    uint8_t seen[SPEC_GBA_POKEDEX_SEEN_COPY_COUNT][POKEDEX_FLAGS_SIZE];
    spec_gba_read_slot_bytes(caught, data, slot, layout->pokedex_caught_offset, sizeof caught);
    for (size_t copy = 0; copy < SPEC_GBA_POKEDEX_SEEN_COPY_COUNT; ++copy) {
        spec_gba_read_slot_bytes(seen[copy], data, slot, layout->pokedex_seen_offsets[copy],
                                 sizeof seen[copy]);
    }
    for (size_t national_number = 1; national_number < SPEC_GBA_POKEDEX_SIZE; ++national_number) {
        bool is_seen = is_pokedex_bit_set(seen[0], national_number)
                       && is_pokedex_bit_set(seen[1], national_number)
                       && is_pokedex_bit_set(seen[2], national_number);
        pokedex->is_seen[national_number] = is_seen;
        pokedex->is_caught[national_number] =
            is_seen && is_pokedex_bit_set(caught, national_number);
    }
    pokedex->is_obtained =
        spec_gba_read_slot_flag(data, slot, layout->flags_offset, layout->pokedex_flag);
    pokedex->has_national_dex = is_national_dex_enabled(data, slot, layout);
    pokedex->unown_personality =
        spec_gba_read_slot_u32(data, slot, layout->unown_personality_offset);
    pokedex->spinda_personality =
        spec_gba_read_slot_u32(data, slot, layout->spinda_personality_offset);
}

// Enabling resets the Pokédex mode, so only a change is written.
void spec_gba_encode_pokedex(uint8_t *data, const spec_gba_slot_t *slot,
                             const spec_gba_layout_t *layout, const spec_gba_pokedex_t *pokedex) {
    uint8_t caught[POKEDEX_FLAGS_SIZE] = {};
    uint8_t seen[POKEDEX_FLAGS_SIZE] = {};
    for (size_t national_number = 1; national_number < SPEC_GBA_POKEDEX_SIZE; ++national_number) {
        if (pokedex->is_caught[national_number]) {
            set_pokedex_bit(caught, national_number);
        }
        if (pokedex->is_seen[national_number] || pokedex->is_caught[national_number]) {
            set_pokedex_bit(seen, national_number);
        }
    }
    spec_gba_write_slot_bytes(data, slot, layout->pokedex_caught_offset, caught, sizeof caught);
    for (size_t copy = 0; copy < SPEC_GBA_POKEDEX_SEEN_COPY_COUNT; ++copy) {
        spec_gba_write_slot_bytes(data, slot, layout->pokedex_seen_offsets[copy], seen,
                                  sizeof seen);
    }
    spec_gba_write_slot_flag(data, slot, layout->flags_offset, layout->pokedex_flag,
                             pokedex->is_obtained);
    if (pokedex->has_national_dex != is_national_dex_enabled(data, slot, layout)) {
        set_national_dex(data, slot, layout, pokedex->has_national_dex);
    }
    spec_gba_write_slot_u32(data, slot, layout->unown_personality_offset,
                            pokedex->unown_personality);
    spec_gba_write_slot_u32(data, slot, layout->spinda_personality_offset,
                            pokedex->spinda_personality);
}
