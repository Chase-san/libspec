// Gen 4 internals: save layouts, block selection and the codecs save.c runs.

#ifndef SPEC_NDS_INTERNAL_H
#define SPEC_NDS_INTERNAL_H

#include <stddef.h>
#include <stdint.h>

#include "nds/nds.h"

constexpr size_t SPEC_NDS_PARTITION_SIZE = 0x40000;

// Offsets are Pokédex-relative.
struct spec_nds_pokedex_layout {
    size_t offset;
    bool records_every_species_language;
    size_t languages_offset;
    size_t form_view_offset;
    size_t language_view_offset;
    size_t obtained_offset;
    size_t national_dex_offset;
    bool has_platinum_forms;
    size_t rotom_offset;
    size_t shaymin_offset;
    size_t giratina_offset;
    bool has_heartgold_soulsilver_forms;
    size_t unown_caught_offset;
    size_t pichu_offset;
};
typedef struct spec_nds_pokedex_layout spec_nds_pokedex_layout_t;

// General offsets are general-block-relative, storage offsets storage-block-relative.
struct spec_nds_layout {
    spec_game_type_t type;
    size_t general_size;
    size_t storage_offset;
    size_t storage_size;
    size_t footer_size;
    // HeartGold and SoulSilver save both blocks to one partition under one counter.
    bool are_blocks_paired;

    size_t player_offset;
    bool has_kanto_badges;
    size_t rival_name_offset;
    size_t battle_points_offset;
    spec_nds_pokedex_layout_t pokedex;

    size_t party_offset;
    size_t daycare_offset;
    size_t current_box_offset;
    size_t box_records_offset;
    size_t box_stride;
    size_t box_names_offset;
    size_t wallpapers_offset;
    // Platinum's and HeartGold and SoulSilver's unlockable wallpapers start at 24 and 32.
    uint8_t wallpaper_count;
    bool has_box_modified_flags;
    size_t box_modified_flags_offset;

    size_t pocket_offsets[SPEC_NDS_POCKET_COUNT];
};
typedef struct spec_nds_layout spec_nds_layout_t;

// Absolute offsets of the copies the game loads, or writes next.
struct spec_nds_block_pair {
    size_t general_offset;
    size_t storage_offset;
    uint32_t save_counter;
    uint32_t general_counter;
    uint32_t storage_counter;
};
typedef struct spec_nds_block_pair spec_nds_block_pair_t;

// Layout functions

const spec_nds_layout_t *spec_nds_get_layout(spec_game_type_t type);

// Block functions

bool spec_nds_find_loaded_blocks(spec_nds_block_pair_t *loaded, const uint8_t *data,
                                 const spec_nds_layout_t *layout);

spec_nds_block_pair_t spec_nds_copy_to_next_blocks(uint8_t *data,
                                                   const spec_nds_block_pair_t *loaded,
                                                   const spec_nds_layout_t *layout);
void spec_nds_stamp_blocks(uint8_t *data, const spec_nds_block_pair_t *blocks,
                           const spec_nds_layout_t *layout);

// Player functions

void spec_nds_decode_player(spec_nds_save_t *save, const uint8_t *general,
                            const spec_nds_layout_t *layout);
void spec_nds_encode_player(uint8_t *general, const spec_nds_layout_t *layout,
                            const spec_nds_save_t *save);

spec_error_t spec_nds_check_player(const spec_nds_save_t *save, const spec_nds_layout_t *layout);

// Pokédex functions

void spec_nds_decode_pokedex(spec_nds_pokedex_t *pokedex, const uint8_t *general,
                             const spec_nds_layout_t *layout);
void spec_nds_encode_pokedex(uint8_t *general, const spec_nds_layout_t *layout,
                             const spec_nds_pokedex_t *pokedex);

spec_error_t spec_nds_check_pokedex(const spec_nds_pokedex_t *pokedex,
                                    const spec_nds_layout_t *layout);

// Storage functions

void spec_nds_decode_storage(spec_nds_save_t *save, const uint8_t *general, const uint8_t *storage,
                             const spec_nds_layout_t *layout);
void spec_nds_encode_storage(uint8_t *general, uint8_t *storage, const spec_nds_layout_t *layout,
                             const spec_nds_save_t *save);

spec_error_t spec_nds_check_storage(const spec_nds_save_t *save, const spec_nds_layout_t *layout);

// As the game's next save writes only flagged boxes to the other partition.
void spec_nds_flag_changed_boxes(uint8_t *storage, const uint8_t *other_storage,
                                 const spec_nds_layout_t *layout);

// Pokémon functions

// Records are encrypted, as stored.
void spec_nds_decode_pokemon(spec_nds_pokemon_t *pokemon, const uint8_t *record,
                             size_t record_size);
void spec_nds_decode_status(spec_nds_status_t *status, uint32_t word);
spec_error_t spec_nds_encode_pokemon(uint8_t *record, size_t record_size,
                                     const spec_nds_pokemon_t *pokemon);
uint32_t spec_nds_encode_status(const spec_nds_status_t *status);

uint16_t spec_nds_current_hp_after(uint16_t current_hp, uint16_t old_max_hp, uint16_t new_max_hp,
                                   bool is_shedinja);

void spec_nds_fill_party_data(spec_nds_pokemon_t *pokemon);

// Mail functions

void spec_nds_decode_mail(spec_nds_mail_t *mail, const uint8_t *bytes);
// Gen 5 mail shares the sentence format.
void spec_nds_decode_mail_sentences(
    spec_nds_mail_sentence_t sentences[static SPEC_NDS_MAIL_SENTENCE_COUNT], const uint8_t *bytes);
void spec_nds_encode_mail(uint8_t *bytes, const spec_nds_mail_t *mail);
void spec_nds_encode_mail_sentences(
    uint8_t *bytes, const spec_nds_mail_sentence_t sentences[static SPEC_NDS_MAIL_SENTENCE_COUNT]);
void spec_nds_init_mail_sentences(
    spec_nds_mail_sentence_t sentences[static SPEC_NDS_MAIL_SENTENCE_COUNT]);

const char *spec_nds_unencodable_mail_field_of(const spec_nds_mail_t *mail);

// Item functions

void spec_nds_decode_items(spec_nds_save_t *save, const uint8_t *general,
                           const spec_nds_layout_t *layout);
void spec_nds_encode_items(uint8_t *general, const spec_nds_layout_t *layout,
                           const spec_nds_save_t *save);

spec_error_t spec_nds_check_items(const spec_nds_save_t *save, const spec_nds_layout_t *layout);

// Text functions

void spec_nds_read_text(uint16_t *text, const uint8_t *bytes, size_t text_size);
void spec_nds_write_text(uint8_t *bytes, const uint16_t *text, size_t text_size);

#endif
