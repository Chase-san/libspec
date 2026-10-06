// Gen 5 items: item data, pocket rules and the bag codec.

#include "ndsi/ndsi.h"
#include "ndsi/ndsi_internal.h"
#include "ndsi/tables.h"
#include "spec_internal.h"

constexpr size_t ITEM_SLOT_SIZE = 4;
constexpr size_t ITEM_QUANTITY_OFFSET = 2;

// Bag-relative, in the order the bag stores them.
constexpr size_t POCKET_OFFSETS[SPEC_NDSI_POCKET_COUNT] = {
    [SPEC_NDSI_POCKET_ITEMS] = 0x000,   [SPEC_NDSI_POCKET_KEY_ITEMS] = 0x4D8,
    [SPEC_NDSI_POCKET_TMS_HMS] = 0x624, [SPEC_NDSI_POCKET_MEDICINE] = 0x7D8,
    [SPEC_NDSI_POCKET_BERRIES] = 0x898,
};

constexpr size_t POCKET_CAPACITIES[SPEC_NDSI_POCKET_COUNT] = {
    [SPEC_NDSI_POCKET_ITEMS] = 310,   [SPEC_NDSI_POCKET_KEY_ITEMS] = 83,
    [SPEC_NDSI_POCKET_TMS_HMS] = 109, [SPEC_NDSI_POCKET_MEDICINE] = 48,
    [SPEC_NDSI_POCKET_BERRIES] = 64,
};

static const spec_ndsi_item_data_t *item_data_of(uint16_t item) {
    if (item >= SPEC_NDSI_ITEM_COUNT || spec_ndsi_items[item].english_name == nullptr) {
        return nullptr;
    }
    return &spec_ndsi_items[item];
}

static uint8_t pocket_in(const spec_ndsi_item_data_t *item_data, spec_game_type_t type) {
    switch (type) {
        case SPEC_GAME_TYPE_BLACK_WHITE:
            return item_data->black_white_pocket;
        case SPEC_GAME_TYPE_BLACK2_WHITE2:
            return item_data->black2_white2_pocket;
        default:
            return SPEC_NDSI_NO_POCKET;
    }
}

static bool is_in_game(const spec_ndsi_item_data_t *item_data, spec_game_type_t type) {
    return item_data != nullptr && pocket_in(item_data, type) != SPEC_NDSI_NO_POCKET;
}

static size_t item_slot_offset(const spec_ndsi_layout_t *layout, spec_ndsi_pocket_t pocket,
                               size_t index) {
    return layout->bag_offset + POCKET_OFFSETS[pocket] + index * ITEM_SLOT_SIZE;
}

void spec_ndsi_decode_items(spec_ndsi_save_t *save, const uint8_t *copy,
                            const spec_ndsi_layout_t *layout) {
    for (size_t pocket = 0; pocket < SPEC_NDSI_POCKET_COUNT; ++pocket) {
        for (size_t index = 0; index < POCKET_CAPACITIES[pocket]; ++index) {
            size_t offset = item_slot_offset(layout, (spec_ndsi_pocket_t)pocket, index);
            save->items[pocket][index] = (spec_ndsi_item_slot_t){
                .item = spec_read_u16_le(&copy[offset]),
                .quantity = spec_read_u16_le(&copy[offset + ITEM_QUANTITY_OFFSET]),
            };
        }
    }
}

static void write_item_slot(uint8_t *copy, size_t offset, const spec_ndsi_item_slot_t *item_slot) {
    spec_write_u16_le(&copy[offset], item_slot->item);
    spec_write_u16_le(&copy[offset + ITEM_QUANTITY_OFFSET], item_slot->quantity);
}

static void encode_pocket(uint8_t *copy, const spec_ndsi_layout_t *layout,
                          spec_ndsi_pocket_t pocket, const spec_ndsi_item_slot_t *item_slots) {
    spec_ndsi_item_slot_t condensed[SPEC_NDSI_POCKET_MAX_CAPACITY];
    spec_condense_pocket(condensed, item_slots, SPEC_NDSI_POCKET_MAX_CAPACITY);
    for (size_t index = 0; index < POCKET_CAPACITIES[pocket]; ++index) {
        write_item_slot(copy, item_slot_offset(layout, pocket, index), &condensed[index]);
    }
}

