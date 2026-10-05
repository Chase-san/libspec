// Gen 4 Pokémon storage: the party, the PC boxes and the daycare.

#include <string.h>

#include "nds/nds.h"
#include "nds/nds_internal.h"
#include "spec_internal.h"

constexpr size_t PARTY_COUNT_OFFSET = 0x4;
constexpr size_t PARTY_RECORDS_OFFSET = 0x8;
constexpr size_t BOX_NAME_FIELD_SIZE = SPEC_NDS_BOX_NAME_SIZE * 2;

constexpr size_t DAYCARE_SLOT_SIZE = 0xEC;
constexpr size_t DAYCARE_STEPS_OFFSET = 0xE8;
constexpr size_t DAYCARE_EGG_PERSONALITY_OFFSET = 0x1D8;
constexpr size_t DAYCARE_STEP_COUNTER_OFFSET = 0x1DC;

static size_t party_record_offset(const spec_nds_layout_t *layout, size_t index) {
    return layout->party_offset + PARTY_RECORDS_OFFSET + index * SPEC_NDS_PARTY_RECORD_SIZE;
}

static size_t box_offset(const spec_nds_layout_t *layout, size_t box) {
    return layout->box_records_offset + box * layout->box_stride;
}

static size_t box_record_offset(const spec_nds_layout_t *layout, size_t box, size_t index) {
    return box_offset(layout, box) + index * SPEC_NDS_BOX_RECORD_SIZE;
}

static size_t daycare_slot_offset(const spec_nds_layout_t *layout, size_t index) {
    return layout->daycare_offset + index * DAYCARE_SLOT_SIZE;
}

static spec_error_t check_pokemon(const spec_nds_pokemon_t *pokemon, size_t record_size) {
    uint8_t record[SPEC_NDS_PARTY_RECORD_SIZE];
    return spec_nds_encode_pokemon(record, record_size, pokemon);
}

// Encoding only fails on what check_storage has already refused.
static void write_pokemon(uint8_t *record, size_t record_size, const spec_nds_pokemon_t *pokemon) {
    (void)spec_nds_encode_pokemon(record, record_size, pokemon);
}

static spec_nds_pokemon_t with_party_data(const spec_nds_pokemon_t *pokemon) {
    spec_nds_pokemon_t party_pokemon = *pokemon;
    spec_nds_fill_party_data(&party_pokemon);
    return party_pokemon;
}

static void decode_party(spec_nds_save_t *save, const uint8_t *general,
                         const spec_nds_layout_t *layout) {
    save->party_count =
        (uint8_t)spec_read_u32_le(&general[layout->party_offset + PARTY_COUNT_OFFSET]);
    for (size_t index = 0; index < SPEC_NDS_PARTY_CAPACITY; ++index) {
        spec_nds_decode_pokemon(&save->party[index], &general[party_record_offset(layout, index)],
                                SPEC_NDS_PARTY_RECORD_SIZE);
        spec_nds_fill_party_data(&save->party[index]);
    }
}

static void decode_boxes(spec_nds_save_t *save, const uint8_t *storage,
                         const spec_nds_layout_t *layout) {
    save->current_box = (uint8_t)spec_read_u32_le(&storage[layout->current_box_offset]);
    for (size_t box = 0; box < SPEC_NDS_BOX_COUNT; ++box) {
        spec_nds_box_t *pc_box = &save->boxes[box];
        spec_nds_read_text(pc_box->name,
                           &storage[layout->box_names_offset + box * BOX_NAME_FIELD_SIZE],
                           SPEC_NDS_BOX_NAME_SIZE);
        pc_box->wallpaper = storage[layout->wallpapers_offset + box];
        for (size_t index = 0; index < SPEC_NDS_BOX_CAPACITY; ++index) {
            spec_nds_decode_pokemon(&pc_box->pokemon[index],
                                    &storage[box_record_offset(layout, box, index)],
                                    SPEC_NDS_BOX_RECORD_SIZE);
        }
    }
}

// Each slot's mail stays as the game wrote it.
static void decode_daycare(spec_nds_daycare_t *daycare, const uint8_t *general,
                           const spec_nds_layout_t *layout) {
    for (size_t index = 0; index < SPEC_NDS_DAYCARE_CAPACITY; ++index) {
        const uint8_t *slot = &general[daycare_slot_offset(layout, index)];
        spec_nds_decode_pokemon(&daycare->slots[index].pokemon, slot, SPEC_NDS_BOX_RECORD_SIZE);
        daycare->slots[index].steps = spec_read_u32_le(&slot[DAYCARE_STEPS_OFFSET]);
    }
    const uint8_t *daycare_bytes = &general[layout->daycare_offset];
    daycare->egg_personality = spec_read_u32_le(&daycare_bytes[DAYCARE_EGG_PERSONALITY_OFFSET]);
    daycare->step_counter = daycare_bytes[DAYCARE_STEP_COUNTER_OFFSET];
}

void spec_nds_decode_storage(spec_nds_save_t *save, const uint8_t *general, const uint8_t *storage,
                             const spec_nds_layout_t *layout) {
    decode_party(save, general, layout);
    decode_boxes(save, storage, layout);
    decode_daycare(&save->daycare, general, layout);
}

