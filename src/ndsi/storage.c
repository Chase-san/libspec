// Gen 5 Pokémon storage: the party, the PC boxes and the daycare.

#include <string.h>

#include "nds/nds_internal.h"
#include "ndsi/ndsi.h"
#include "ndsi/ndsi_internal.h"
#include "spec_internal.h"

// The byte after the records does not follow the party count, so it stays as the game wrote it.
constexpr size_t PARTY_COUNT_OFFSET = 0x4;
constexpr size_t PARTY_RECORDS_OFFSET = 0x8;

constexpr size_t CURRENT_BOX_OFFSET = 0x0;
constexpr size_t BOX_NAMES_OFFSET = 0x4;
constexpr size_t BOX_NAME_FIELD_SIZE = SPEC_NDSI_BOX_NAME_SIZE * 2;
constexpr size_t WALLPAPERS_OFFSET = 0x3C4;
constexpr size_t BOX_STRIDE = 0x1000;
// Sixteen plain wallpapers, then eight special ones.
constexpr uint8_t WALLPAPER_COUNT = 24;

constexpr size_t DAYCARE_SLOT_SIZE = 0xE4;
constexpr size_t DAYCARE_RECORD_OFFSET = 0x4;
constexpr size_t DAYCARE_STEPS_OFFSET = 0xE0;
constexpr size_t DAYCARE_EGG_WAITING_OFFSET = 0x1C8;
constexpr uint32_t DAYCARE_SLOT_OCCUPIED = 1;

static size_t party_record_offset(const spec_ndsi_layout_t *layout, size_t index) {
    return layout->party_offset + PARTY_RECORDS_OFFSET + index * SPEC_NDSI_PARTY_RECORD_SIZE;
}

static size_t box_record_offset(const spec_ndsi_layout_t *layout, size_t box, size_t index) {
    return layout->first_box_offset + box * BOX_STRIDE + index * SPEC_NDSI_BOX_RECORD_SIZE;
}

static size_t daycare_slot_offset(const spec_ndsi_layout_t *layout, size_t index) {
    return layout->daycare_offset + index * DAYCARE_SLOT_SIZE;
}

static spec_error_t check_pokemon(const spec_ndsi_pokemon_t *pokemon, size_t record_size) {
    uint8_t record[SPEC_NDSI_PARTY_RECORD_SIZE];
    return spec_ndsi_encode_pokemon(record, record_size, pokemon);
}

// Encoding only fails on what check_storage has already refused.
static void write_pokemon(uint8_t *record, size_t record_size, const spec_ndsi_pokemon_t *pokemon) {
    (void)spec_ndsi_encode_pokemon(record, record_size, pokemon);
}

static spec_ndsi_pokemon_t with_party_data(const spec_ndsi_pokemon_t *pokemon) {
    spec_ndsi_pokemon_t party_pokemon = *pokemon;
    spec_ndsi_fill_party_data(&party_pokemon);
    return party_pokemon;
}

static void decode_party(spec_ndsi_save_t *save, const uint8_t *copy,
                         const spec_ndsi_layout_t *layout) {
    save->party_count = (uint8_t)spec_read_u32_le(&copy[layout->party_offset + PARTY_COUNT_OFFSET]);
    for (size_t index = 0; index < SPEC_NDSI_PARTY_CAPACITY; ++index) {
        spec_ndsi_decode_pokemon(&save->party[index], &copy[party_record_offset(layout, index)],
                                 SPEC_NDSI_PARTY_RECORD_SIZE);
        spec_ndsi_fill_party_data(&save->party[index]);
    }
}

static spec_error_t check_party(const spec_ndsi_save_t *save) {
    if (save->party_count > SPEC_NDSI_PARTY_CAPACITY) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "party_count is beyond the party");
    }
    for (size_t index = 0; index < SPEC_NDSI_PARTY_CAPACITY; ++index) {
        spec_ndsi_pokemon_t party_pokemon = with_party_data(&save->party[index]);
        if (check_pokemon(&party_pokemon, SPEC_NDSI_PARTY_RECORD_SIZE) != SPEC_OK) {
            return spec_locate_error(SPEC_ERROR_LOCATION_PARTY, (uint32_t)index, 0);
        }
    }
    return SPEC_OK;
}

