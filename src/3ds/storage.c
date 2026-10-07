// Gen 6 and 7 Pokémon storage: the party, the PC boxes and the daycares, and how many of each.

#include <string.h>

#include "3ds/3ds.h"
#include "3ds/3ds_internal.h"
#include "nds/nds_internal.h"
#include "spec_internal.h"

constexpr size_t PARTY_COUNT_OFFSET = 0x618;

constexpr size_t BOX_NAME_FIELD_SIZE = SPEC_3DS_BOX_NAME_SIZE * 2;
constexpr size_t GEN6_WALLPAPERS_OFFSET = 0x41E;
constexpr size_t GEN6_CURRENT_BOX_OFFSET = 0x43F;
constexpr size_t GEN7_WALLPAPERS_OFFSET = 0x5C0;
constexpr size_t GEN7_CURRENT_BOX_OFFSET = 0x5E3;
constexpr size_t BOX_SIZE = SPEC_3DS_BOX_CAPACITY * SPEC_3DS_BOX_RECORD_SIZE;

// Gen 6: an occupied word and the experience gained before each record; a second daycare follows.
constexpr size_t GEN6_DAYCARE_SIZE = 0x1F0;
constexpr size_t GEN6_DAYCARE_SLOT_SIZE = 0xF0;
constexpr size_t GEN6_DAYCARE_EXPERIENCE_OFFSET = 0x4;
constexpr size_t GEN6_DAYCARE_RECORD_OFFSET = 0x8;
constexpr size_t GEN6_DAYCARE_EGG_WAITING_OFFSET = 0x1E0;
// Gen 7: an occupied byte before each record.
constexpr size_t GEN7_DAYCARE_SLOT_SIZE = 0xE9;
constexpr size_t GEN7_DAYCARE_RECORD_OFFSET = 0x1;
constexpr size_t GEN7_DAYCARE_EGG_WAITING_OFFSET = 0x1D8;

static uint8_t generation_of(const spec_3ds_layout_t *layout) {
    return layout->is_gen7 ? 7 : 6;
}

static size_t party_record_offset(const spec_3ds_layout_t *layout, size_t index) {
    return layout->party_offset + index * SPEC_3DS_PARTY_RECORD_SIZE;
}

static size_t box_record_offset(const spec_3ds_layout_t *layout, size_t box, size_t index) {
    return layout->boxes_offset + box * BOX_SIZE + index * SPEC_3DS_BOX_RECORD_SIZE;
}

static size_t daycare_slot_offset(const spec_3ds_layout_t *layout, size_t daycare, size_t index) {
    if (layout->is_gen7) {
        return layout->daycare_offset + index * GEN7_DAYCARE_SLOT_SIZE;
    }
    return layout->daycare_offset + daycare * GEN6_DAYCARE_SIZE + index * GEN6_DAYCARE_SLOT_SIZE;
}

static size_t egg_waiting_offset(const spec_3ds_layout_t *layout, size_t daycare) {
    if (layout->is_gen7) {
        return layout->daycare_offset + GEN7_DAYCARE_EGG_WAITING_OFFSET;
    }
    return layout->daycare_offset + daycare * GEN6_DAYCARE_SIZE + GEN6_DAYCARE_EGG_WAITING_OFFSET;
}

static spec_3ds_pokemon_t with_party_data(const spec_3ds_pokemon_t *pokemon) {
    spec_3ds_pokemon_t party_pokemon = *pokemon;
    spec_3ds_fill_party_data(&party_pokemon);
    return party_pokemon;
}

// A slot whose record holds no species is empty, whatever else the record holds.
static spec_error_t check_pokemon(const spec_3ds_pokemon_t *pokemon, size_t record_size,
                                  const spec_3ds_layout_t *layout) {
    if (pokemon->species == 0) {
        return SPEC_OK;
    }
    if (pokemon->generation != generation_of(layout)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "generation is not the save's");
    }
    uint8_t record[SPEC_3DS_PARTY_RECORD_SIZE];
    return spec_3ds_encode_pokemon(record, record_size, pokemon);
}

