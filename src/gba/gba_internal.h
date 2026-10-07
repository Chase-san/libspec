// Gen 3 internals: save layouts, flash slot access and the codecs save.c runs.

#ifndef SPEC_GBA_INTERNAL_H
#define SPEC_GBA_INTERNAL_H

#include <stddef.h>
#include <stdint.h>

#include "gba/gba.h"

constexpr size_t SPEC_GBA_SECTION_COUNT = 14;        // pret/pokeemerald NUM_SECTORS_PER_SLOT
constexpr size_t SPEC_GBA_SECTION_DATA_SIZE = 0xF80; // pret/pokeemerald SECTOR_DATA_SIZE
constexpr size_t SPEC_GBA_POKEDEX_SEEN_COPY_COUNT = 3;

struct spec_gba_national_dex_layout {
    size_t magic_offset;
    uint8_t magic;
    size_t var_offset;
    uint16_t var_value;
    uint16_t flag;
    bool does_enabling_pick_national_mode;
};
typedef struct spec_gba_national_dex_layout spec_gba_national_dex_layout_t;

struct spec_gba_daycare_layout {
    size_t slot_count;
    size_t record_offsets[SPEC_GBA_DAYCARE_CAPACITY];
    size_t steps_offsets[SPEC_GBA_DAYCARE_CAPACITY];
    uint16_t egg_waiting_flag;
    size_t egg_personality_offset;
    size_t egg_personality_size;
    size_t step_counter_offset;
};
typedef struct spec_gba_daycare_layout spec_gba_daycare_layout_t;

// Offsets run through section data in section id order.
struct spec_gba_layout {
    spec_game_type_t type;
    uint16_t section_sizes[SPEC_GBA_SECTION_COUNT];

    size_t trainer_name_offset;
    size_t trainer_gender_offset;
    size_t trainer_id_offset;
    size_t secret_id_offset;
    size_t play_time_offset;
    size_t button_mode_offset;
    size_t options_offset;
    uint8_t window_frame_count;
    bool has_security_key;
    size_t security_key_offset;
    size_t money_offset;
    size_t coins_offset;
    bool has_battle_points;
    size_t battle_points_offset;
    bool has_rival_name;
    size_t rival_name_offset;
    size_t flags_offset;
    uint16_t first_badge_flag;

    uint16_t pokedex_flag;
    size_t pokedex_order_offset;
    size_t pokedex_mode_offset;
    size_t unown_personality_offset;
    size_t spinda_personality_offset;
    size_t pokedex_caught_offset;
    size_t pokedex_seen_offsets[SPEC_GBA_POKEDEX_SEEN_COPY_COUNT];
    spec_gba_national_dex_layout_t national_dex;

    size_t party_count_offset;
    size_t party_offset;
    size_t current_box_offset;
    size_t box_records_offset;
    size_t box_names_offset;
    size_t wallpapers_offset;
    uint8_t wallpaper_count;
    spec_gba_daycare_layout_t daycare;

    size_t pocket_offsets[SPEC_GBA_POCKET_COUNT];
};
typedef struct spec_gba_layout spec_gba_layout_t;

struct spec_gba_save_slot {
    size_t first_sector;
    uint8_t position_of_section[SPEC_GBA_SECTION_COUNT];
    uint32_t counter;
};
typedef struct spec_gba_save_slot spec_gba_save_slot_t;

// Layout functions

const spec_gba_layout_t *spec_gba_get_layout(spec_game_type_t type);

// Save slot functions

void spec_gba_read_slot_bytes(uint8_t *bytes, const uint8_t *data, const spec_gba_save_slot_t *slot,
                              size_t offset, size_t size);
bool spec_gba_read_slot_flag(const uint8_t *data, const spec_gba_save_slot_t *slot,
                             size_t flags_offset, uint16_t flag);
uint16_t spec_gba_read_slot_u16(const uint8_t *data, const spec_gba_save_slot_t *slot,
                                size_t offset);
uint32_t spec_gba_read_slot_u32(const uint8_t *data, const spec_gba_save_slot_t *slot,
                                size_t offset);
