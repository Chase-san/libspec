// Gen 1 internals: layouts, Pokémon lists, checksums, the codecs save.c runs and the text engine.

#ifndef SPEC_GB_INTERNAL_H
#define SPEC_GB_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <uchar.h>

#include "gb/gb.h"
#include "gb/tables.h"

constexpr size_t SPEC_GB_BANK_SIZE = 0x2000;
constexpr size_t SPEC_GB_FIRST_BOX_BANK = 2;
constexpr size_t SPEC_GB_BOX_BANK_COUNT = 2;
// The checksummed game data starts with the player's name.
constexpr size_t SPEC_GB_GAME_DATA_OFFSET = 0x2598;
constexpr uint8_t SPEC_GB_END_OF_TEXT = 0x50;
constexpr uint8_t SPEC_GB_END_OF_LIST = 0xFF;

// A Pokémon list: a count, the species bytes and their terminator, the records, the OT names,
// then the nicknames.
struct spec_gb_list_shape {
    size_t capacity;
    size_t record_size;
    size_t name_size;
};
typedef struct spec_gb_list_shape spec_gb_list_shape_t;

// Offsets are file offsets; Japanese saves move most fields.
struct spec_gb_layout {
    size_t name_size;
    size_t box_count;
    size_t boxes_per_bank;
    spec_gb_list_shape_t party_shape;
    spec_gb_list_shape_t box_shape;
    bool has_box_checksums;
    bool does_bank_checksum_sum_itself;

    size_t pokedex_caught_offset;
    size_t pokedex_seen_offset;
    size_t bag_offset;
    size_t money_offset;
    size_t rival_name_offset;
    size_t badges_offset;
    size_t trainer_id_offset;
    size_t pikachu_friendship_offset;
    size_t pc_items_offset;
    size_t current_box_offset;
    size_t coins_offset;
    size_t event_flags_offset;
    size_t play_time_offset;
    size_t daycare_offset;
    size_t party_offset;
    size_t current_box_list_offset;
    size_t checksum_offset;
};
typedef struct spec_gb_layout spec_gb_layout_t;

// Layout functions

const spec_gb_layout_t *spec_gb_get_layout(spec_language_t language);

// Pokémon list functions

size_t spec_gb_list_nickname_offset(const spec_gb_list_shape_t *shape, size_t index);
size_t spec_gb_list_record_offset(const spec_gb_list_shape_t *shape, size_t index);
size_t spec_gb_list_size(const spec_gb_list_shape_t *shape);
size_t spec_gb_list_species_offset(const spec_gb_list_shape_t *shape, size_t index);
size_t spec_gb_list_trainer_name_offset(const spec_gb_list_shape_t *shape, size_t index);

// Checksum functions

uint8_t spec_gb_checksum(const uint8_t *bytes, size_t size);
bool spec_gb_is_game_data_valid(const uint8_t *data, const spec_gb_layout_t *layout);

void spec_gb_stamp_box_bank(uint8_t *data, const spec_gb_layout_t *layout, size_t bank);
void spec_gb_stamp_game_data(uint8_t *data, const spec_gb_layout_t *layout);

// Player functions

void spec_gb_decode_player(spec_gb_save_t *save, const uint8_t *data,
                           const spec_gb_layout_t *layout);
void spec_gb_encode_player(uint8_t *data, const spec_gb_layout_t *layout,
                           const spec_gb_save_t *save);

spec_error_t spec_gb_check_player(const spec_gb_save_t *save, const spec_gb_layout_t *layout);

// Pokédex functions

void spec_gb_decode_pokedex(spec_gb_pokedex_t *pokedex, const uint8_t *data,
                            const spec_gb_layout_t *layout);
void spec_gb_encode_pokedex(uint8_t *data, const spec_gb_layout_t *layout,
                            const spec_gb_pokedex_t *pokedex);

// Storage functions

void spec_gb_decode_storage(spec_gb_save_t *save, const uint8_t *data,
                            const spec_gb_layout_t *layout);
void spec_gb_encode_storage(uint8_t *data, const spec_gb_layout_t *layout,
                            const spec_gb_save_t *save);

spec_error_t spec_gb_check_storage(const spec_gb_save_t *save, const spec_gb_layout_t *layout);

// Pokémon functions

// The DVs' two bytes hold Attack, Defense, Speed and Special; HP's is their low bits.
void spec_gb_decode_dvs(uint8_t dvs[static SPEC_GB_STAT_COUNT], const uint8_t *bytes);
// Each move's PP byte keeps its PP Ups in the top two bits.
void spec_gb_decode_moves(spec_gb_move_t moves[static SPEC_GB_MOVE_COUNT], const uint8_t *ids,
                          const uint8_t *pp_bytes);
// Names live beside the record, so decoding leaves them as they were.
void spec_gb_decode_pokemon(spec_gb_pokemon_t *pokemon, const uint8_t *record, size_t record_size);
void spec_gb_decode_status(spec_gb_status_t *status, uint8_t byte);
void spec_gb_encode_dvs(uint8_t *bytes, const uint8_t dvs[static SPEC_GB_STAT_COUNT]);
void spec_gb_encode_moves(uint8_t *ids, uint8_t *pp_bytes,
                          const spec_gb_move_t moves[static SPEC_GB_MOVE_COUNT]);
spec_error_t spec_gb_encode_pokemon(uint8_t *record, size_t record_size,
                                    const spec_gb_pokemon_t *pokemon);
uint8_t spec_gb_encode_status(const spec_gb_status_t *status);

// As CalcStat, with stat experience's square-root bonus.
uint16_t spec_gb_calculate_stat(uint8_t base_stat, uint8_t dv, uint16_t stat_experience,
                                uint8_t level, bool is_hp);
uint8_t spec_gb_hp_dv(const uint8_t dvs[static SPEC_GB_STAT_COUNT]);

void spec_gb_fill_party_data(spec_gb_pokemon_t *pokemon);

// Item functions

void spec_gb_decode_items(spec_gb_save_t *save, const uint8_t *data,
                          const spec_gb_layout_t *layout);
void spec_gb_encode_items(uint8_t *data, const spec_gb_layout_t *layout,
                          const spec_gb_save_t *save);

spec_error_t spec_gb_check_items(const spec_gb_save_t *save);

// Text functions

spec_error_t spec_gb_decode_text(char8_t *utf8, const uint8_t *text, size_t text_size,
                                 const spec_gb_character_t *charmap);
spec_error_t spec_gb_encode_text(uint8_t *text, size_t text_size, const char8_t *utf8,
                                 const spec_gb_character_t *charmap);

size_t spec_gb_name_size(spec_language_t language);

#endif
