// Gen 2 items: item data, pocket rules, and the bag and PC codec.

#include <string.h>

#include "gbc/gbc.h"
#include "gbc/gbc_internal.h"
#include "gbc/tables.h"
#include "spec_internal.h"

constexpr size_t ITEM_SLOT_SIZE = 2;
constexpr size_t ITEM_QUANTITY_OFFSET = 1;
constexpr size_t FIRST_SLOT_OFFSET = 1;

constexpr size_t POCKET_CAPACITIES[SPEC_GBC_POCKET_COUNT] = {
    [SPEC_GBC_POCKET_ITEMS] = 20,     [SPEC_GBC_POCKET_BALLS] = 12,
    [SPEC_GBC_POCKET_KEY_ITEMS] = 25, [SPEC_GBC_POCKET_TMS_HMS] = SPEC_GBC_MACHINE_COUNT,
    [SPEC_GBC_POCKET_PC] = 50,
};

static const spec_gbc_item_data_t *item_data_of(uint16_t item) {
    if (item >= SPEC_GBC_ITEM_COUNT || spec_gbc_items[item].english_name == nullptr) {
        return nullptr;
    }
    return &spec_gbc_items[item];
}

static uint8_t pocket_in(const spec_gbc_item_data_t *item_data, spec_game_type_t type) {
    switch (type) {
        case SPEC_GAME_TYPE_GOLD_SILVER:
            return item_data->gold_silver_pocket;
        case SPEC_GAME_TYPE_CRYSTAL:
            return item_data->crystal_pocket;
        default:
            return SPEC_GBC_NO_POCKET;
    }
}

static bool is_in_game(const spec_gbc_item_data_t *item_data, spec_game_type_t type) {
    return item_data != nullptr && pocket_in(item_data, type) != SPEC_GBC_NO_POCKET;
}

static size_t pocket_offset(const spec_gbc_layout_t *layout, spec_gbc_pocket_t pocket) {
    switch (pocket) {
        case SPEC_GBC_POCKET_ITEMS:
            return layout->items_offset;
        case SPEC_GBC_POCKET_BALLS:
            return layout->balls_offset;
        case SPEC_GBC_POCKET_KEY_ITEMS:
            return layout->key_items_offset;
        case SPEC_GBC_POCKET_TMS_HMS:
            return layout->tms_hms_offset;
        default:
            return layout->pc_items_offset;
    }
}

// Key items are listed without quantities.
static size_t slot_size_of(spec_gbc_pocket_t pocket) {
    return pocket == SPEC_GBC_POCKET_KEY_ITEMS ? 1 : ITEM_SLOT_SIZE;
}

// The TM/HM pocket is one quantity per machine, in machine order.
static void decode_machines(spec_gbc_item_slot_t *item_slots, const uint8_t *quantities) {
    size_t filled_slot_count = 0;
    for (uint16_t item = 0; item < SPEC_GBC_ITEM_COUNT; ++item) {
        uint8_t machine = spec_gbc_items[item].machine;
        if (machine != 0 && quantities[machine - 1] != 0) {
            item_slots[filled_slot_count++] =
                (spec_gbc_item_slot_t){.item = item, .quantity = quantities[machine - 1]};
        }
    }
}

// A count, the slots, then a terminator; the game leaves what follows.
static void decode_list(spec_gbc_item_slot_t *item_slots, const uint8_t *bytes,
                        spec_gbc_pocket_t pocket) {
    size_t slot_size = slot_size_of(pocket);
    for (size_t index = 0; index < bytes[0] && index < POCKET_CAPACITIES[pocket]; ++index) {
        const uint8_t *slot = &bytes[FIRST_SLOT_OFFSET + index * slot_size];
        uint8_t quantity = slot_size == 1 ? 1 : slot[ITEM_QUANTITY_OFFSET];
        item_slots[index] = (spec_gbc_item_slot_t){.item = slot[0], .quantity = quantity};
    }
}