// New games fill every slot with a template that holds no species, and a Pokémon leaving one
// leaves the encrypted empty record; an empty slot is kept as found. Encoding only fails on what
// check_storage has already refused.
static void write_pokemon(uint8_t *record, size_t record_size, const spec_3ds_pokemon_t *pokemon,
                          uint8_t generation) {
    if (pokemon->species != 0) {
        (void)spec_3ds_encode_pokemon(record, record_size, pokemon);
        return;
    }
    spec_3ds_pokemon_t stored;
    spec_3ds_decode_pokemon(&stored, record, record_size, generation);
    if (stored.species != 0) {
        spec_3ds_pokemon_t empty = {.generation = generation};
        (void)spec_3ds_encode_pokemon(record, record_size, &empty);
    }
}

static void decode_party(spec_3ds_save_t *save, const uint8_t *data,
                         const spec_3ds_layout_t *layout) {
    save->party_count = data[layout->party_offset + PARTY_COUNT_OFFSET];
    for (size_t index = 0; index < SPEC_3DS_PARTY_CAPACITY; ++index) {
        spec_3ds_decode_pokemon(&save->party[index], &data[party_record_offset(layout, index)],
                                SPEC_3DS_PARTY_RECORD_SIZE, generation_of(layout));
        spec_3ds_fill_party_data(&save->party[index]);
    }
}

static void decode_boxes(spec_3ds_save_t *save, const uint8_t *data,
                         const spec_3ds_layout_t *layout) {
    const uint8_t *box_info = &data[layout->box_info_offset];
    size_t wallpapers_offset = layout->is_gen7 ? GEN7_WALLPAPERS_OFFSET : GEN6_WALLPAPERS_OFFSET;
    save->current_box =
        box_info[layout->is_gen7 ? GEN7_CURRENT_BOX_OFFSET : GEN6_CURRENT_BOX_OFFSET];
    for (size_t box = 0; box < layout->box_count; ++box) {
        spec_3ds_box_t *pc_box = &save->boxes[box];
        spec_nds_read_text(pc_box->name, &box_info[box * BOX_NAME_FIELD_SIZE],
                           SPEC_3DS_BOX_NAME_SIZE);
        pc_box->wallpaper = box_info[wallpapers_offset + box];
        for (size_t index = 0; index < SPEC_3DS_BOX_CAPACITY; ++index) {
            spec_3ds_decode_pokemon(&pc_box->pokemon[index],
                                    &data[box_record_offset(layout, box, index)],
                                    SPEC_3DS_BOX_RECORD_SIZE, generation_of(layout));
        }
    }
}

// A slot whose occupied mark is clear is empty; the game leaves the record when it hands the
// Pokémon back.
static void decode_daycare(spec_3ds_daycare_t *daycare, const uint8_t *data,
                           const spec_3ds_layout_t *layout, size_t daycare_index) {
    size_t record_offset =
        layout->is_gen7 ? GEN7_DAYCARE_RECORD_OFFSET : GEN6_DAYCARE_RECORD_OFFSET;
    for (size_t index = 0; index < SPEC_3DS_DAYCARE_CAPACITY; ++index) {
        const uint8_t *slot = &data[daycare_slot_offset(layout, daycare_index, index)];
        daycare->slots[index] = (spec_3ds_daycare_slot_t){};
        if (slot[0] == 0) {
            continue;
        }
        spec_3ds_decode_pokemon(&daycare->slots[index].pokemon, &slot[record_offset],
                                SPEC_3DS_BOX_RECORD_SIZE, generation_of(layout));
        if (!layout->is_gen7) {
            daycare->slots[index].experience_gained =
                spec_read_u32_le(&slot[GEN6_DAYCARE_EXPERIENCE_OFFSET]);
        }
    }
    daycare->is_egg_waiting = data[egg_waiting_offset(layout, daycare_index)] != 0;
}

void spec_3ds_decode_storage(spec_3ds_save_t *save, const uint8_t *data,
                             const spec_3ds_layout_t *layout) {
    decode_party(save, data, layout);
    decode_boxes(save, data, layout);
    for (size_t daycare = 0; daycare < layout->daycare_count; ++daycare) {
        decode_daycare(&save->daycares[daycare], data, layout, daycare);
    }
}