uint8_t spec_gba_read_slot_u8(const uint8_t *data, const spec_gba_save_slot_t *slot, size_t offset);
void spec_gba_write_slot_bytes(uint8_t *data, const spec_gba_save_slot_t *slot, size_t offset,
                               const uint8_t *bytes, size_t size);
void spec_gba_write_slot_flag(uint8_t *data, const spec_gba_save_slot_t *slot, size_t flags_offset,
                              uint16_t flag, bool is_set);
void spec_gba_write_slot_u16(uint8_t *data, const spec_gba_save_slot_t *slot, size_t offset,
                             uint16_t value);
void spec_gba_write_slot_u32(uint8_t *data, const spec_gba_save_slot_t *slot, size_t offset,
                             uint32_t value);
void spec_gba_write_slot_u8(uint8_t *data, const spec_gba_save_slot_t *slot, size_t offset,
                            uint8_t value);

bool spec_gba_find_active_slot(spec_gba_save_slot_t *active, const uint8_t *data,
                               const uint16_t section_sizes[static SPEC_GBA_SECTION_COUNT]);

spec_gba_save_slot_t spec_gba_copy_to_next_slot(uint8_t *data, const spec_gba_save_slot_t *active);
void spec_gba_stamp_slot(uint8_t *data, const spec_gba_save_slot_t *slot,
                         const uint16_t section_sizes[static SPEC_GBA_SECTION_COUNT]);

// Player functions

void spec_gba_decode_player(spec_gba_save_t *save, const uint8_t *data,
                            const spec_gba_save_slot_t *slot, const spec_gba_layout_t *layout);
void spec_gba_encode_player(uint8_t *data, const spec_gba_save_slot_t *slot,
                            const spec_gba_layout_t *layout, const spec_gba_save_t *save);

spec_error_t spec_gba_check_player(const spec_gba_save_t *save, const spec_gba_layout_t *layout);
uint32_t spec_gba_read_security_key(const uint8_t *data, const spec_gba_save_slot_t *slot,
                                    const spec_gba_layout_t *layout);

// Pokédex functions

void spec_gba_decode_pokedex(spec_gba_pokedex_t *pokedex, const uint8_t *data,
                             const spec_gba_save_slot_t *slot, const spec_gba_layout_t *layout);
void spec_gba_encode_pokedex(uint8_t *data, const spec_gba_save_slot_t *slot,
                             const spec_gba_layout_t *layout, const spec_gba_pokedex_t *pokedex);

// Storage functions

void spec_gba_decode_storage(spec_gba_save_t *save, const uint8_t *data,
                             const spec_gba_save_slot_t *slot, const spec_gba_layout_t *layout);
void spec_gba_encode_storage(uint8_t *data, const spec_gba_save_slot_t *slot,
                             const spec_gba_layout_t *layout, const spec_gba_save_t *save);

spec_error_t spec_gba_check_storage(const spec_gba_save_t *save, const spec_gba_layout_t *layout);

// Pokémon functions

// Records are encrypted, as stored.
void spec_gba_decode_pokemon(spec_gba_pokemon_t *pokemon, const uint8_t *record,
                             size_t record_size);
spec_error_t spec_gba_encode_pokemon(uint8_t *record, size_t record_size,
                                     const spec_gba_pokemon_t *pokemon);

void spec_gba_fill_party_data(spec_gba_pokemon_t *pokemon);

// Item functions

void spec_gba_decode_items(spec_gba_save_t *save, const uint8_t *data,
                           const spec_gba_save_slot_t *slot, const spec_gba_layout_t *layout);
void spec_gba_encode_items(uint8_t *data, const spec_gba_save_slot_t *slot,
                           const spec_gba_layout_t *layout, const spec_gba_save_t *save);

spec_error_t spec_gba_check_items(const spec_gba_save_t *save, const spec_gba_layout_t *layout);

// Text functions

// A name that fills its field gets its terminator in the byte after it, as the naming screen
// writes it; otherwise that byte is left as the game wrote it.
void spec_gba_write_slot_name(uint8_t *data, const spec_gba_save_slot_t *slot, size_t offset,
                              const uint8_t *name, size_t name_size);

#endif
