// Reading and writing a whole Gen 5 save: detection, then each part's codec in turn.

#include "ndsi/ndsi.h"
#include "ndsi/ndsi_internal.h"
#include "spec_internal.h"

// Each pair's footer has its own place and size, so only one pair's can match.
constexpr spec_game_type_t TYPES[] = {
    SPEC_GAME_TYPE_BLACK_WHITE,
    SPEC_GAME_TYPE_BLACK2_WHITE2,
};

static spec_error_t check_parts(const spec_ndsi_save_t *save, const spec_ndsi_layout_t *layout) {
    spec_error_t error = spec_ndsi_check_player(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    error = spec_ndsi_check_pokedex(&save->pokedex);
    if (error != SPEC_OK) {
        return error;
    }
    error = spec_ndsi_check_storage(save);
    if (error != SPEC_OK) {
        return error;
    }
    return spec_ndsi_check_items(save);
}

spec_error_t spec_ndsi_read_save(spec_ndsi_save_t *save,
                                 const uint8_t data[static SPEC_NDSI_SAVE_SIZE]) {
    for (size_t index = 0; index < sizeof TYPES / sizeof TYPES[0]; ++index) {
        const spec_ndsi_layout_t *layout = spec_ndsi_get_layout(TYPES[index]);
        size_t loaded_offset = 0;
        if (!spec_ndsi_find_loaded_copy(&loaded_offset, data, layout)) {
            continue;
        }
        const uint8_t *copy = &data[loaded_offset];
        *save = (spec_ndsi_save_t){.type = layout->type};
        spec_ndsi_decode_player(save, copy, layout);
        spec_ndsi_decode_pokedex(&save->pokedex, copy, layout);
        spec_ndsi_decode_storage(save, copy, layout);
        spec_ndsi_decode_items(save, copy, layout);
        return SPEC_OK;
    }
    return spec_fail(SPEC_ERROR_INVALID_SAVE, "no Gen 5 game would load this save");
}

// Everything is checked before the save changes, so a failure changes nothing.
spec_error_t spec_ndsi_write_save(const spec_ndsi_save_t *save,
                                  uint8_t data[static SPEC_NDSI_SAVE_SIZE]) {
    const spec_ndsi_layout_t *layout = spec_ndsi_get_layout(save->type);
    if (layout == nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "type is not an NDSi game type");
    }
    size_t loaded_offset = 0;
    if (!spec_ndsi_find_loaded_copy(&loaded_offset, data, layout)) {
        return spec_fail(SPEC_ERROR_INVALID_SAVE, "the save's game would not load it");
    }
    spec_error_t error = check_parts(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    size_t next_offset = spec_ndsi_copy_to_other(data, loaded_offset, layout);
    uint8_t *copy = &data[next_offset];
    spec_ndsi_encode_player(copy, layout, save);
    spec_ndsi_encode_pokedex(copy, layout, &save->pokedex);
    spec_ndsi_encode_storage(copy, layout, save);
    spec_ndsi_encode_items(copy, layout, save);
    spec_ndsi_stamp_copy(data, next_offset, loaded_offset, layout);
    spec_ndsi_mirror_copy(data, next_offset, loaded_offset, layout);
    return SPEC_OK;
}

spec_error_t spec_ndsi_check_save(const spec_ndsi_save_t *save) {
    const spec_ndsi_layout_t *layout = spec_ndsi_get_layout(save->type);
    if (layout == nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "type is not an NDSi game type");
    }
    return check_parts(save, layout);
}
