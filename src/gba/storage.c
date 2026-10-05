// Gen 3 Pokémon storage: the party, the PC boxes and the daycare.

#include "gba/gba.h"
#include "gba/gba_internal.h"
#include "spec_internal.h"

constexpr size_t BOX_NAME_FIELD_SIZE = SPEC_GBA_BOX_NAME_SIZE + 1;

static size_t party_record_offset(const spec_gba_layout_t *layout, size_t index) {
    return layout->party_offset + index * SPEC_GBA_PARTY_RECORD_SIZE;
}

static size_t box_record_offset(const spec_gba_layout_t *layout, size_t box, size_t index) {
    return layout->box_records_offset
           + (box * SPEC_GBA_BOX_CAPACITY + index) * SPEC_GBA_BOX_RECORD_SIZE;
}

static void read_pokemon(spec_gba_pokemon_t *pokemon, const uint8_t *data,
                         const spec_gba_slot_t *slot, size_t offset, size_t record_size) {
    uint8_t record[SPEC_GBA_PARTY_RECORD_SIZE];
    spec_gba_read_slot_bytes(record, data, slot, offset, record_size);
    spec_gba_decode_pokemon(pokemon, record, record_size);
}

static spec_error_t check_pokemon(const spec_gba_pokemon_t *pokemon, size_t record_size) {
    uint8_t record[SPEC_GBA_PARTY_RECORD_SIZE];
    return spec_gba_encode_pokemon(record, record_size, pokemon);
}

// Encoding only fails on what check_storage has already refused.
static void write_pokemon(uint8_t *data, const spec_gba_slot_t *slot, size_t offset,
                          size_t record_size, const spec_gba_pokemon_t *pokemon) {
    uint8_t record[SPEC_GBA_PARTY_RECORD_SIZE];
    (void)spec_gba_encode_pokemon(record, record_size, pokemon);
    spec_gba_write_slot_bytes(data, slot, offset, record, record_size);
}

static spec_gba_pokemon_t with_party_data(const spec_gba_pokemon_t *pokemon) {
    spec_gba_pokemon_t party_pokemon = *pokemon;
    spec_gba_fill_party_data(&party_pokemon);
    return party_pokemon;
}

static void decode_party(spec_gba_save_t *save, const uint8_t *data, const spec_gba_slot_t *slot,
                         const spec_gba_layout_t *layout) {
    save->party_count = spec_gba_read_slot_u8(data, slot, layout->party_count_offset);
    for (size_t index = 0; index < SPEC_GBA_PARTY_CAPACITY; ++index) {
        read_pokemon(&save->party[index], data, slot, party_record_offset(layout, index),
                     SPEC_GBA_PARTY_RECORD_SIZE);
        spec_gba_fill_party_data(&save->party[index]);
    }
}

static spec_error_t check_party(const spec_gba_save_t *save) {
    for (size_t index = 0; index < SPEC_GBA_PARTY_CAPACITY; ++index) {
        spec_gba_pokemon_t party_pokemon = with_party_data(&save->party[index]);
        if (check_pokemon(&party_pokemon, SPEC_GBA_PARTY_RECORD_SIZE) != SPEC_OK) {
            return spec_locate_error(SPEC_ERROR_LOCATION_PARTY, (uint32_t)index, 0);
        }
    }
    return SPEC_OK;
}

static void encode_party(uint8_t *data, const spec_gba_slot_t *slot,
                         const spec_gba_layout_t *layout, const spec_gba_save_t *save) {
    spec_gba_write_slot_u8(data, slot, layout->party_count_offset, save->party_count);
    for (size_t index = 0; index < SPEC_GBA_PARTY_CAPACITY; ++index) {
        spec_gba_pokemon_t party_pokemon = with_party_data(&save->party[index]);
        write_pokemon(data, slot, party_record_offset(layout, index), SPEC_GBA_PARTY_RECORD_SIZE,
                      &party_pokemon);
    }
}