static void encode_party(uint8_t *data, const spec_3ds_layout_t *layout,
                         const spec_3ds_save_t *save) {
    data[layout->party_offset + PARTY_COUNT_OFFSET] = save->party_count;
    for (size_t index = 0; index < SPEC_3DS_PARTY_CAPACITY; ++index) {
        spec_3ds_pokemon_t party_pokemon = with_party_data(&save->party[index]);
        write_pokemon(&data[party_record_offset(layout, index)], SPEC_3DS_PARTY_RECORD_SIZE,
                      &party_pokemon, generation_of(layout));
    }
}

static void encode_boxes(uint8_t *data, const spec_3ds_layout_t *layout,
                         const spec_3ds_save_t *save) {
    uint8_t *box_info = &data[layout->box_info_offset];
    size_t wallpapers_offset = layout->is_gen7 ? GEN7_WALLPAPERS_OFFSET : GEN6_WALLPAPERS_OFFSET;
    box_info[layout->is_gen7 ? GEN7_CURRENT_BOX_OFFSET : GEN6_CURRENT_BOX_OFFSET] =
        save->current_box;
    for (size_t box = 0; box < layout->box_count; ++box) {
        const spec_3ds_box_t *pc_box = &save->boxes[box];
        spec_nds_write_text(&box_info[box * BOX_NAME_FIELD_SIZE], pc_box->name,
                            SPEC_3DS_BOX_NAME_SIZE);
        box_info[wallpapers_offset + box] = pc_box->wallpaper;
        for (size_t index = 0; index < SPEC_3DS_BOX_CAPACITY; ++index) {
            write_pokemon(&data[box_record_offset(layout, box, index)], SPEC_3DS_BOX_RECORD_SIZE,
                          &pc_box->pokemon[index], generation_of(layout));
        }
    }
}

// As the game hands a Pokémon back: the occupied mark and experience cleared, the record kept.
static void encode_daycare(uint8_t *data, const spec_3ds_layout_t *layout,
                           const spec_3ds_daycare_t *daycare, size_t daycare_index) {
    size_t record_offset =
        layout->is_gen7 ? GEN7_DAYCARE_RECORD_OFFSET : GEN6_DAYCARE_RECORD_OFFSET;
    for (size_t index = 0; index < SPEC_3DS_DAYCARE_CAPACITY; ++index) {
        uint8_t *slot = &data[daycare_slot_offset(layout, daycare_index, index)];
        const spec_3ds_daycare_slot_t *daycare_slot = &daycare->slots[index];
        bool is_occupied = daycare_slot->pokemon.species != 0;
        if (layout->is_gen7) {
            slot[0] = is_occupied ? 1 : 0;
        } else {
            spec_write_u32_le(slot, is_occupied ? 1 : 0);
            spec_write_u32_le(&slot[GEN6_DAYCARE_EXPERIENCE_OFFSET],
                              daycare_slot->experience_gained);
        }
        if (is_occupied) {
            write_pokemon(&slot[record_offset], SPEC_3DS_BOX_RECORD_SIZE, &daycare_slot->pokemon,
                          generation_of(layout));
        }
    }
    data[egg_waiting_offset(layout, daycare_index)] = daycare->is_egg_waiting ? 1 : 0;
}

void spec_3ds_encode_storage(uint8_t *data, const spec_3ds_layout_t *layout,
                             const spec_3ds_save_t *save) {
    encode_party(data, layout, save);
    encode_boxes(data, layout, save);
    for (size_t daycare = 0; daycare < layout->daycare_count; ++daycare) {
        encode_daycare(data, layout, &save->daycares[daycare], daycare);
    }
}

static spec_error_t check_party(const spec_3ds_save_t *save, const spec_3ds_layout_t *layout) {
    if (save->party_count > SPEC_3DS_PARTY_CAPACITY) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "party_count is beyond the party");
    }
    for (size_t index = 0; index < SPEC_3DS_PARTY_CAPACITY; ++index) {
        spec_3ds_pokemon_t party_pokemon = with_party_data(&save->party[index]);
        if (check_pokemon(&party_pokemon, SPEC_3DS_PARTY_RECORD_SIZE, layout) != SPEC_OK) {
            return spec_locate_error(SPEC_ERROR_LOCATION_PARTY, (uint32_t)index, 0);
        }
    }
    return SPEC_OK;
}

