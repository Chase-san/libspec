// Reading and writing a whole Gen 4 save: detection, then each part's codec in turn.

#include "nds/nds.h"
#include "nds/nds_internal.h"
#include "spec_internal.h"

// Each game's blocks have their own sizes, so only one game's footers can match.
constexpr spec_game_type_t TYPES[] = {
    SPEC_GAME_TYPE_DIAMOND_PEARL,
    SPEC_GAME_TYPE_PLATINUM,
    SPEC_GAME_TYPE_HEARTGOLD_SOULSILVER,
};

static spec_error_t check_parts(const spec_nds_save_t *save, const spec_nds_layout_t *layout) {
    spec_error_t error = spec_nds_check_player(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    error = spec_nds_check_pokedex(&save->pokedex, layout);
    if (error != SPEC_OK) {
        return error;
    }
    error = spec_nds_check_storage(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    return spec_nds_check_items(save, layout);
}

spec_error_t spec_nds_read_save(spec_nds_save_t *save,
                                const uint8_t data[static SPEC_NDS_SAVE_SIZE]) {
    for (size_t index = 0; index < sizeof TYPES / sizeof TYPES[0]; ++index) {
        const spec_nds_layout_t *layout = spec_nds_get_layout(TYPES[index]);
        spec_nds_block_pair_t loaded;
        if (!spec_nds_find_loaded_blocks(&loaded, data, layout)) {
            continue;
        }
        const uint8_t *general = &data[loaded.general_offset];
        const uint8_t *storage = &data[loaded.storage_offset];
        *save = (spec_nds_save_t){.type = layout->type};
        spec_nds_decode_player(save, general, layout);
        spec_nds_decode_pokedex(&save->pokedex, general, layout);
        spec_nds_decode_storage(save, general, storage, layout);
        spec_nds_decode_items(save, general, layout);
        return SPEC_OK;
    }
    return spec_fail(SPEC_ERROR_INVALID_SAVE, "no Gen 4 game would load this save");
}

// Everything is checked before the save changes, so a failure changes nothing.
spec_error_t spec_nds_write_save(const spec_nds_save_t *save,
                                 uint8_t data[static SPEC_NDS_SAVE_SIZE]) {
    const spec_nds_layout_t *layout = spec_nds_get_layout(save->type);
    if (layout == nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "type is not an NDS game type");
    }
    spec_nds_block_pair_t loaded;
    if (!spec_nds_find_loaded_blocks(&loaded, data, layout)) {
        return spec_fail(SPEC_ERROR_INVALID_SAVE, "the save's game would not load it");
    }
    spec_error_t error = check_parts(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    spec_nds_block_pair_t next = spec_nds_copy_to_next_blocks(data, &loaded, layout);
    uint8_t *general = &data[next.general_offset];
    uint8_t *storage = &data[next.storage_offset];
    spec_nds_encode_player(general, layout, save);
    spec_nds_encode_pokedex(general, layout, &save->pokedex);
    spec_nds_encode_storage(general, storage, layout, save);
    spec_nds_encode_items(general, layout, save);
    if (layout->has_box_modified_flags) {
        spec_nds_flag_changed_boxes(storage, &data[loaded.storage_offset], layout);
    }
    spec_nds_stamp_blocks(data, &next, layout);
    return SPEC_OK;
}

spec_error_t spec_nds_check_save(const spec_nds_save_t *save) {
    const spec_nds_layout_t *layout = spec_nds_get_layout(save->type);
    if (layout == nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "type is not an NDS game type");
    }
    return check_parts(save, layout);
}