void spec_gbc_decode_items(spec_gbc_save_t *save, const uint8_t *data,
                           const spec_gbc_layout_t *layout) {
    for (size_t pocket = 0; pocket < SPEC_GBC_POCKET_COUNT; ++pocket) {
        const uint8_t *bytes = &data[pocket_offset(layout, (spec_gbc_pocket_t)pocket)];
        if (pocket == SPEC_GBC_POCKET_TMS_HMS) {
            decode_machines(save->items[pocket], bytes);
        } else {
            decode_list(save->items[pocket], bytes, (spec_gbc_pocket_t)pocket);
        }
    }
}

static void encode_machines(uint8_t *quantities, const spec_gbc_item_slot_t *item_slots) {
    memset(quantities, 0, SPEC_GBC_MACHINE_COUNT);
    for (size_t index = 0; index < SPEC_GBC_POCKET_MAX_CAPACITY; ++index) {
        if (!spec_is_item_slot_empty(&item_slots[index])) {
            uint8_t machine = spec_gbc_items[item_slots[index].item].machine;
            quantities[machine - 1] = (uint8_t)item_slots[index].quantity;
        }
    }
}

static void encode_list(uint8_t *bytes, const spec_gbc_item_slot_t *item_slots,
                        spec_gbc_pocket_t pocket) {
    spec_gbc_item_slot_t condensed[SPEC_GBC_POCKET_MAX_CAPACITY];
    spec_condense_pocket(condensed, item_slots, SPEC_GBC_POCKET_MAX_CAPACITY);
    size_t count = spec_count_filled_slots(condensed, SPEC_GBC_POCKET_MAX_CAPACITY);
    size_t slot_size = slot_size_of(pocket);
    bytes[0] = (uint8_t)count;
    for (size_t index = 0; index < count; ++index) {
        uint8_t *slot = &bytes[FIRST_SLOT_OFFSET + index * slot_size];
        slot[0] = (uint8_t)condensed[index].item;
        if (slot_size != 1) {
            slot[ITEM_QUANTITY_OFFSET] = (uint8_t)condensed[index].quantity;
        }
    }
    bytes[FIRST_SLOT_OFFSET + count * slot_size] = SPEC_GB_END_OF_LIST;
}

void spec_gbc_encode_items(uint8_t *data, const spec_gbc_layout_t *layout,
                           const spec_gbc_save_t *save) {
    for (size_t pocket = 0; pocket < SPEC_GBC_POCKET_COUNT; ++pocket) {
        uint8_t *bytes = &data[pocket_offset(layout, (spec_gbc_pocket_t)pocket)];
        if (pocket == SPEC_GBC_POCKET_TMS_HMS) {
            encode_machines(bytes, save->items[pocket]);
        } else {
            encode_list(bytes, save->items[pocket], (spec_gbc_pocket_t)pocket);
        }
    }
}

static spec_error_t check_quantity(spec_gbc_pocket_t pocket,
                                   const spec_gbc_item_slot_t *item_slot) {
    if (pocket == SPEC_GBC_POCKET_KEY_ITEMS && item_slot->quantity != 1) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "a key item's quantity is 1");
    }
    if (item_slot->quantity > UINT8_MAX) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "the quantity does not fit in a byte");
    }
    return SPEC_OK;
}

