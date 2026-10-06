// Reading and writing a whole Gen 6 or 7 save: detection, each part's codec, then the footer.

#include "3ds/3ds.h"
#include "3ds/3ds_internal.h"
#include "spec_internal.h"

static spec_error_t check_parts(const spec_3ds_save_t *save, const spec_3ds_layout_t *layout) {
    spec_error_t error = spec_3ds_check_player(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    error = spec_3ds_check_pokedex(&save->pokedex, layout);
    if (error != SPEC_OK) {
        return error;
    }
    error = spec_3ds_check_storage(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    return spec_3ds_check_items(save, layout);
}

// A new game's file, which the game writes before its first save, has no footer to match.
spec_error_t spec_3ds_read_save(spec_3ds_save_t *save, const uint8_t *data, size_t data_size) {
    const spec_3ds_layout_t *layout = spec_3ds_find_layout(data_size);
    if (layout == nullptr) {
        return spec_fail(SPEC_ERROR_INVALID_SAVE, "data_size is no Gen 6 or 7 save's size");
    }
    if (!spec_3ds_is_footer_valid(data, layout)) {
        return spec_fail(SPEC_ERROR_INVALID_SAVE, "the save's game would not load it");
    }
    *save = (spec_3ds_save_t){.type = layout->type};
    spec_3ds_decode_player(save, data, layout);
    spec_3ds_decode_pokedex(&save->pokedex, data, layout);
    spec_3ds_decode_storage(save, data, layout);
    spec_3ds_decode_items(save, data, layout);
    return SPEC_OK;
}

// Everything is checked before the save changes, so a failure changes nothing. The game keeps
// one copy, so the write is in place; the footer's two timers stay as they are.
spec_error_t spec_3ds_write_save(const spec_3ds_save_t *save, uint8_t *data, size_t data_size) {
    const spec_3ds_layout_t *layout = spec_3ds_get_layout(save->type);
    if (layout == nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "type is not a 3DS game type");
    }
    if (data_size != layout->save_size || !spec_3ds_is_footer_valid(data, layout)) {
        return spec_fail(SPEC_ERROR_INVALID_SAVE, "the save's game would not load it");
    }
    spec_error_t error = check_parts(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    spec_3ds_encode_player(data, layout, save);
    spec_3ds_encode_pokedex(data, layout, &save->pokedex);
    spec_3ds_encode_storage(data, layout, save);
    spec_3ds_encode_items(data, layout, save);
    spec_3ds_stamp_footer(data, layout);
    if (layout->is_gen7) {
        spec_3ds_sign_save(data, layout);
    }
    return SPEC_OK;
}

spec_error_t spec_3ds_check_save(const spec_3ds_save_t *save) {
    const spec_3ds_layout_t *layout = spec_3ds_get_layout(save->type);
    if (layout == nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "type is not a 3DS game type");
    }
    return check_parts(save, layout);
}

size_t spec_3ds_save_size(spec_game_type_t type) {
    const spec_3ds_layout_t *layout = spec_3ds_get_layout(type);
    return layout == nullptr ? 0 : layout->save_size;
}
