// Gen 6 and 7 items: item data, pocket rules and the bag codec.

#include "3ds/3ds.h"
#include "3ds/3ds_internal.h"
#include "3ds/tables.h"
#include "spec_internal.h"

constexpr size_t ITEM_SLOT_SIZE = 4;
constexpr size_t GEN6_QUANTITY_OFFSET = 2;
// Gen 7 packs a slot into one word.
constexpr unsigned GEN7_ITEM_BIT = 0;
constexpr unsigned GEN7_QUANTITY_BIT = 10;
constexpr unsigned GEN7_FREE_SPACE_BIT = 20;
constexpr unsigned GEN7_FIELD_BIT_COUNT = 10;
constexpr unsigned GEN7_IS_NEW_BIT = 30;

static bool is_3ds_game(spec_game_type_t type) {
    return type >= SPEC_GAME_TYPE_X_Y && type <= SPEC_GAME_TYPE_ULTRA_SUN_ULTRA_MOON;
}

static uint8_t pocket_in(uint16_t item, spec_game_type_t type) {
    if (item >= SPEC_3DS_ITEM_COUNT || !is_3ds_game(type)) {
        return SPEC_3DS_NO_POCKET;
    }
    return spec_3ds_items[item].pockets[type - SPEC_GAME_TYPE_X_Y];
}

// Gen 7 keeps a used-up item in its slot; Gen 6 empties it.
static bool is_slot_empty(const spec_3ds_item_slot_t *item_slot, const spec_3ds_layout_t *layout) {
    return item_slot->item == 0 || (!layout->is_gen7 && item_slot->quantity == 0);
}

static size_t item_slot_offset(const spec_3ds_layout_t *layout, spec_3ds_pocket_t pocket,
                               size_t index) {
    return layout->bag_offset + layout->pocket_offsets[pocket] + index * ITEM_SLOT_SIZE;
}

static void write_item_slot(uint8_t *slot, const spec_3ds_layout_t *layout,
                            const spec_3ds_item_slot_t *item_slot) {
    if (!layout->is_gen7) {
        spec_write_u16_le(slot, item_slot->item);
        spec_write_u16_le(&slot[GEN6_QUANTITY_OFFSET], item_slot->quantity);
        return;
    }
    uint32_t word = 0;
    word = spec_set_bits(word, GEN7_ITEM_BIT, GEN7_FIELD_BIT_COUNT, item_slot->item);
    word = spec_set_bits(word, GEN7_QUANTITY_BIT, GEN7_FIELD_BIT_COUNT, item_slot->quantity);
    word =
        spec_set_bits(word, GEN7_FREE_SPACE_BIT, GEN7_FIELD_BIT_COUNT, item_slot->free_space_index);
    word = spec_set_flag(word, GEN7_IS_NEW_BIT, item_slot->is_new);
    spec_write_u32_le(slot, word);
}

static spec_3ds_item_slot_t read_item_slot(const uint8_t *slot, const spec_3ds_layout_t *layout) {
    if (!layout->is_gen7) {
        return (spec_3ds_item_slot_t){
            .item = spec_read_u16_le(slot),
            .quantity = spec_read_u16_le(&slot[GEN6_QUANTITY_OFFSET]),
        };
    }
    uint32_t word = spec_read_u32_le(slot);
    return (spec_3ds_item_slot_t){
        .item = (uint16_t)spec_get_bits(word, GEN7_ITEM_BIT, GEN7_FIELD_BIT_COUNT),
        .quantity = (uint16_t)spec_get_bits(word, GEN7_QUANTITY_BIT, GEN7_FIELD_BIT_COUNT),
        .free_space_index =
            (uint16_t)spec_get_bits(word, GEN7_FREE_SPACE_BIT, GEN7_FIELD_BIT_COUNT),
        .is_new = spec_get_flag(word, GEN7_IS_NEW_BIT),
    };
}

void spec_3ds_decode_items(spec_3ds_save_t *save, const uint8_t *data,
                           const spec_3ds_layout_t *layout) {
    for (size_t pocket = 0; pocket < SPEC_3DS_POCKET_COUNT; ++pocket) {
        for (size_t index = 0; index < layout->pocket_capacities[pocket]; ++index) {
            size_t offset = item_slot_offset(layout, (spec_3ds_pocket_t)pocket, index);
            save->items[pocket][index] = read_item_slot(&data[offset], layout);
        }
    }
}

// As Gen 4 and 5's bags, Gen 6's keeps its filled slots first, in order.
static void encode_gen6_pocket(uint8_t *data, const spec_3ds_layout_t *layout,
                               spec_3ds_pocket_t pocket, const spec_3ds_item_slot_t *item_slots) {
    size_t written = 0;
    for (size_t index = 0; index < SPEC_3DS_POCKET_MAX_CAPACITY; ++index) {
        if (!is_slot_empty(&item_slots[index], layout)) {
            write_item_slot(&data[item_slot_offset(layout, pocket, written++)], layout,
                            &item_slots[index]);
        }
    }
    const spec_3ds_item_slot_t empty = {};
    for (; written < layout->pocket_capacities[pocket]; ++written) {
        write_item_slot(&data[item_slot_offset(layout, pocket, written)], layout, &empty);
    }
}

