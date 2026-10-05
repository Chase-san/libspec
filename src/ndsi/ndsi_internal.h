// NDSi internals: save layouts, the two copies and the codecs save.c runs.

#ifndef SPEC_NDSI_INTERNAL_H
#define SPEC_NDSI_INTERNAL_H

#include <stddef.h>
#include <stdint.h>

#include "ndsi/ndsi.h"

constexpr size_t SPEC_NDSI_BLOCK_TRAILER_SIZE = 4;
constexpr size_t SPEC_NDSI_BLOCK_MAX_COUNT = 73;

// size is the payload the block's checksum covers, before its trailer.
struct spec_ndsi_block {
    size_t offset;
    size_t size;
};
typedef struct spec_ndsi_block spec_ndsi_block_t;

// Offsets are copy-relative.
struct spec_ndsi_layout {
    spec_game_type_t type;
    size_t copy_size;
    size_t block_count;
    spec_ndsi_block_t blocks[SPEC_NDSI_BLOCK_MAX_COUNT];
    size_t table_offset;
    size_t footer_offset;

    size_t trainer_offset;
    size_t misc_offset;
    bool has_rival_name;
    size_t rival_name_offset;
    size_t pokedex_offset;
    size_t spinda_offset;

    size_t party_offset;
    size_t daycare_offset;
    size_t box_info_offset;
    size_t first_box_offset;
    size_t bag_offset;
};
typedef struct spec_ndsi_layout spec_ndsi_layout_t;

const spec_ndsi_layout_t *spec_ndsi_get_layout(spec_game_type_t type);

bool spec_ndsi_find_loaded_copy(size_t *copy_offset, const uint8_t *data,
                                const spec_ndsi_layout_t *layout);
size_t spec_ndsi_copy_to_other(uint8_t *data, size_t loaded_offset,
                               const spec_ndsi_layout_t *layout);
void spec_ndsi_stamp_copy(uint8_t *data, size_t copy_offset, size_t previous_offset,
                          const spec_ndsi_layout_t *layout);
void spec_ndsi_mirror_copy(uint8_t *data, size_t from_offset, size_t to_offset,
                           const spec_ndsi_layout_t *layout);

void spec_ndsi_decode_player(spec_ndsi_save_t *save, const uint8_t *copy,
                             const spec_ndsi_layout_t *layout);
spec_error_t spec_ndsi_check_player(const spec_ndsi_save_t *save, const spec_ndsi_layout_t *layout);
void spec_ndsi_encode_player(uint8_t *copy, const spec_ndsi_layout_t *layout,
                             const spec_ndsi_save_t *save);

void spec_ndsi_decode_pokedex(spec_ndsi_pokedex_t *pokedex, const uint8_t *copy,
                              const spec_ndsi_layout_t *layout);
spec_error_t spec_ndsi_check_pokedex(const spec_ndsi_pokedex_t *pokedex);
void spec_ndsi_encode_pokedex(uint8_t *copy, const spec_ndsi_layout_t *layout,
                              const spec_ndsi_pokedex_t *pokedex);

void spec_ndsi_decode_storage(spec_ndsi_save_t *save, const uint8_t *copy,
                              const spec_ndsi_layout_t *layout);
spec_error_t spec_ndsi_check_storage(const spec_ndsi_save_t *save);
void spec_ndsi_encode_storage(uint8_t *copy, const spec_ndsi_layout_t *layout,
                              const spec_ndsi_save_t *save);

// Records are encrypted, as stored.
void spec_ndsi_decode_pokemon(spec_ndsi_pokemon_t *pokemon, const uint8_t *record,
                              size_t record_size);
spec_error_t spec_ndsi_encode_pokemon(uint8_t *record, size_t record_size,
                                      const spec_ndsi_pokemon_t *pokemon);
void spec_ndsi_fill_party_data(spec_ndsi_pokemon_t *pokemon);

void spec_ndsi_decode_mail(spec_ndsi_mail_t *mail, const uint8_t *bytes);
void spec_ndsi_encode_mail(uint8_t *bytes, const spec_ndsi_mail_t *mail);
spec_ndsi_mail_t spec_ndsi_no_mail(void);

spec_error_t spec_ndsi_check_item_placement(spec_game_type_t type, spec_ndsi_pocket_t pocket,
                                            uint16_t item);
void spec_ndsi_decode_items(spec_ndsi_save_t *save, const uint8_t *copy,
                            const spec_ndsi_layout_t *layout);
spec_error_t spec_ndsi_check_items(const spec_ndsi_save_t *save);
void spec_ndsi_encode_items(uint8_t *copy, const spec_ndsi_layout_t *layout,
                            const spec_ndsi_save_t *save);

#endif
