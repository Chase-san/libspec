// Gen 2 Pokémon storage: the party, the PC boxes and the daycare.

#include <string.h>

#include "gbc/gbc.h"
#include "gbc/gbc_internal.h"
#include "spec_internal.h"

// Each stored box's list ends in two bytes the game only erases.
constexpr size_t BOX_PADDING_SIZE = 2;

constexpr unsigned DAYCARE_HAS_POKEMON_BIT = 0;
constexpr unsigned DAYCARE_MAN_COMPATIBLE_BIT = 5;
constexpr unsigned DAYCARE_MAN_HAS_EGG_BIT = 6;

static void read_entry(spec_gbc_pokemon_t *pokemon, const uint8_t *list,
                       const spec_gb_list_shape_t *shape, size_t index) {
    *pokemon = (spec_gbc_pokemon_t){};
    spec_gbc_decode_pokemon(pokemon, &list[spec_gb_list_record_offset(shape, index)],
                            shape->record_size);
    pokemon->is_egg = list[spec_gb_list_species_offset(shape, index)] == SPEC_GBC_EGG;
    memcpy(pokemon->trainer.name, &list[spec_gb_list_trainer_name_offset(shape, index)],
           shape->name_size);
    memcpy(pokemon->nickname, &list[spec_gb_list_nickname_offset(shape, index)], shape->name_size);
}

// The count is kept even when it overflows the list, so writing refuses it.
static uint8_t decode_list(spec_gbc_pokemon_t *pokemon, const uint8_t *list,
                           const spec_gb_list_shape_t *shape) {
    uint8_t count = list[0];
    for (size_t index = 0; index < count && index < shape->capacity; ++index) {
        read_entry(&pokemon[index], list, shape, index);
    }
    return count;
}

// Encoding only fails on what check_storage has already refused.
static void write_entry(uint8_t *list, const spec_gb_list_shape_t *shape, size_t index,
                        const spec_gbc_pokemon_t *pokemon) {
    list[spec_gb_list_species_offset(shape, index)] =
        pokemon->is_egg ? SPEC_GBC_EGG : pokemon->species;
    (void)spec_gbc_encode_pokemon(&list[spec_gb_list_record_offset(shape, index)],
                                  shape->record_size, pokemon);
    memcpy(&list[spec_gb_list_trainer_name_offset(shape, index)], pokemon->trainer.name,
           shape->name_size);
    memcpy(&list[spec_gb_list_nickname_offset(shape, index)], pokemon->nickname, shape->name_size);
}

// Only the listed Pokémon and the terminator are written; the game leaves what follows.
static void encode_list(uint8_t *list, const spec_gb_list_shape_t *shape,
                        const spec_gbc_pokemon_t *pokemon, uint8_t count) {
    list[0] = count;
    for (size_t index = 0; index < count; ++index) {
        write_entry(list, shape, index, &pokemon[index]);
    }
    list[spec_gb_list_species_offset(shape, count)] = SPEC_GB_END_OF_LIST;
}

// A name's bytes past a Japanese name's 6 must be zero.
static spec_error_t check_pokemon(const spec_gbc_pokemon_t *pokemon, size_t record_size,
                                  size_t name_size) {
    size_t unused_name_size = SPEC_GBC_NAME_SIZE - name_size;
    if (!spec_is_all_zero(&pokemon->nickname[name_size], unused_name_size)
        || !spec_is_all_zero(&pokemon->trainer.name[name_size], unused_name_size)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "a name is longer than this game's names");
    }
    uint8_t record[SPEC_GBC_PARTY_RECORD_SIZE];
    return spec_gbc_encode_pokemon(record, record_size, pokemon);
}