static spec_error_t check_pocket(const spec_gbc_item_slot_t *item_slots, spec_gbc_pocket_t pocket,
                                 spec_game_type_t type) {
    bool is_machine_filled[SPEC_GBC_MACHINE_COUNT + 1] = {};
    size_t filled_slot_count = 0;
    for (size_t index = 0; index < SPEC_GBC_POCKET_MAX_CAPACITY; ++index) {
        const spec_gbc_item_slot_t *item_slot = &item_slots[index];
        if (spec_is_item_slot_empty(item_slot)) {
            continue;
        }
        if (filled_slot_count == POCKET_CAPACITIES[pocket]) {
            (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                            "the pocket holds more items than the game allows");
            return spec_locate_error(SPEC_ERROR_LOCATION_ITEMS, pocket, (uint32_t)index);
        }
        if (spec_gbc_check_item_placement(type, pocket, item_slot->item) != SPEC_OK
            || check_quantity(pocket, item_slot) != SPEC_OK) {
            return spec_locate_error(SPEC_ERROR_LOCATION_ITEMS, pocket, (uint32_t)index);
        }
        if (pocket == SPEC_GBC_POCKET_TMS_HMS) {
            uint8_t machine = spec_gbc_items[item_slot->item].machine;
            if (is_machine_filled[machine]) {
                (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "the machine fills two slots");
                return spec_locate_error(SPEC_ERROR_LOCATION_ITEMS, pocket, (uint32_t)index);
            }
            is_machine_filled[machine] = true;
        }
        ++filled_slot_count;
    }
    return SPEC_OK;
}

spec_error_t spec_gbc_check_items(const spec_gbc_save_t *save) {
    for (size_t pocket = 0; pocket < SPEC_GBC_POCKET_COUNT; ++pocket) {
        spec_error_t error =
            check_pocket(save->items[pocket], (spec_gbc_pocket_t)pocket, save->type);
        if (error != SPEC_OK) {
            return error;
        }
    }
    return SPEC_OK;
}

bool spec_gbc_is_mail(uint16_t item) {
    const spec_gbc_item_data_t *item_data = item_data_of(item);
    return item_data != nullptr && item_data->is_mail;
}

// Every item's menu lets the PC take it (pret PlayerDepositItemMenu).
spec_error_t spec_gbc_check_item_placement(spec_game_type_t type, spec_gbc_pocket_t pocket,
                                           uint16_t item) {
    if (pocket >= SPEC_GBC_POCKET_COUNT) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "pocket is not a Gen 2 pocket");
    }
    if (item == 0) {
        return SPEC_OK;
    }
    const spec_gbc_item_data_t *item_data = item_data_of(item);
    if (!is_in_game(item_data, type)) {
        return spec_fail(SPEC_ERROR_INVALID_ITEM, "the item is not in this game");
    }
    if (pocket != SPEC_GBC_POCKET_PC && pocket_in(item_data, type) != pocket) {
        return spec_fail(SPEC_ERROR_WRONG_POCKET, "the item belongs in another pocket");
    }
    return SPEC_OK;
}

spec_error_t spec_gbc_get_pocket_for_item(spec_gbc_pocket_t *pocket, spec_game_type_t type,
                                          uint16_t item) {
    const spec_gbc_item_data_t *item_data = item_data_of(item);
    if (!is_in_game(item_data, type)) {
        return spec_fail(SPEC_ERROR_INVALID_ITEM, "the item is not in this game");
    }
    *pocket = (spec_gbc_pocket_t)pocket_in(item_data, type);
    return SPEC_OK;
}

// TODO: Item names for other languages; pret's localized builds have them.
const char *spec_gbc_item_name(uint16_t item, spec_language_t language) {
    const spec_gbc_item_data_t *item_data = item_data_of(item);
    if (item_data == nullptr || language != SPEC_LANGUAGE_ENGLISH) {
        return nullptr;
    }
    return item_data->english_name;
}

size_t spec_gbc_pocket_capacity(spec_gbc_pocket_t pocket) {
    if (pocket >= SPEC_GBC_POCKET_COUNT) {
        return 0;
    }
    return POCKET_CAPACITIES[pocket];
}

size_t spec_gbc_pocket_item_count(const spec_gbc_save_t *save, spec_gbc_pocket_t pocket) {
    if (pocket >= SPEC_GBC_POCKET_COUNT) {
        return 0;
    }
    return spec_count_filled_slots(save->items[pocket], SPEC_GBC_POCKET_MAX_CAPACITY);
}