static void encode_party(uint8_t *copy, const spec_ndsi_layout_t *layout,
                         const spec_ndsi_save_t *save) {
    spec_write_u32_le(&copy[layout->party_offset + PARTY_COUNT_OFFSET], save->party_count);
    for (size_t index = 0; index < SPEC_NDSI_PARTY_CAPACITY; ++index) {
        spec_ndsi_pokemon_t party_pokemon = with_party_data(&save->party[index]);
        write_pokemon(&copy[party_record_offset(layout, index)], SPEC_NDSI_PARTY_RECORD_SIZE,
                      &party_pokemon);
    }
}

static void decode_boxes(spec_ndsi_save_t *save, const uint8_t *copy,
                         const spec_ndsi_layout_t *layout) {
    const uint8_t *box_info = &copy[layout->box_info_offset];
    save->current_box = box_info[CURRENT_BOX_OFFSET];
    for (size_t box = 0; box < SPEC_NDSI_BOX_COUNT; ++box) {
        spec_ndsi_box_t *pc_box = &save->boxes[box];
        spec_nds_read_text(pc_box->name, &box_info[BOX_NAMES_OFFSET + box * BOX_NAME_FIELD_SIZE],
                           SPEC_NDSI_BOX_NAME_SIZE);
        pc_box->wallpaper = box_info[WALLPAPERS_OFFSET + box];
        for (size_t index = 0; index < SPEC_NDSI_BOX_CAPACITY; ++index) {
            spec_ndsi_decode_pokemon(&pc_box->pokemon[index],
                                     &copy[box_record_offset(layout, box, index)],
                                     SPEC_NDSI_BOX_RECORD_SIZE);
        }
    }
}

static spec_error_t check_boxes(const spec_ndsi_save_t *save) {
    if (save->current_box >= SPEC_NDSI_BOX_COUNT) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "current_box is beyond the last box");
    }
    for (size_t box = 0; box < SPEC_NDSI_BOX_COUNT; ++box) {
        if (save->boxes[box].wallpaper >= WALLPAPER_COUNT) {
            (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                            "the wallpaper is beyond the game's wallpapers");
            return spec_locate_error(SPEC_ERROR_LOCATION_WALLPAPER, (uint32_t)box, 0);
        }
        for (size_t index = 0; index < SPEC_NDSI_BOX_CAPACITY; ++index) {
            if (check_pokemon(&save->boxes[box].pokemon[index], SPEC_NDSI_BOX_RECORD_SIZE)
                != SPEC_OK) {
                return spec_locate_error(SPEC_ERROR_LOCATION_BOX, (uint32_t)box, (uint32_t)index);
            }
        }
    }
    return SPEC_OK;
}

static void encode_boxes(uint8_t *copy, const spec_ndsi_layout_t *layout,
                         const spec_ndsi_save_t *save) {
    uint8_t *box_info = &copy[layout->box_info_offset];
    box_info[CURRENT_BOX_OFFSET] = save->current_box;
    for (size_t box = 0; box < SPEC_NDSI_BOX_COUNT; ++box) {
        const spec_ndsi_box_t *pc_box = &save->boxes[box];
        spec_nds_write_text(&box_info[BOX_NAMES_OFFSET + box * BOX_NAME_FIELD_SIZE], pc_box->name,
                            SPEC_NDSI_BOX_NAME_SIZE);
        box_info[WALLPAPERS_OFFSET + box] = pc_box->wallpaper;
        for (size_t index = 0; index < SPEC_NDSI_BOX_CAPACITY; ++index) {
            write_pokemon(&copy[box_record_offset(layout, box, index)], SPEC_NDSI_BOX_RECORD_SIZE,
                          &pc_box->pokemon[index]);
        }
    }
}