static spec_error_t check_list(const spec_gbc_pokemon_t *pokemon, uint8_t count,
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

static spec_gbc_pokemon_t with_party_data(const spec_gbc_pokemon_t *pokemon) {
    spec_gbc_pokemon_t party_pokemon = *pokemon;
    spec_gbc_fill_party_data(&party_pokemon);
    return party_pokemon;
}

static size_t box_offset(const spec_gbc_layout_t *layout, size_t box) {
    size_t bank = SPEC_GB_FIRST_BOX_BANK + box / layout->boxes_per_bank;
    size_t box_size = spec_gb_list_size(&layout->box_shape) + BOX_PADDING_SIZE;
    return bank * SPEC_GB_BANK_SIZE + box % layout->boxes_per_bank * box_size;
}

// The man's byte and his parent, the lady's byte, the steps to the egg, the mother, the lady's
// parent, then the next egg; each Pokémon is its nickname, OT name and record.
static size_t daycare_pokemon_size(const spec_gbc_layout_t *layout) {
    return 2 * layout->name_size + SPEC_GBC_BOX_RECORD_SIZE;
}

static size_t daycare_lady_offset(const spec_gbc_layout_t *layout) {
    return layout->daycare_offset + 1 + daycare_pokemon_size(layout);
}

static size_t daycare_pokemon_offset(const spec_gbc_layout_t *layout, size_t index) {
    return index == 0 ? layout->daycare_offset + 1 : daycare_lady_offset(layout) + 3;
}

static size_t daycare_egg_offset(const spec_gbc_layout_t *layout) {
    return daycare_pokemon_offset(layout, 1) + daycare_pokemon_size(layout);
}

static void decode_party(spec_gbc_save_t *save, const uint8_t *data,
                         const spec_gbc_layout_t *layout) {
    save->party_count = decode_list(save->party, &data[layout->party_offset], &layout->party_shape);
    for (size_t index = 0; index < SPEC_GBC_PARTY_CAPACITY; ++index) {
        spec_gbc_fill_party_data(&save->party[index]);
    }
}

// As LoadBox: the stored boxes count, not the working copy of the current one.
static void decode_boxes(spec_gbc_save_t *save, const uint8_t *data,
                         const spec_gbc_layout_t *layout) {
    save->current_box = data[layout->current_box_offset];
    for (size_t box = 0; box < layout->box_count; ++box) {
        spec_gbc_box_t *pc_box = &save->boxes[box];
        memcpy(pc_box->name, &data[layout->box_names_offset + box * SPEC_GBC_BOX_NAME_SIZE],
               SPEC_GBC_BOX_NAME_SIZE);
        pc_box->count =
            decode_list(pc_box->pokemon, &data[box_offset(layout, box)], &layout->box_shape);
    }
}

static void read_daycare_pokemon(spec_gbc_pokemon_t *pokemon, const uint8_t *bytes,
                                 const spec_gbc_layout_t *layout) {
    *pokemon = (spec_gbc_pokemon_t){};
    memcpy(pokemon->nickname, bytes, layout->name_size);
    memcpy(pokemon->trainer.name, &bytes[layout->name_size], layout->name_size);
    spec_gbc_decode_pokemon(pokemon, &bytes[2 * layout->name_size], SPEC_GBC_BOX_RECORD_SIZE);
}

// A slot's bit decides; withdrawing leaves the Pokémon's bytes behind.
static void decode_daycare(spec_gbc_daycare_t *daycare, const uint8_t *data,
                           const spec_gbc_layout_t *layout) {
    uint8_t man_byte = data[layout->daycare_offset];
    uint8_t lady_byte = data[daycare_lady_offset(layout)];
    uint8_t slot_bytes[SPEC_GBC_DAYCARE_CAPACITY] = {man_byte, lady_byte};
    for (size_t index = 0; index < SPEC_GBC_DAYCARE_CAPACITY; ++index) {
        daycare->parents[index] = (spec_gbc_pokemon_t){};
        if (spec_get_bits(slot_bytes[index], DAYCARE_HAS_POKEMON_BIT, 1) != 0) {
            read_daycare_pokemon(&daycare->parents[index],
                                 &data[daycare_pokemon_offset(layout, index)], layout);
        }
    }
    daycare->are_parents_compatible = spec_get_bits(man_byte, DAYCARE_MAN_COMPATIBLE_BIT, 1) != 0;
    daycare->is_egg_waiting = spec_get_bits(man_byte, DAYCARE_MAN_HAS_EGG_BIT, 1) != 0;
    daycare->steps_to_egg = data[daycare_lady_offset(layout) + 1];
    daycare->mother_index = data[daycare_lady_offset(layout) + 2];
    read_daycare_pokemon(&daycare->egg, &data[daycare_egg_offset(layout)], layout);
    daycare->egg.is_egg = daycare->egg.species != 0;
}

void spec_gbc_decode_storage(spec_gbc_save_t *save, const uint8_t *data,
                             const spec_gbc_layout_t *layout) {
    decode_party(save, data, layout);
    decode_boxes(save, data, layout);
    decode_daycare(&save->daycare, data, layout);
}

static void encode_party(uint8_t *data, const spec_gbc_layout_t *layout,
                         const spec_gbc_save_t *save) {
    spec_gbc_pokemon_t party[SPEC_GBC_PARTY_CAPACITY];
    for (size_t index = 0; index < SPEC_GBC_PARTY_CAPACITY; ++index) {
        party[index] = with_party_data(&save->party[index]);
    }
    encode_list(&data[layout->party_offset], &layout->party_shape, party, save->party_count);
}

// As SaveBox: the working copy of the current box matches its stored one.
static void encode_boxes(uint8_t *data, const spec_gbc_layout_t *layout,
                         const spec_gbc_save_t *save) {
    data[layout->current_box_offset] = save->current_box;
    for (size_t box = 0; box < layout->box_count; ++box) {
        const spec_gbc_box_t *pc_box = &save->boxes[box];
        memcpy(&data[layout->box_names_offset + box * SPEC_GBC_BOX_NAME_SIZE], pc_box->name,
               SPEC_GBC_BOX_NAME_SIZE);
        encode_list(&data[box_offset(layout, box)], &layout->box_shape, pc_box->pokemon,
                    pc_box->count);
    }
    memcpy(&data[layout->active_box_offset], &data[box_offset(layout, save->current_box)],
           spec_gb_list_size(&layout->box_shape));
}

static void write_daycare_pokemon(uint8_t *bytes, const spec_gbc_layout_t *layout,
                                  const spec_gbc_pokemon_t *pokemon) {
    memcpy(bytes, pokemon->nickname, layout->name_size);
    memcpy(&bytes[layout->name_size], pokemon->trainer.name, layout->name_size);
    (void)spec_gbc_encode_pokemon(&bytes[2 * layout->name_size], SPEC_GBC_BOX_RECORD_SIZE, pokemon);
}

// The bytes' other bits record conversations, and are left as they are.
static void encode_daycare(uint8_t *data, const spec_gbc_layout_t *layout,
                           const spec_gbc_daycare_t *daycare) {
    uint32_t man_byte = data[layout->daycare_offset];
    man_byte =
        spec_set_bits(man_byte, DAYCARE_HAS_POKEMON_BIT, 1, daycare->parents[0].species != 0);
    man_byte =
        spec_set_bits(man_byte, DAYCARE_MAN_COMPATIBLE_BIT, 1, daycare->are_parents_compatible);
    man_byte = spec_set_bits(man_byte, DAYCARE_MAN_HAS_EGG_BIT, 1, daycare->is_egg_waiting);
    uint32_t lady_byte = data[daycare_lady_offset(layout)];
    lady_byte =
        spec_set_bits(lady_byte, DAYCARE_HAS_POKEMON_BIT, 1, daycare->parents[1].species != 0);
    data[layout->daycare_offset] = (uint8_t)man_byte;
    data[daycare_lady_offset(layout)] = (uint8_t)lady_byte;
    data[daycare_lady_offset(layout) + 1] = daycare->steps_to_egg;
    data[daycare_lady_offset(layout) + 2] = daycare->mother_index;
    for (size_t index = 0; index < SPEC_GBC_DAYCARE_CAPACITY; ++index) {
        if (daycare->parents[index].species != 0) {
            write_daycare_pokemon(&data[daycare_pokemon_offset(layout, index)], layout,
                                  &daycare->parents[index]);
        }
    }
    write_daycare_pokemon(&data[daycare_egg_offset(layout)], layout, &daycare->egg);
}

void spec_gbc_encode_storage(uint8_t *data, const spec_gbc_layout_t *layout,
                             const spec_gbc_save_t *save) {
    encode_party(data, layout, save);
    encode_boxes(data, layout, save);
    encode_daycare(data, layout, &save->daycare);
}

static spec_error_t check_party(const spec_gbc_save_t *save, const spec_gbc_layout_t *layout) {
    spec_gbc_pokemon_t party[SPEC_GBC_PARTY_CAPACITY];
    for (size_t index = 0; index < SPEC_GBC_PARTY_CAPACITY; ++index) {
        party[index] = with_party_data(&save->party[index]);
    }
    spec_error_t error =
        check_list(party, save->party_count, &layout->party_shape, SPEC_ERROR_LOCATION_PARTY, 0);
    if (error != SPEC_OK) {
        return error;
    }
    for (size_t index = 0; index < save->party_count; ++index) {
        if (spec_gbc_check_mail(&party[index].party_data.mail, layout) != SPEC_OK) {
            return spec_locate_error(SPEC_ERROR_LOCATION_PARTY, (uint32_t)index, 0);
        }
    }
    return SPEC_OK;
}

static spec_error_t check_boxes(const spec_gbc_save_t *save, const spec_gbc_layout_t *layout) {
    if (save->current_box >= layout->box_count) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "current_box is beyond the last box");
    }
    for (size_t box = 0; box < SPEC_GBC_BOX_COUNT; ++box) {
        const spec_gbc_box_t *pc_box = &save->boxes[box];
        bool does_game_have_box = box < layout->box_count;
        if (!does_game_have_box
            && (pc_box->count != 0 || !spec_is_all_zero(pc_box->name, SPEC_GBC_BOX_NAME_SIZE))) {
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

static spec_error_t check_daycare(const spec_gbc_daycare_t *daycare,
                                  const spec_gbc_layout_t *layout) {
    for (size_t index = 0; index < SPEC_GBC_DAYCARE_CAPACITY; ++index) {
        const spec_gbc_pokemon_t *parent = &daycare->parents[index];
        if (parent->species != 0
            && check_pokemon(parent, SPEC_GBC_BOX_RECORD_SIZE, layout->name_size) != SPEC_OK) {
            return spec_locate_error(SPEC_ERROR_LOCATION_DAYCARE, (uint32_t)index, 0);
        }
    }
    if (check_pokemon(&daycare->egg, SPEC_GBC_BOX_RECORD_SIZE, layout->name_size) != SPEC_OK) {
        return spec_locate_error(SPEC_ERROR_LOCATION_DAYCARE, SPEC_GBC_DAYCARE_CAPACITY, 0);
    }
    return SPEC_OK;
}

spec_error_t spec_gbc_check_storage(const spec_gbc_save_t *save, const spec_gbc_layout_t *layout) {
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