static bool is_box_empty(const spec_3ds_box_t *pc_box) {
    for (size_t index = 0; index < SPEC_3DS_BOX_CAPACITY; ++index) {
        if (pc_box->pokemon[index].species != 0) {
            return false;
        }
    }
    return pc_box->wallpaper == 0
           && spec_is_all_zero((const uint8_t *)pc_box->name, sizeof pc_box->name);
}

static spec_error_t check_boxes(const spec_3ds_save_t *save, const spec_3ds_layout_t *layout) {
    if (save->current_box >= layout->box_count) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "current_box is beyond the last box");
    }
    for (size_t box = 0; box < SPEC_3DS_BOX_MAX_COUNT; ++box) {
        if (box >= layout->box_count) {
            if (!is_box_empty(&save->boxes[box])) {
                (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "this game has fewer boxes");
                return spec_locate_error(SPEC_ERROR_LOCATION_BOX, (uint32_t)box, 0);
            }
            continue;
        }
        for (size_t index = 0; index < SPEC_3DS_BOX_CAPACITY; ++index) {
            if (check_pokemon(&save->boxes[box].pokemon[index], SPEC_3DS_BOX_RECORD_SIZE, layout)
                != SPEC_OK) {
                return spec_locate_error(SPEC_ERROR_LOCATION_BOX, (uint32_t)box, (uint32_t)index);
            }
        }
    }
    return SPEC_OK;
}

static spec_error_t check_daycare_slot(const spec_3ds_daycare_slot_t *daycare_slot,
                                       const spec_3ds_layout_t *layout) {
    bool is_occupied = daycare_slot->pokemon.species != 0;
    if (daycare_slot->experience_gained != 0 && (layout->is_gen7 || !is_occupied)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                         "only a Pokémon in a Gen 6 daycare gains experience there");
    }
    return check_pokemon(&daycare_slot->pokemon, SPEC_3DS_BOX_RECORD_SIZE, layout);
}

static bool is_daycare_empty(const spec_3ds_daycare_t *daycare) {
    for (size_t index = 0; index < SPEC_3DS_DAYCARE_CAPACITY; ++index) {
        const spec_3ds_daycare_slot_t *daycare_slot = &daycare->slots[index];
        if (daycare_slot->pokemon.species != 0 || daycare_slot->experience_gained != 0) {
            return false;
        }
    }
    return !daycare->is_egg_waiting;
}

static spec_error_t check_daycares(const spec_3ds_save_t *save, const spec_3ds_layout_t *layout) {
    for (size_t daycare = 0; daycare < SPEC_3DS_DAYCARE_MAX_COUNT; ++daycare) {
        uint32_t first_slot = (uint32_t)(daycare * SPEC_3DS_DAYCARE_CAPACITY);
        if (daycare >= layout->daycare_count) {
            if (!is_daycare_empty(&save->daycares[daycare])) {
                (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "this game has one daycare");
                return spec_locate_error(SPEC_ERROR_LOCATION_DAYCARE, first_slot, 0);
            }
            continue;
        }
        for (size_t index = 0; index < SPEC_3DS_DAYCARE_CAPACITY; ++index) {
            if (check_daycare_slot(&save->daycares[daycare].slots[index], layout) != SPEC_OK) {
                return spec_locate_error(SPEC_ERROR_LOCATION_DAYCARE, first_slot + (uint32_t)index,
                                         0);
            }
        }
    }
    return SPEC_OK;
}

spec_error_t spec_3ds_check_storage(const spec_3ds_save_t *save, const spec_3ds_layout_t *layout) {
    spec_error_t error = check_party(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    error = check_boxes(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    return check_daycares(save, layout);
}

size_t spec_3ds_box_count(spec_game_type_t type) {
    const spec_3ds_layout_t *layout = spec_3ds_get_layout(type);
    if (layout == nullptr) {
        return 0;
    }
    return layout->box_count;
}

size_t spec_3ds_daycare_count(spec_game_type_t type) {
    const spec_3ds_layout_t *layout = spec_3ds_get_layout(type);
    if (layout == nullptr) {
        return 0;
    }
    return layout->daycare_count;
}