// A slot whose occupied word is clear is empty, whatever its record still holds.
static void decode_daycare(spec_ndsi_daycare_t *daycare, const uint8_t *copy,
                           const spec_ndsi_layout_t *layout) {
    for (size_t index = 0; index < SPEC_NDSI_DAYCARE_CAPACITY; ++index) {
        const uint8_t *slot = &copy[daycare_slot_offset(layout, index)];
        daycare->slots[index] = (spec_ndsi_daycare_slot_t){};
        if (spec_read_u32_le(slot) == 0) {
            continue;
        }
        spec_ndsi_decode_pokemon(&daycare->slots[index].pokemon, &slot[DAYCARE_RECORD_OFFSET],
                                 SPEC_NDSI_PARTY_RECORD_SIZE);
        daycare->slots[index].steps = spec_read_u32_le(&slot[DAYCARE_STEPS_OFFSET]);
    }
    daycare->is_egg_waiting =
        spec_read_u32_le(&copy[layout->daycare_offset + DAYCARE_EGG_WAITING_OFFSET]) != 0;
}

static spec_error_t check_daycare(const spec_ndsi_daycare_t *daycare) {
    for (size_t index = 0; index < SPEC_NDSI_DAYCARE_CAPACITY; ++index) {
        spec_ndsi_pokemon_t daycare_pokemon = with_party_data(&daycare->slots[index].pokemon);
        if (check_pokemon(&daycare_pokemon, SPEC_NDSI_PARTY_RECORD_SIZE) != SPEC_OK) {
            return spec_locate_error(SPEC_ERROR_LOCATION_DAYCARE, (uint32_t)index, 0);
        }
    }
    return SPEC_OK;
}

// An empty slot is all zero, as the game leaves it.
static void encode_daycare(uint8_t *copy, const spec_ndsi_layout_t *layout,
                           const spec_ndsi_daycare_t *daycare) {
    for (size_t index = 0; index < SPEC_NDSI_DAYCARE_CAPACITY; ++index) {
        uint8_t *slot = &copy[daycare_slot_offset(layout, index)];
        const spec_ndsi_daycare_slot_t *daycare_slot = &daycare->slots[index];
        memset(slot, 0, DAYCARE_SLOT_SIZE);
        if (daycare_slot->pokemon.species == 0) {
            continue;
        }
        spec_ndsi_pokemon_t daycare_pokemon = with_party_data(&daycare_slot->pokemon);
        spec_write_u32_le(slot, DAYCARE_SLOT_OCCUPIED);
        write_pokemon(&slot[DAYCARE_RECORD_OFFSET], SPEC_NDSI_PARTY_RECORD_SIZE, &daycare_pokemon);
        spec_write_u32_le(&slot[DAYCARE_STEPS_OFFSET], daycare_slot->steps);
    }
    spec_write_u32_le(&copy[layout->daycare_offset + DAYCARE_EGG_WAITING_OFFSET],
                      daycare->is_egg_waiting ? 1 : 0);
}

void spec_ndsi_decode_storage(spec_ndsi_save_t *save, const uint8_t *copy,
                              const spec_ndsi_layout_t *layout) {
    decode_party(save, copy, layout);
    decode_boxes(save, copy, layout);
    decode_daycare(&save->daycare, copy, layout);
}

spec_error_t spec_ndsi_check_storage(const spec_ndsi_save_t *save) {
    spec_error_t error = check_party(save);
    if (error != SPEC_OK) {
        return error;
    }
    error = check_boxes(save);
    if (error != SPEC_OK) {
        return error;
    }
    return check_daycare(&save->daycare);
}

void spec_ndsi_encode_storage(uint8_t *copy, const spec_ndsi_layout_t *layout,
                              const spec_ndsi_save_t *save) {
    encode_party(copy, layout, save);
    encode_boxes(copy, layout, save);
    encode_daycare(copy, layout, &save->daycare);
}
