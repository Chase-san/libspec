#ifndef SPEC_GBA_INTERNAL_H
#define SPEC_GBA_INTERNAL_H

#include <stddef.h>
#include <stdint.h>

#include "gba/gba.h"

constexpr size_t SPEC_GBA_SECTION_COUNT = 14;
constexpr size_t SPEC_GBA_SECTION_DATA_SIZE = 0xF80;

// Slot offsets run through section data in section id order.
struct spec_gba_slot {
    size_t first_sector;
    uint8_t position_of_section[SPEC_GBA_SECTION_COUNT];
    uint32_t counter;
};
typedef struct spec_gba_slot spec_gba_slot_t;

bool spec_gba_find_active_slot(spec_gba_slot_t *active, const uint8_t *data,
                               const uint16_t section_sizes[static SPEC_GBA_SECTION_COUNT]);
spec_gba_slot_t spec_gba_copy_to_next_slot(uint8_t *data, const spec_gba_slot_t *active);
void spec_gba_stamp_slot(uint8_t *data, const spec_gba_slot_t *slot,
                         const uint16_t section_sizes[static SPEC_GBA_SECTION_COUNT]);
void spec_gba_read_slot_bytes(uint8_t *bytes, const uint8_t *data, const spec_gba_slot_t *slot,
                              size_t offset, size_t size);
void spec_gba_write_slot_bytes(uint8_t *data, const spec_gba_slot_t *slot, size_t offset,
                               const uint8_t *bytes, size_t size);

// Records are encrypted, as stored.
void spec_gba_decode_pokemon(spec_gba_pokemon_t *pokemon, const uint8_t *record,
                             size_t record_size);
spec_error_t spec_gba_encode_pokemon(uint8_t *record, size_t record_size,
                                     const spec_gba_pokemon_t *pokemon);

void spec_gba_fill_party_data(spec_gba_pokemon_t *pokemon);

spec_error_t spec_gba_check_item_placement(spec_game_type_t type, spec_gba_pocket_t pocket,
                                           uint16_t item);
bool spec_gba_is_item_slot_empty(const spec_gba_item_slot_t *item_slot);

#endif
