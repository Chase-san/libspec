// Gen 1 items: item names, and the bag and PC codec.

#include "gb/gb.h"
#include "gb/gb_internal.h"
#include "gb/tables.h"
#include "spec_internal.h"

constexpr size_t ITEM_SLOT_SIZE = 2;
constexpr size_t ITEM_QUANTITY_OFFSET = 1;
constexpr size_t FIRST_ITEM_SLOT_OFFSET = 1;

constexpr size_t POCKET_CAPACITIES[SPEC_GB_POCKET_COUNT] = {
    [SPEC_GB_POCKET_BAG] = 20,
    [SPEC_GB_POCKET_PC] = 50,
};

static size_t pocket_offset(const spec_gb_layout_t *layout, spec_gb_pocket_t pocket) {
    return pocket == SPEC_GB_POCKET_BAG ? layout->bag_offset : layout->pc_items_offset;
}

static bool is_item(uint16_t item) {
    return item < SPEC_GB_ITEM_COUNT && spec_gb_item_names[item] != nullptr;
}

static size_t item_slot_offset(size_t index) {
    return FIRST_ITEM_SLOT_OFFSET + index * ITEM_SLOT_SIZE;
}

void spec_gb_decode_items(spec_gb_save_t *save, const uint8_t *data,
                          const spec_gb_layout_t *layout) {
    for (size_t pocket = 0; pocket < SPEC_GB_POCKET_COUNT; ++pocket) {
        const uint8_t *bytes = &data[pocket_offset(layout, (spec_gb_pocket_t)pocket)];
        for (size_t index = 0; index < bytes[0] && index < POCKET_CAPACITIES[pocket]; ++index) {
            save->items[pocket][index] = (spec_gb_item_slot_t){
                .item = bytes[item_slot_offset(index)],
                .quantity = bytes[item_slot_offset(index) + ITEM_QUANTITY_OFFSET],
            };
        }
    }
}

// A count, the slots, then a terminator; the game leaves what follows.
static void encode_pocket(uint8_t *bytes, const spec_gb_item_slot_t *item_slots) {
    spec_gb_item_slot_t condensed[SPEC_GB_POCKET_MAX_CAPACITY];
    spec_condense_pocket(condensed, item_slots, SPEC_GB_POCKET_MAX_CAPACITY);
    size_t count = spec_count_filled_slots(condensed, SPEC_GB_POCKET_MAX_CAPACITY);
    bytes[0] = (uint8_t)count;
    for (size_t index = 0; index < count; ++index) {
        bytes[item_slot_offset(index)] = (uint8_t)condensed[index].item;
        bytes[item_slot_offset(index) + ITEM_QUANTITY_OFFSET] = (uint8_t)condensed[index].quantity;
    }
    bytes[item_slot_offset(count)] = SPEC_GB_END_OF_LIST;
}

void spec_gb_encode_items(uint8_t *data, const spec_gb_layout_t *layout,
                          const spec_gb_save_t *save) {
    for (size_t pocket = 0; pocket < SPEC_GB_POCKET_COUNT; ++pocket) {
        encode_pocket(&data[pocket_offset(layout, (spec_gb_pocket_t)pocket)], save->items[pocket]);
    }
}

static spec_error_t check_pocket(const spec_gb_item_slot_t *item_slots, spec_gb_pocket_t pocket) {
    size_t filled_slot_count = 0;
    for (size_t index = 0; index < SPEC_GB_POCKET_MAX_CAPACITY; ++index) {
        if (spec_is_item_slot_empty(&item_slots[index])) {
            continue;
        }
        if (filled_slot_count == POCKET_CAPACITIES[pocket]) {
            (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                            "the pocket holds more items than the game allows");
            return spec_locate_error(SPEC_ERROR_LOCATION_ITEMS, pocket, (uint32_t)index);
        }
        if (spec_gb_check_item_placement(pocket, item_slots[index].item) != SPEC_OK) {
            return spec_locate_error(SPEC_ERROR_LOCATION_ITEMS, pocket, (uint32_t)index);
        }
        if (item_slots[index].quantity > UINT8_MAX) {
            (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "the quantity does not fit in a byte");
            return spec_locate_error(SPEC_ERROR_LOCATION_ITEMS, pocket, (uint32_t)index);
        }
        ++filled_slot_count;
    }
    return SPEC_OK;
}

spec_error_t spec_gb_check_items(const spec_gb_save_t *save) {
    for (size_t pocket = 0; pocket < SPEC_GB_POCKET_COUNT; ++pocket) {
        spec_error_t error = check_pocket(save->items[pocket], (spec_gb_pocket_t)pocket);
        if (error != SPEC_OK) {
            return error;
        }
    }
    return SPEC_OK;
}

// Either pocket takes any item.
spec_error_t spec_gb_check_item_placement(spec_gb_pocket_t pocket, uint16_t item) {
    if (pocket >= SPEC_GB_POCKET_COUNT) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "pocket is not a Gen 1 pocket");
    }
    if (item != 0 && !is_item(item)) {
        return spec_fail(SPEC_ERROR_INVALID_ITEM, "the item is not in this game");
    }
    return SPEC_OK;
}

// TODO: Item names for other languages; pret's localized builds have them.
const char *spec_gb_item_name(uint16_t item, spec_language_t language) {
    if (!is_item(item) || language != SPEC_LANGUAGE_ENGLISH) {
        return nullptr;
    }
    return spec_gb_item_names[item];
}

size_t spec_gb_pocket_capacity(spec_gb_pocket_t pocket) {
    if (pocket >= SPEC_GB_POCKET_COUNT) {
        return 0;
    }
    return POCKET_CAPACITIES[pocket];
}

size_t spec_gb_pocket_item_count(const spec_gb_save_t *save, spec_gb_pocket_t pocket) {
    if (pocket >= SPEC_GB_POCKET_COUNT) {
        return 0;
    }
    return spec_count_filled_slots(save->items[pocket], SPEC_GB_POCKET_MAX_CAPACITY);
}
