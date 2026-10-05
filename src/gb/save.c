// Reading and writing a whole Gen 1 save: the checksum, then each part's codec in turn.

#include "gb/gb.h"
#include "gb/gb_internal.h"
#include "spec_internal.h"

static bool is_gen1_game(spec_game_type_t type) {
    return type == SPEC_GAME_TYPE_RED_BLUE || type == SPEC_GAME_TYPE_YELLOW;
}

static spec_error_t find_layout(const spec_gb_layout_t **layout, spec_game_type_t type,
                                spec_language_t language) {
    if (!is_gen1_game(type)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "type is not a Gen 1 game type");
    }
    *layout = spec_gb_get_layout(language);
    if (*layout == nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "no Gen 1 game is in that language");
    }
    return SPEC_OK;
}

static spec_error_t check_parts(const spec_gb_save_t *save, const spec_gb_layout_t *layout) {
    spec_error_t error = spec_gb_check_player(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    error = spec_gb_check_storage(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    return spec_gb_check_items(save);
}

// As LoadMainData: there is no backup, so a bad checksum is the end of the save.
spec_error_t spec_gb_read_save(spec_gb_save_t *save, const uint8_t data[static SPEC_GB_SAVE_SIZE],
                               spec_game_type_t type, spec_language_t language) {
    const spec_gb_layout_t *layout = nullptr;
    spec_error_t error = find_layout(&layout, type, language);
    if (error != SPEC_OK) {
        return error;
    }
    if (!spec_gb_is_game_data_valid(data, layout)) {
        return spec_fail(SPEC_ERROR_INVALID_SAVE,
                         "the game data's checksum does not match for that language");
    }
    *save = (spec_gb_save_t){.type = type, .language = language};
    spec_gb_decode_player(save, data, layout);
    spec_gb_decode_pokedex(&save->pokedex, data, layout);
    spec_gb_decode_storage(save, data, layout);
    spec_gb_decode_items(save, data, layout);
    return SPEC_OK;
}

// Everything is checked before the save changes, so a failure changes nothing.
spec_error_t spec_gb_write_save(const spec_gb_save_t *save,
                                uint8_t data[static SPEC_GB_SAVE_SIZE]) {
    const spec_gb_layout_t *layout = nullptr;
    spec_error_t error = find_layout(&layout, save->type, save->language);
    if (error != SPEC_OK) {
        return error;
    }
    if (!spec_gb_is_game_data_valid(data, layout)) {
        return spec_fail(SPEC_ERROR_INVALID_SAVE,
                         "the game data's checksum does not match for the save's language");
    }
    error = check_parts(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    spec_gb_encode_player(data, layout, save);
    spec_gb_encode_pokedex(data, layout, &save->pokedex);
    spec_gb_encode_storage(data, layout, save);
    spec_gb_encode_items(data, layout, save);
    spec_gb_stamp_game_data(data, layout);
    return SPEC_OK;
}

spec_error_t spec_gb_check_save(const spec_gb_save_t *save) {
    const spec_gb_layout_t *layout = nullptr;
    spec_error_t error = find_layout(&layout, save->type, save->language);
    if (error != SPEC_OK) {
        return error;
    }
    return check_parts(save, layout);
}
