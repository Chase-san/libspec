// Gen 1 Pokémon storage: the party, the PC boxes and the daycare, and each language's boxes.

#include <string.h>

#include "gb/gb.h"
#include "gb/gb_internal.h"
#include "spec_internal.h"

constexpr unsigned BOX_INDEX_BIT_COUNT = 7;
constexpr unsigned HAS_CHANGED_BOXES_BIT = 7;

constexpr size_t DAYCARE_NICKNAME_OFFSET = 1;

static void read_entry(spec_gb_pokemon_t *pokemon, const uint8_t *list,
                       const spec_gb_list_shape_t *shape, size_t index) {
    *pokemon = (spec_gb_pokemon_t){};
    spec_gb_decode_pokemon(pokemon, &list[spec_gb_list_record_offset(shape, index)],
                           shape->record_size);
    memcpy(pokemon->trainer.name, &list[spec_gb_list_trainer_name_offset(shape, index)],
           shape->name_size);
    memcpy(pokemon->nickname, &list[spec_gb_list_nickname_offset(shape, index)], shape->name_size);
}

// The count is kept even when it overflows the list, so writing refuses it.
static uint8_t decode_list(spec_gb_pokemon_t *pokemon, const uint8_t *list,
                           const spec_gb_list_shape_t *shape) {
    uint8_t count = list[0];
    for (size_t index = 0; index < count && index < shape->capacity; ++index) {
        read_entry(&pokemon[index], list, shape, index);
    }
    return count;
}

// Encoding only fails on what check_storage has already refused.
static void write_entry(uint8_t *list, const spec_gb_list_shape_t *shape, size_t index,
                        const spec_gb_pokemon_t *pokemon) {
    list[spec_gb_list_species_offset(shape, index)] = pokemon->species;
    (void)spec_gb_encode_pokemon(&list[spec_gb_list_record_offset(shape, index)],
                                 shape->record_size, pokemon);
    memcpy(&list[spec_gb_list_trainer_name_offset(shape, index)], pokemon->trainer.name,
           shape->name_size);
    memcpy(&list[spec_gb_list_nickname_offset(shape, index)], pokemon->nickname, shape->name_size);
}

// Only the listed Pokémon and the terminator are written; the game leaves what follows.
static void encode_list(uint8_t *list, const spec_gb_list_shape_t *shape,
                        const spec_gb_pokemon_t *pokemon, uint8_t count) {
    list[0] = count;
    for (size_t index = 0; index < count; ++index) {
        write_entry(list, shape, index, &pokemon[index]);
    }
    list[spec_gb_list_species_offset(shape, count)] = SPEC_GB_END_OF_LIST;
}

// A name's bytes past a Japanese name's 6 must be zero.
static spec_error_t check_pokemon(const spec_gb_pokemon_t *pokemon, size_t record_size,
                                  size_t name_size) {
    size_t unused_name_size = SPEC_GB_NAME_SIZE - name_size;
    if (!spec_is_all_zero(&pokemon->nickname[name_size], unused_name_size)
        || !spec_is_all_zero(&pokemon->trainer.name[name_size], unused_name_size)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "a name is longer than this game's names");
    }
    uint8_t record[SPEC_GB_PARTY_RECORD_SIZE];
    return spec_gb_encode_pokemon(record, record_size, pokemon);
}

static spec_error_t check_list(const spec_gb_pokemon_t *pokemon, uint8_t count,
                               const spec_gb_list_shape_t *shape, spec_error_location_t location,
                               uint32_t box) {
    if (count > shape->capacity) {
        (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "the count is beyond the list");
        return spec_locate_error(location, box, 0);
    }
    for (size_t index = 0; index < count; ++index) {
        if (check_pokemon(&pokemon[index], shape->record_size, shape->name_size) != SPEC_OK) {
            uint32_t index0 = location == SPEC_ERROR_LOCATION_BOX ? box : (uint32_t)index;
            uint32_t index1 = location == SPEC_ERROR_LOCATION_BOX ? (uint32_t)index : 0;
            return spec_locate_error(location, index0, index1);
        }
    }
    return SPEC_OK;
}

static spec_gb_pokemon_t with_party_data(const spec_gb_pokemon_t *pokemon) {
    spec_gb_pokemon_t party_pokemon = *pokemon;
    spec_gb_fill_party_data(&party_pokemon);
    return party_pokemon;
}