static void decode_boxes(spec_gba_save_t *save, const uint8_t *data, const spec_gba_slot_t *slot,
                         const spec_gba_layout_t *layout) {
    save->current_box = spec_gba_read_slot_u8(data, slot, layout->current_box_offset);
    for (size_t box = 0; box < SPEC_GBA_BOX_COUNT; ++box) {
        spec_gba_box_t *pc_box = &save->boxes[box];
        spec_gba_read_slot_bytes(pc_box->name, data, slot,
                                 layout->box_names_offset + box * BOX_NAME_FIELD_SIZE,
                                 SPEC_GBA_BOX_NAME_SIZE);
        pc_box->wallpaper = spec_gba_read_slot_u8(data, slot, layout->wallpapers_offset + box);
        for (size_t index = 0; index < SPEC_GBA_BOX_CAPACITY; ++index) {
            read_pokemon(&pc_box->pokemon[index], data, slot, box_record_offset(layout, box, index),
                         SPEC_GBA_BOX_RECORD_SIZE);
        }
    }
}

static spec_error_t check_boxes(const spec_gba_save_t *save, const spec_gba_layout_t *layout) {
    if (save->current_box >= SPEC_GBA_BOX_COUNT) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "current_box is beyond the last box");
    }
    for (size_t box = 0; box < SPEC_GBA_BOX_COUNT; ++box) {
        if (save->boxes[box].wallpaper >= layout->wallpaper_count) {
            (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                            "the wallpaper is beyond this game's wallpapers");
            return spec_locate_error(SPEC_ERROR_LOCATION_WALLPAPER, (uint32_t)box, 0);
        }
        for (size_t index = 0; index < SPEC_GBA_BOX_CAPACITY; ++index) {
            if (check_pokemon(&save->boxes[box].pokemon[index], SPEC_GBA_BOX_RECORD_SIZE)
                != SPEC_OK) {
                return spec_locate_error(SPEC_ERROR_LOCATION_BOX, (uint32_t)box, (uint32_t)index);
            }
        }
    }
    return SPEC_OK;
}

static void encode_boxes(uint8_t *data, const spec_gba_slot_t *slot,
                         const spec_gba_layout_t *layout, const spec_gba_save_t *save) {
    spec_gba_write_slot_u8(data, slot, layout->current_box_offset, save->current_box);
    for (size_t box = 0; box < SPEC_GBA_BOX_COUNT; ++box) {
        const spec_gba_box_t *pc_box = &save->boxes[box];
        spec_gba_write_slot_name(data, slot, layout->box_names_offset + box * BOX_NAME_FIELD_SIZE,
                                 pc_box->name, SPEC_GBA_BOX_NAME_SIZE);
        spec_gba_write_slot_u8(data, slot, layout->wallpapers_offset + box, pc_box->wallpaper);
        for (size_t index = 0; index < SPEC_GBA_BOX_CAPACITY; ++index) {
            write_pokemon(data, slot, box_record_offset(layout, box, index),
                          SPEC_GBA_BOX_RECORD_SIZE, &pc_box->pokemon[index]);
        }
    }
}

static void decode_daycare(spec_gba_daycare_t *daycare, const uint8_t *data,
                           const spec_gba_slot_t *slot, const spec_gba_layout_t *layout) {
    const spec_gba_daycare_layout_t *daycare_layout = &layout->daycare;
    for (size_t index = 0; index < daycare_layout->slot_count; ++index) {
        read_pokemon(&daycare->slots[index].pokemon, data, slot,
                     daycare_layout->record_offsets[index], SPEC_GBA_BOX_RECORD_SIZE);
        daycare->slots[index].steps =
            spec_gba_read_slot_u32(data, slot, daycare_layout->steps_offsets[index]);
    }
    daycare->is_egg_waiting =
        spec_gba_read_slot_flag(data, slot, layout->flags_offset, daycare_layout->egg_waiting_flag);
    daycare->egg_personality =
        daycare_layout->egg_personality_size == 4
            ? spec_gba_read_slot_u32(data, slot, daycare_layout->egg_personality_offset)
            : spec_gba_read_slot_u16(data, slot, daycare_layout->egg_personality_offset);
    daycare->step_counter = spec_gba_read_slot_u8(data, slot, daycare_layout->step_counter_offset);
}