void spec_3ds_encode_items(uint8_t *data, const spec_3ds_layout_t *layout,
                           const spec_3ds_save_t *save) {
    for (size_t pocket = 0; pocket < SPEC_3DS_POCKET_COUNT; ++pocket) {
        if (!layout->is_gen7) {
            encode_gen6_pocket(data, layout, (spec_3ds_pocket_t)pocket, save->items[pocket]);
            continue;
        }
        for (size_t index = 0; index < layout->pocket_capacities[pocket]; ++index) {
            write_item_slot(&data[item_slot_offset(layout, (spec_3ds_pocket_t)pocket, index)],
                            layout, &save->items[pocket][index]);
        }
    }
}

static spec_error_t check_item_slot(const spec_3ds_item_slot_t *item_slot,
                                    const spec_3ds_layout_t *layout, spec_3ds_pocket_t pocket) {
    if (!layout->is_gen7 && (item_slot->free_space_index != 0 || item_slot->is_new)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "Gen 6 has no free space or new items");
    }
    bool does_fit = spec_fits_in_bits(item_slot->item, GEN7_FIELD_BIT_COUNT)
                    && spec_fits_in_bits(item_slot->quantity, GEN7_FIELD_BIT_COUNT)
                    && spec_fits_in_bits(item_slot->free_space_index, GEN7_FIELD_BIT_COUNT);
    if (layout->is_gen7 && !does_fit) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "an item slot does not fit in its word");
    }
    return spec_3ds_check_item_placement(layout->type, pocket, item_slot->item);
}

// Gen 6 condenses a pocket, so only its filled slots count; Gen 7 keeps each where it is.
static spec_error_t check_pocket(const spec_3ds_item_slot_t *item_slots, spec_3ds_pocket_t pocket,
                                 const spec_3ds_layout_t *layout) {
    size_t capacity = layout->pocket_capacities[pocket];
    size_t filled_slot_count = 0;
    for (size_t index = 0; index < SPEC_3DS_POCKET_MAX_CAPACITY; ++index) {
        const spec_3ds_item_slot_t *item_slot = &item_slots[index];
        bool is_empty = is_slot_empty(item_slot, layout);
        bool is_beyond = layout->is_gen7
                             ? index >= capacity && (!is_empty || item_slot->quantity != 0)
                             : !is_empty && filled_slot_count == capacity;
        if (is_beyond) {
            (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                            "the pocket holds more items than this game allows");
            return spec_locate_error(SPEC_ERROR_LOCATION_ITEMS, pocket, (uint32_t)index);
        }
        if (is_empty) {
            continue;
        }
        if (check_item_slot(item_slot, layout, pocket) != SPEC_OK) {
            return spec_locate_error(SPEC_ERROR_LOCATION_ITEMS, pocket, (uint32_t)index);
        }
        ++filled_slot_count;
    }
    return SPEC_OK;
}

spec_error_t spec_3ds_check_items(const spec_3ds_save_t *save, const spec_3ds_layout_t *layout) {
    for (size_t pocket = 0; pocket < SPEC_3DS_POCKET_COUNT; ++pocket) {
        spec_error_t error = check_pocket(save->items[pocket], (spec_3ds_pocket_t)pocket, layout);
        if (error != SPEC_OK) {
            return error;
        }
    }
    return SPEC_OK;
}

spec_error_t spec_3ds_check_item_placement(spec_game_type_t type, spec_3ds_pocket_t pocket,
                                           uint16_t item) {
    if (item == 0) {
        return SPEC_OK;
    }
    uint8_t item_pocket = pocket_in(item, type);
    if (item_pocket == SPEC_3DS_NO_POCKET) {
        return spec_fail(SPEC_ERROR_INVALID_ITEM, "the item is not in this game's bag");
    }
    if (item_pocket != pocket) {
        return spec_fail(SPEC_ERROR_WRONG_POCKET, "the item belongs in another pocket");
    }
    return SPEC_OK;
}

spec_error_t spec_3ds_get_pocket_for_item(spec_3ds_pocket_t *pocket, spec_game_type_t type,
                                          uint16_t item) {
    uint8_t item_pocket = pocket_in(item, type);
    if (item_pocket == SPEC_3DS_NO_POCKET) {
        return spec_fail(SPEC_ERROR_INVALID_ITEM, "the item is not in this game's bag");
    }
    *pocket = (spec_3ds_pocket_t)item_pocket;
    return SPEC_OK;
}

// The 3DS games' own names, which the shared name tables hold.
const char *spec_3ds_item_name(uint16_t item, spec_language_t language) {
    return spec_item_name(item, language);
}

size_t spec_3ds_pocket_capacity(spec_game_type_t type, spec_3ds_pocket_t pocket) {
    const spec_3ds_layout_t *layout = spec_3ds_get_layout(type);
    if (layout == nullptr || pocket >= SPEC_3DS_POCKET_COUNT) {
        return 0;
    }
    return layout->pocket_capacities[pocket];
}

size_t spec_3ds_pocket_item_count(const spec_3ds_save_t *save, spec_3ds_pocket_t pocket) {
    const spec_3ds_layout_t *layout = spec_3ds_get_layout(save->type);
    if (layout == nullptr || pocket >= SPEC_3DS_POCKET_COUNT) {
        return 0;
    }
    size_t item_count = 0;
    for (size_t index = 0; index < SPEC_3DS_POCKET_MAX_CAPACITY; ++index) {
        item_count += !is_slot_empty(&save->items[pocket][index], layout);
    }
    return item_count;
}