static size_t box_offset(const spec_gb_layout_t *layout, size_t box) {
    size_t bank = SPEC_GB_FIRST_BOX_BANK + box / layout->boxes_per_bank;
    size_t box_in_bank = box % layout->boxes_per_bank;
    return bank * SPEC_GB_BANK_SIZE + box_in_bank * spec_gb_list_size(&layout->box_shape);
}

static uint8_t current_box_of(uint8_t current_box_byte) {
    return (uint8_t)spec_get_bits(current_box_byte, 0, BOX_INDEX_BIT_COUNT);
}

static bool has_changed_boxes(uint8_t current_box_byte) {
    return spec_get_bits(current_box_byte, HAS_CHANGED_BOXES_BIT, 1) != 0;
}

static size_t daycare_trainer_name_offset(const spec_gb_layout_t *layout) {
    return DAYCARE_NICKNAME_OFFSET + layout->name_size;
}

static size_t daycare_record_offset(const spec_gb_layout_t *layout) {
    return daycare_trainer_name_offset(layout) + layout->name_size;
}

static void decode_party(spec_gb_save_t *save, const uint8_t *data,
                         const spec_gb_layout_t *layout) {
    save->party_count = decode_list(save->party, &data[layout->party_offset], &layout->party_shape);
    for (size_t index = 0; index < SPEC_GB_PARTY_CAPACITY; ++index) {
        spec_gb_fill_party_data(&save->party[index]);
    }
}

// The current box lives in the game data; the stored boxes count only once the player has
// changed boxes, and hold whatever the cartridge did before then.
static void decode_boxes(spec_gb_save_t *save, const uint8_t *data,
                         const spec_gb_layout_t *layout) {
    uint8_t current_box_byte = data[layout->current_box_offset];
    save->current_box = current_box_of(current_box_byte);
    for (size_t box = 0; box < layout->box_count; ++box) {
        spec_gb_box_t *pc_box = &save->boxes[box];
        if (box == save->current_box) {
            pc_box->count = decode_list(pc_box->pokemon, &data[layout->current_box_list_offset],
                                        &layout->box_shape);
        } else if (has_changed_boxes(current_box_byte)) {
            pc_box->count =
                decode_list(pc_box->pokemon, &data[box_offset(layout, box)], &layout->box_shape);
        }
    }
}

// The in-use byte decides; withdrawing leaves the Pokémon's bytes behind.
static void decode_daycare(spec_gb_pokemon_t *daycare, const uint8_t *data,
                           const spec_gb_layout_t *layout) {
    const uint8_t *bytes = &data[layout->daycare_offset];
    *daycare = (spec_gb_pokemon_t){};
    if (bytes[0] == 0) {
        return;
    }
    spec_gb_decode_pokemon(daycare, &bytes[daycare_record_offset(layout)], SPEC_GB_BOX_RECORD_SIZE);
    memcpy(daycare->nickname, &bytes[DAYCARE_NICKNAME_OFFSET], layout->name_size);
    memcpy(daycare->trainer.name, &bytes[daycare_trainer_name_offset(layout)], layout->name_size);
}

void spec_gb_decode_storage(spec_gb_save_t *save, const uint8_t *data,
                            const spec_gb_layout_t *layout) {
    decode_party(save, data, layout);
    decode_boxes(save, data, layout);
    decode_daycare(&save->daycare, data, layout);
}

static void encode_party(uint8_t *data, const spec_gb_layout_t *layout,
                         const spec_gb_save_t *save) {
    spec_gb_pokemon_t party[SPEC_GB_PARTY_CAPACITY];
    for (size_t index = 0; index < SPEC_GB_PARTY_CAPACITY; ++index) {
        party[index] = with_party_data(&save->party[index]);
    }
    encode_list(&data[layout->party_offset], &layout->party_shape, party, save->party_count);
}

static bool is_any_other_box_filled(const spec_gb_save_t *save, const spec_gb_layout_t *layout) {
    for (size_t box = 0; box < layout->box_count; ++box) {
        if (box != save->current_box && save->boxes[box].count != 0) {
            return true;
        }
    }
    return false;
}