static void encode_party(uint8_t *general, const spec_nds_layout_t *layout,
                         const spec_nds_save_t *save) {
    spec_write_u32_le(&general[layout->party_offset + PARTY_COUNT_OFFSET], save->party_count);
    for (size_t index = 0; index < SPEC_NDS_PARTY_CAPACITY; ++index) {
        spec_nds_pokemon_t party_pokemon = with_party_data(&save->party[index]);
        write_pokemon(&general[party_record_offset(layout, index)], SPEC_NDS_PARTY_RECORD_SIZE,
                      &party_pokemon);
    }
}

static void encode_boxes(uint8_t *storage, const spec_nds_layout_t *layout,
                         const spec_nds_save_t *save) {
    spec_write_u32_le(&storage[layout->current_box_offset], save->current_box);
    for (size_t box = 0; box < SPEC_NDS_BOX_COUNT; ++box) {
        const spec_nds_box_t *pc_box = &save->boxes[box];
        spec_nds_write_text(&storage[layout->box_names_offset + box * BOX_NAME_FIELD_SIZE],
                            pc_box->name, SPEC_NDS_BOX_NAME_SIZE);
        storage[layout->wallpapers_offset + box] = pc_box->wallpaper;
        for (size_t index = 0; index < SPEC_NDS_BOX_CAPACITY; ++index) {
            write_pokemon(&storage[box_record_offset(layout, box, index)], SPEC_NDS_BOX_RECORD_SIZE,
                          &pc_box->pokemon[index]);
        }
    }
}

static void encode_daycare(uint8_t *general, const spec_nds_layout_t *layout,
                           const spec_nds_daycare_t *daycare) {
    for (size_t index = 0; index < SPEC_NDS_DAYCARE_CAPACITY; ++index) {
        uint8_t *slot = &general[daycare_slot_offset(layout, index)];
        write_pokemon(slot, SPEC_NDS_BOX_RECORD_SIZE, &daycare->slots[index].pokemon);
        spec_write_u32_le(&slot[DAYCARE_STEPS_OFFSET], daycare->slots[index].steps);
    }
    uint8_t *daycare_bytes = &general[layout->daycare_offset];
    spec_write_u32_le(&daycare_bytes[DAYCARE_EGG_PERSONALITY_OFFSET], daycare->egg_personality);
    daycare_bytes[DAYCARE_STEP_COUNTER_OFFSET] = daycare->step_counter;
}

void spec_nds_encode_storage(uint8_t *general, uint8_t *storage, const spec_nds_layout_t *layout,
                             const spec_nds_save_t *save) {
    encode_party(general, layout, save);
    encode_boxes(storage, layout, save);
    encode_daycare(general, layout, &save->daycare);
}

static spec_error_t check_party(const spec_nds_save_t *save) {
    if (save->party_count > SPEC_NDS_PARTY_CAPACITY) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "party_count is beyond the party");
    }
    for (size_t index = 0; index < SPEC_NDS_PARTY_CAPACITY; ++index) {
        spec_nds_pokemon_t party_pokemon = with_party_data(&save->party[index]);
        if (check_pokemon(&party_pokemon, SPEC_NDS_PARTY_RECORD_SIZE) != SPEC_OK) {
            return spec_locate_error(SPEC_ERROR_LOCATION_PARTY, (uint32_t)index, 0);
        }
    }
    return SPEC_OK;
}

static spec_error_t check_boxes(const spec_nds_save_t *save, const spec_nds_layout_t *layout) {
    if (save->current_box >= SPEC_NDS_BOX_COUNT) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "current_box is beyond the last box");
    }
    for (size_t box = 0; box < SPEC_NDS_BOX_COUNT; ++box) {
        if (save->boxes[box].wallpaper >= layout->wallpaper_count) {
            (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                            "the wallpaper is beyond this game's wallpapers");
            return spec_locate_error(SPEC_ERROR_LOCATION_WALLPAPER, (uint32_t)box, 0);
        }
        for (size_t index = 0; index < SPEC_NDS_BOX_CAPACITY; ++index) {
            if (check_pokemon(&save->boxes[box].pokemon[index], SPEC_NDS_BOX_RECORD_SIZE)
                != SPEC_OK) {
                return spec_locate_error(SPEC_ERROR_LOCATION_BOX, (uint32_t)box, (uint32_t)index);
            }
        }
    }
    return SPEC_OK;
}

static spec_error_t check_daycare(const spec_nds_daycare_t *daycare) {
    for (size_t index = 0; index < SPEC_NDS_DAYCARE_CAPACITY; ++index) {
        if (check_pokemon(&daycare->slots[index].pokemon, SPEC_NDS_BOX_RECORD_SIZE) != SPEC_OK) {
            return spec_locate_error(SPEC_ERROR_LOCATION_DAYCARE, (uint32_t)index, 0);
        }
    }
    return SPEC_OK;
}

spec_error_t spec_nds_check_storage(const spec_nds_save_t *save, const spec_nds_layout_t *layout) {
    spec_error_t error = check_party(save);
    if (error != SPEC_OK) {
        return error;
    }
    error = check_boxes(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    return check_daycare(&save->daycare);
}

void spec_nds_flag_changed_boxes(uint8_t *storage, const uint8_t *other_storage,
                                 const spec_nds_layout_t *layout) {
    uint32_t changed_boxes = 0;
    for (size_t box = 0; box < SPEC_NDS_BOX_COUNT; ++box) {
        size_t offset = box_offset(layout, box);
        bool has_changed =
            memcmp(&storage[offset], &other_storage[offset], layout->box_stride) != 0;
        changed_boxes = spec_set_bits(changed_boxes, (unsigned)box, 1, has_changed);
    }
    spec_write_u32_le(&storage[layout->box_modified_flags_offset], changed_boxes);
}