static spec_error_t check_daycare_slot(const spec_gba_daycare_slot_t *daycare_slot,
                                       bool does_game_have_slot) {
    if (does_game_have_slot) {
        return check_pokemon(&daycare_slot->pokemon, SPEC_GBA_BOX_RECORD_SIZE);
    }
    if (daycare_slot->pokemon.species != 0 || daycare_slot->steps != 0) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "this game's daycare has no such slot");
    }
    return SPEC_OK;
}

static spec_error_t check_daycare(const spec_gba_daycare_t *daycare,
                                  const spec_gba_layout_t *layout) {
    const spec_gba_daycare_layout_t *daycare_layout = &layout->daycare;
    for (size_t index = 0; index < SPEC_GBA_DAYCARE_CAPACITY; ++index) {
        bool does_game_have_slot = index < daycare_layout->slot_count;
        if (check_daycare_slot(&daycare->slots[index], does_game_have_slot) != SPEC_OK) {
            return spec_locate_error(SPEC_ERROR_LOCATION_DAYCARE, (uint32_t)index, 0);
        }
    }
    if (daycare_layout->egg_personality_size == 2 && daycare->egg_personality > UINT16_MAX) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                         "this game keeps only an egg personality's low 16 bits");
    }
    return SPEC_OK;
}

static void encode_daycare(uint8_t *data, const spec_gba_slot_t *slot,
                           const spec_gba_layout_t *layout, const spec_gba_daycare_t *daycare) {
    const spec_gba_daycare_layout_t *daycare_layout = &layout->daycare;
    for (size_t index = 0; index < daycare_layout->slot_count; ++index) {
        write_pokemon(data, slot, daycare_layout->record_offsets[index], SPEC_GBA_BOX_RECORD_SIZE,
                      &daycare->slots[index].pokemon);
        spec_gba_write_slot_u32(data, slot, daycare_layout->steps_offsets[index],
                                daycare->slots[index].steps);
    }
    spec_gba_write_slot_flag(data, slot, layout->flags_offset, daycare_layout->egg_waiting_flag,
                             daycare->is_egg_waiting);
    if (daycare_layout->egg_personality_size == 4) {
        spec_gba_write_slot_u32(data, slot, daycare_layout->egg_personality_offset,
                                daycare->egg_personality);
    } else {
        spec_gba_write_slot_u16(data, slot, daycare_layout->egg_personality_offset,
                                (uint16_t)daycare->egg_personality);
    }
    spec_gba_write_slot_u8(data, slot, daycare_layout->step_counter_offset, daycare->step_counter);
}

void spec_gba_decode_storage(spec_gba_save_t *save, const uint8_t *data,
                             const spec_gba_slot_t *slot, const spec_gba_layout_t *layout) {
    decode_party(save, data, slot, layout);
    decode_boxes(save, data, slot, layout);
    decode_daycare(&save->daycare, data, slot, layout);
}

spec_error_t spec_gba_check_storage(const spec_gba_save_t *save, const spec_gba_layout_t *layout) {
    spec_error_t error = check_party(save);
    if (error != SPEC_OK) {
        return error;
    }
    error = check_boxes(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    return check_daycare(&save->daycare, layout);
}

void spec_gba_encode_storage(uint8_t *data, const spec_gba_slot_t *slot,
                             const spec_gba_layout_t *layout, const spec_gba_save_t *save) {
    encode_party(data, slot, layout, save);
    encode_boxes(data, slot, layout, save);
    encode_daycare(data, slot, layout, &save->daycare);
}