// As ChangeBox: the current box's slot is marked empty, and the first change sets the flag.
// Until the player changes boxes, the stored boxes are left as they are.
static void encode_boxes(uint8_t *data, const spec_gb_layout_t *layout,
                         const spec_gb_save_t *save) {
    uint8_t old_current_box_byte = data[layout->current_box_offset];
    bool is_box_changed = save->current_box != current_box_of(old_current_box_byte);
    bool needs_stored_boxes = has_changed_boxes(old_current_box_byte) || is_box_changed
                              || is_any_other_box_filled(save, layout);
    const spec_gb_box_t *current_box = &save->boxes[save->current_box];
    encode_list(&data[layout->current_box_list_offset], &layout->box_shape, current_box->pokemon,
                current_box->count);
    if (!needs_stored_boxes) {
        data[layout->current_box_offset] = save->current_box;
        return;
    }
    for (size_t box = 0; box < layout->box_count; ++box) {
        uint8_t count = box == save->current_box ? 0 : save->boxes[box].count;
        encode_list(&data[box_offset(layout, box)], &layout->box_shape, save->boxes[box].pokemon,
                    count);
    }
    for (size_t bank = 0; bank < SPEC_GB_BOX_BANK_COUNT; ++bank) {
        spec_gb_stamp_box_bank(data, layout, SPEC_GB_FIRST_BOX_BANK + bank);
    }
    data[layout->current_box_offset] =
        (uint8_t)spec_set_bits(save->current_box, HAS_CHANGED_BOXES_BIT, 1, 1);
}

static void encode_daycare(uint8_t *data, const spec_gb_layout_t *layout,
                           const spec_gb_pokemon_t *daycare) {
    uint8_t *bytes = &data[layout->daycare_offset];
    bytes[0] = daycare->species != 0 ? 1 : 0;
    if (daycare->species == 0) {
        return;
    }
    (void)spec_gb_encode_pokemon(&bytes[daycare_record_offset(layout)], SPEC_GB_BOX_RECORD_SIZE,
                                 daycare);
    memcpy(&bytes[DAYCARE_NICKNAME_OFFSET], daycare->nickname, layout->name_size);
    memcpy(&bytes[daycare_trainer_name_offset(layout)], daycare->trainer.name, layout->name_size);
}

void spec_gb_encode_storage(uint8_t *data, const spec_gb_layout_t *layout,
                            const spec_gb_save_t *save) {
    encode_party(data, layout, save);
    encode_boxes(data, layout, save);
    encode_daycare(data, layout, &save->daycare);
}

static spec_error_t check_party(const spec_gb_save_t *save, const spec_gb_layout_t *layout) {
    spec_gb_pokemon_t party[SPEC_GB_PARTY_CAPACITY];
    for (size_t index = 0; index < SPEC_GB_PARTY_CAPACITY; ++index) {
        party[index] = with_party_data(&save->party[index]);
    }
    return check_list(party, save->party_count, &layout->party_shape, SPEC_ERROR_LOCATION_PARTY, 0);
}

static spec_error_t check_boxes(const spec_gb_save_t *save, const spec_gb_layout_t *layout) {
    if (save->current_box >= layout->box_count) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "current_box is beyond the last box");
    }
    for (size_t box = 0; box < SPEC_GB_BOX_COUNT; ++box) {
        const spec_gb_box_t *pc_box = &save->boxes[box];
        if (box >= layout->box_count && pc_box->count != 0) {
            (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "this game has no such box");
            return spec_locate_error(SPEC_ERROR_LOCATION_BOX, (uint32_t)box, 0);
        }
        spec_error_t error = check_list(pc_box->pokemon, pc_box->count, &layout->box_shape,
                                        SPEC_ERROR_LOCATION_BOX, (uint32_t)box);
        if (error != SPEC_OK) {
            return error;
        }
    }
    return SPEC_OK;
}

static spec_error_t check_daycare(const spec_gb_pokemon_t *daycare,
                                  const spec_gb_layout_t *layout) {
    if (daycare->species != 0
        && check_pokemon(daycare, SPEC_GB_BOX_RECORD_SIZE, layout->name_size) != SPEC_OK) {
        return spec_locate_error(SPEC_ERROR_LOCATION_DAYCARE, 0, 0);
    }
    return SPEC_OK;
}

spec_error_t spec_gb_check_storage(const spec_gb_save_t *save, const spec_gb_layout_t *layout) {
    spec_error_t error = check_party(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    error = check_boxes(save, layout);
    if (error != SPEC_OK) {
        return error;
    }
    return check_daycare(&save->daycare, layout);
}

size_t spec_gb_box_capacity(spec_language_t language) {
    const spec_gb_layout_t *layout = spec_gb_get_layout(language);
    if (layout == nullptr) {
        return 0;
    }
    return layout->box_shape.capacity;
}

size_t spec_gb_box_count(spec_language_t language) {
    const spec_gb_layout_t *layout = spec_gb_get_layout(language);
    if (layout == nullptr) {
        return 0;
    }
    return layout->box_count;
}
