// Gen 2 internals: layouts, the primary and backup copies, and the codecs save.c runs.

#ifndef SPEC_GBC_INTERNAL_H
#define SPEC_GBC_INTERNAL_H

#include <stddef.h>
#include <stdint.h>

#include "gb/gb_internal.h"
#include "gbc/gbc.h"

// The checksummed game data starts with the trainer's ID, after the check value.
constexpr size_t SPEC_GBC_GAME_DATA_OFFSET = 0x2009;
constexpr size_t SPEC_GBC_BACKUP_CHUNK_MAX_COUNT = 5;
constexpr uint8_t SPEC_GBC_EGG = 0xFD;

// A run of game data and where its backup copy lies.
struct spec_gbc_backup_chunk {
    size_t primary_offset;
    size_t size;
    size_t backup_offset;
};
typedef struct spec_gbc_backup_chunk spec_gbc_backup_chunk_t;

// Offsets are file offsets.
struct spec_gbc_layout {
    size_t save_size;
    size_t name_size;
    size_t box_count;
    size_t boxes_per_bank;
    spec_gb_list_shape_t party_shape;
    spec_gb_list_shape_t box_shape;

    size_t game_data_size;
    size_t checksum_offset;
    size_t backup_check_value_offset;
    size_t backup_checksum_offset;
    size_t backup_chunk_count;
    spec_gbc_backup_chunk_t backup_chunks[SPEC_GBC_BACKUP_CHUNK_MAX_COUNT];

    size_t mail_size;
    size_t mail_author_size;
    bool has_mail_nationality;
    size_t party_mail_offset;
    size_t party_mail_backup_offset;
    size_t mailbox_offset;
    size_t mailbox_backup_offset;

    size_t rival_name_offset;
    size_t play_time_offset;
    size_t status_flags_offset;
    size_t money_offset;
    size_t coins_offset;
    size_t badges_offset;
    size_t tms_hms_offset;
    size_t items_offset;
    size_t key_items_offset;
    size_t balls_offset;
    size_t pc_items_offset;
    size_t current_box_offset;
    size_t box_names_offset;
    size_t party_offset;
    size_t pokedex_caught_offset;
    size_t pokedex_seen_offset;
    size_t unown_dex_offset;
    size_t daycare_offset;
    size_t active_box_offset;
    bool has_player_gender;
    size_t player_gender_offset;
};
typedef struct spec_gbc_layout spec_gbc_layout_t;

const spec_gbc_layout_t *spec_gbc_get_layout(spec_game_type_t type, spec_language_t language);

bool spec_gbc_has_save(const uint8_t *data, const spec_gbc_layout_t *layout);
bool spec_gbc_is_primary_valid(const uint8_t *data, const spec_gbc_layout_t *layout);
bool spec_gbc_is_backup_valid(const uint8_t *data, const spec_gbc_layout_t *layout);
void spec_gbc_restore_primary(uint8_t *data, const spec_gbc_layout_t *layout);
void spec_gbc_stamp_copies(uint8_t *data, const spec_gbc_layout_t *layout);

void spec_gbc_decode_player(spec_gbc_save_t *save, const uint8_t *data,
                            const spec_gbc_layout_t *layout);
spec_error_t spec_gbc_check_player(const spec_gbc_save_t *save, const spec_gbc_layout_t *layout);
void spec_gbc_encode_player(uint8_t *data, const spec_gbc_layout_t *layout,
                            const spec_gbc_save_t *save);

void spec_gbc_decode_pokedex(spec_gbc_pokedex_t *pokedex, const uint8_t *data,
                             const spec_gbc_layout_t *layout);
spec_error_t spec_gbc_check_pokedex(const spec_gbc_pokedex_t *pokedex);
void spec_gbc_encode_pokedex(uint8_t *data, const spec_gbc_layout_t *layout,
                             const spec_gbc_pokedex_t *pokedex);

void spec_gbc_decode_storage(spec_gbc_save_t *save, const uint8_t *data,
                             const spec_gbc_layout_t *layout);
spec_error_t spec_gbc_check_storage(const spec_gbc_save_t *save, const spec_gbc_layout_t *layout);
void spec_gbc_encode_storage(uint8_t *data, const spec_gbc_layout_t *layout,
                             const spec_gbc_save_t *save);

// Names live beside the record, so decoding leaves them, and is_egg, as they were.
void spec_gbc_decode_pokemon(spec_gbc_pokemon_t *pokemon, const uint8_t *record,
                             size_t record_size);
spec_error_t spec_gbc_encode_pokemon(uint8_t *record, size_t record_size,
                                     const spec_gbc_pokemon_t *pokemon);
void spec_gbc_fill_party_data(spec_gbc_pokemon_t *pokemon);

spec_error_t spec_gbc_check_mail(const spec_gbc_mail_t *mail, const spec_gbc_layout_t *layout);
void spec_gbc_decode_party_mail(spec_gbc_save_t *save, const uint8_t *data,
                                const spec_gbc_layout_t *layout);
void spec_gbc_encode_party_mail(uint8_t *data, const spec_gbc_layout_t *layout,
                                const spec_gbc_save_t *save);

bool spec_gbc_is_mail(uint16_t item);
void spec_gbc_decode_items(spec_gbc_save_t *save, const uint8_t *data,
                           const spec_gbc_layout_t *layout);
spec_error_t spec_gbc_check_items(const spec_gbc_save_t *save);
void spec_gbc_encode_items(uint8_t *data, const spec_gbc_layout_t *layout,
                           const spec_gbc_save_t *save);

#endif