void spec_ndsi_encode_items(uint8_t *copy, const spec_ndsi_layout_t *layout,
                            const spec_ndsi_save_t *save) {
    for (size_t pocket = 0; pocket < SPEC_NDSI_POCKET_COUNT; ++pocket) {
        encode_pocket(copy, layout, (spec_ndsi_pocket_t)pocket, save->items[pocket]);
    }
}

static spec_error_t check_pocket(const spec_ndsi_item_slot_t *item_slots, spec_ndsi_pocket_t pocket,
                                 spec_game_type_t type) {
    size_t filled_slot_count = 0;
    for (size_t index = 0; index < SPEC_NDSI_POCKET_MAX_CAPACITY; ++index) {
        if (spec_is_item_slot_empty(&item_slots[index])) {
            continue;
        }
        if (filled_slot_count == POCKET_CAPACITIES[pocket]) {
            (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                            "the pocket holds more items than this game allows");
            return spec_locate_error(SPEC_ERROR_LOCATION_ITEMS, pocket, (uint32_t)index);
        }
        if (spec_ndsi_check_item_placement(type, pocket, item_slots[index].item) != SPEC_OK) {
            return spec_locate_error(SPEC_ERROR_LOCATION_ITEMS, pocket, (uint32_t)index);
        }
        ++filled_slot_count;
    }
    return SPEC_OK;
}

spec_error_t spec_ndsi_check_items(const spec_ndsi_save_t *save) {
    for (size_t pocket = 0; pocket < SPEC_NDSI_POCKET_COUNT; ++pocket) {
        spec_error_t error =
            check_pocket(save->items[pocket], (spec_ndsi_pocket_t)pocket, save->type);
        if (error != SPEC_OK) {
            return error;
        }
    }
    return SPEC_OK;
}

spec_error_t spec_ndsi_check_item_placement(spec_game_type_t type, spec_ndsi_pocket_t pocket,
                                            uint16_t item) {
    if (item == 0) {
        return SPEC_OK;
    }
    const spec_ndsi_item_data_t *item_data = item_data_of(item);
    if (!is_in_game(item_data, type)) {
        return spec_fail(SPEC_ERROR_INVALID_ITEM, "the item is not in this game");
    }
    if (pocket_in(item_data, type) != pocket) {
        return spec_fail(SPEC_ERROR_WRONG_POCKET, "the item belongs in another pocket");
    }
    return SPEC_OK;
}

spec_error_t spec_ndsi_get_pocket_for_item(spec_ndsi_pocket_t *pocket, spec_game_type_t type,
                                           uint16_t item) {
    const spec_ndsi_item_data_t *item_data = item_data_of(item);
    if (!is_in_game(item_data, type)) {
        return spec_fail(SPEC_ERROR_INVALID_ITEM, "the item is not in this game");
    }
    *pocket = (spec_ndsi_pocket_t)pocket_in(item_data, type);
    return SPEC_OK;
}

// The English names are the game's own; the other languages take the 3DS games' names, whose
// numbers are the same.
const char *spec_ndsi_item_name(uint16_t item, spec_language_t language) {
    const spec_ndsi_item_data_t *item_data = item_data_of(item);
    if (item_data == nullptr) {
        return nullptr;
    }
    if (language == SPEC_LANGUAGE_ENGLISH) {
        return item_data->english_name;
    }
    return spec_item_name(item, language);
}

static bool is_ndsi_game(spec_game_type_t type) {
    return type == SPEC_GAME_TYPE_BLACK_WHITE || type == SPEC_GAME_TYPE_BLACK2_WHITE2;
}

// The two pairs' bags hold the same pockets.
size_t spec_ndsi_pocket_capacity(spec_game_type_t type, spec_ndsi_pocket_t pocket) {
    if (!is_ndsi_game(type) || pocket >= SPEC_NDSI_POCKET_COUNT) {
        return 0;
    }
    return POCKET_CAPACITIES[pocket];
}

size_t spec_ndsi_pocket_item_count(const spec_ndsi_save_t *save, spec_ndsi_pocket_t pocket) {
    if (pocket >= SPEC_NDSI_POCKET_COUNT) {
        return 0;
    }
    return spec_count_filled_slots(save->items[pocket], SPEC_NDSI_POCKET_MAX_CAPACITY);
}
