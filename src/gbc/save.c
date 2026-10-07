// Reading and writing a whole Gen 2 save: the copy the game loads, then each part's codec in
// turn.

#include <string.h>

#include "gbc/gbc.h"
#include "gbc/gbc_internal.h"
#include "spec_internal.h"

static spec_error_t find_layout(const spec_gbc_layout_t **layout, spec_game_type_t type,
                                spec_language_t language) {
    *layout = spec_gbc_get_layout(type, language);
    if (*layout == nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                         "no Gen 2 game of that type is in that language");
    }
    return SPEC_OK;
}

// Emulators and cart dumpers such as FlashGBX keep the cartridge clock's registers and a 32- or
// 64-bit timestamp after the save.
static bool is_save_size(size_t data_size, const spec_gbc_layout_t *layout) {
    constexpr size_t CLOCK_DATA_SIZES[] = {0, 44, 48};
    for (size_t index = 0; index < sizeof CLOCK_DATA_SIZES / sizeof CLOCK_DATA_SIZES[0]; ++index) {
        if (data_size == layout->save_size + CLOCK_DATA_SIZES[index]) {
            return true;
        }
    }
    return false;
}

static spec_error_t find_sized_layout(const spec_gbc_layout_t **layout, spec_game_type_t type,
                                      spec_language_t language, size_t data_size) {
    spec_error_t error = find_layout(layout, type, language);
    if (error != SPEC_OK) {
        return error;
    }
    if (!is_save_size(data_size, *layout)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "data_size is not this game's save size");
    }
    return SPEC_OK;
}

static spec_error_t find_loaded_copy(bool *is_primary, const uint8_t *data,
                                     const spec_gbc_layout_t *layout) {
    if (!spec_gbc_has_save(data, layout)) {
        return spec_fail(SPEC_ERROR_INVALID_SAVE, "neither copy's check values mark a save");
    }
    *is_primary = spec_gbc_is_primary_valid(data, layout);
    if (!*is_primary && !spec_gbc_is_backup_valid(data, layout)) {
        return spec_fail(SPEC_ERROR_INVALID_SAVE, "neither copy's checksum matches");
    }
    return SPEC_OK;
}

static spec_error_t check_parts(const spec_gbc_save_t *save, const spec_gbc_layout_t *layout) {
    spec_error_t error = spec_gbc_check_player(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    error = spec_gbc_check_pokedex(&save->pokedex);
    if (error != SPEC_OK) {
        return error;
    }
    error = spec_gbc_check_storage(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    return spec_gbc_check_items(save);
}

// As TryLoadSaveFile: the primary, else the backup, which the game then saves over the primary.
spec_error_t spec_gbc_read_save(spec_gbc_save_t *save, const uint8_t *data, size_t data_size,
                                spec_game_type_t type, spec_language_t language) {
    const spec_gbc_layout_t *layout = nullptr;
    spec_error_t error = find_sized_layout(&layout, type, language, data_size);
    if (error != SPEC_OK) {
        return error;
    }
    bool is_primary = false;
    error = find_loaded_copy(&is_primary, data, layout);
    if (error != SPEC_OK) {
        return error;
    }
    uint8_t loaded[SPEC_GBC_JAPANESE_CRYSTAL_SAVE_SIZE];
    memcpy(loaded, data, layout->save_size);
    if (!is_primary) {
        spec_gbc_restore_primary(loaded, layout);
    }
    *save = (spec_gbc_save_t){.type = type, .language = language};
    spec_gbc_decode_player(save, loaded, layout);
    spec_gbc_decode_pokedex(&save->pokedex, loaded, layout);
    spec_gbc_decode_storage(save, loaded, layout);
    spec_gbc_decode_party_mail(save, loaded, layout);
    spec_gbc_decode_items(save, loaded, layout);
    return SPEC_OK;
}

// As _SaveGameData. Everything is checked before the save changes, so a failure changes nothing.
spec_error_t spec_gbc_write_save(const spec_gbc_save_t *save, uint8_t *data, size_t data_size) {
    const spec_gbc_layout_t *layout = nullptr;
    spec_error_t error = find_sized_layout(&layout, save->type, save->language, data_size);
    if (error != SPEC_OK) {
        return error;
    }
    bool is_primary = false;
    error = find_loaded_copy(&is_primary, data, layout);
    if (error != SPEC_OK) {
        return error;
    }
    error = check_parts(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    if (!is_primary) {
        spec_gbc_restore_primary(data, layout);
    }
    spec_gbc_encode_player(data, layout, save);
    spec_gbc_encode_pokedex(data, layout, &save->pokedex);
    spec_gbc_encode_storage(data, layout, save);
    spec_gbc_encode_party_mail(data, layout, save);
    spec_gbc_encode_items(data, layout, save);
    spec_gbc_stamp_copies(data, layout);
    return SPEC_OK;
}

spec_error_t spec_gbc_check_save(const spec_gbc_save_t *save) {
    const spec_gbc_layout_t *layout = nullptr;
    spec_error_t error = find_layout(&layout, save->type, save->language);
    if (error != SPEC_OK) {
        return error;
    }
    return check_parts(save, layout);
}
