// Gen 4 items: item data, pocket rules and the bag codec.

#include "nds/nds.h"
#include "nds/nds_internal.h"
#include "nds/tables.h"
#include "spec_internal.h"

constexpr size_t ITEM_SLOT_SIZE = 4;
constexpr size_t ITEM_QUANTITY_OFFSET = 2;

constexpr size_t DIAMOND_PEARL_PLATINUM_POCKET_CAPACITIES[SPEC_NDS_POCKET_COUNT] = {
    [SPEC_NDS_POCKET_ITEMS] = 165,       [SPEC_NDS_POCKET_MEDICINE] = 40,
    [SPEC_NDS_POCKET_POKE_BALLS] = 15,   [SPEC_NDS_POCKET_TMS_HMS] = 100,
    [SPEC_NDS_POCKET_BERRIES] = 64,      [SPEC_NDS_POCKET_MAIL] = 12,
    [SPEC_NDS_POCKET_BATTLE_ITEMS] = 30, [SPEC_NDS_POCKET_KEY_ITEMS] = 50,
};

constexpr size_t HEARTGOLD_SOULSILVER_POCKET_CAPACITIES[SPEC_NDS_POCKET_COUNT] = {
    [SPEC_NDS_POCKET_ITEMS] = 165,       [SPEC_NDS_POCKET_MEDICINE] = 40,
    [SPEC_NDS_POCKET_POKE_BALLS] = 24,   [SPEC_NDS_POCKET_TMS_HMS] = 101,
    [SPEC_NDS_POCKET_BERRIES] = 64,      [SPEC_NDS_POCKET_MAIL] = 12,
    [SPEC_NDS_POCKET_BATTLE_ITEMS] = 30, [SPEC_NDS_POCKET_KEY_ITEMS] = 50,
};

static const spec_nds_item_data_t *item_data_of(uint16_t item) {
    if (item >= SPEC_NDS_ITEM_COUNT || spec_nds_items[item].english_name == nullptr) {
        return nullptr;
    }
    return &spec_nds_items[item];
}

static uint8_t pocket_in(const spec_nds_item_data_t *item_data, spec_game_type_t type) {
    switch (type) {
        case SPEC_GAME_TYPE_DIAMOND_PEARL:
            return item_data->diamond_pearl_pocket;
        case SPEC_GAME_TYPE_PLATINUM:
            return item_data->platinum_pocket;
        case SPEC_GAME_TYPE_HEARTGOLD_SOULSILVER:
            return item_data->heartgold_soulsilver_pocket;
        default:
            return SPEC_NDS_NO_POCKET;
    }
}

static bool is_in_game(const spec_nds_item_data_t *item_data, spec_game_type_t type) {
    return item_data != nullptr && pocket_in(item_data, type) != SPEC_NDS_NO_POCKET;
}

static size_t item_slot_offset(const spec_nds_layout_t *layout, spec_nds_pocket_t pocket,
                               size_t index) {
    return layout->pocket_offsets[pocket] + index * ITEM_SLOT_SIZE;
}

void spec_nds_decode_items(spec_nds_save_t *save, const uint8_t *general,
                           const spec_nds_layout_t *layout) {
    for (size_t pocket = 0; pocket < SPEC_NDS_POCKET_COUNT; ++pocket) {
        size_t capacity = spec_nds_pocket_capacity(layout->type, (spec_nds_pocket_t)pocket);
        for (size_t index = 0; index < capacity; ++index) {
            size_t offset = item_slot_offset(layout, (spec_nds_pocket_t)pocket, index);
            save->items[pocket][index] = (spec_nds_item_slot_t){
                .item = spec_read_u16_le(&general[offset]),
                .quantity = spec_read_u16_le(&general[offset + ITEM_QUANTITY_OFFSET]),
            };
        }
    }
}

static void write_item_slot(uint8_t *general, size_t offset,
                            const spec_nds_item_slot_t *item_slot) {
    spec_write_u16_le(&general[offset], item_slot->item);
    spec_write_u16_le(&general[offset + ITEM_QUANTITY_OFFSET], item_slot->quantity);
}

static void encode_pocket(uint8_t *general, const spec_nds_layout_t *layout,
                          spec_nds_pocket_t pocket, const spec_nds_item_slot_t *item_slots) {
    spec_nds_item_slot_t condensed[SPEC_NDS_POCKET_MAX_CAPACITY];
    spec_condense_pocket(condensed, item_slots, SPEC_NDS_POCKET_MAX_CAPACITY);
    size_t capacity = spec_nds_pocket_capacity(layout->type, pocket);
    for (size_t index = 0; index < capacity; ++index) {
        write_item_slot(general, item_slot_offset(layout, pocket, index), &condensed[index]);
    }
}

void spec_nds_encode_items(uint8_t *general, const spec_nds_layout_t *layout,
                           const spec_nds_save_t *save) {
    for (size_t pocket = 0; pocket < SPEC_NDS_POCKET_COUNT; ++pocket) {
        encode_pocket(general, layout, (spec_nds_pocket_t)pocket, save->items[pocket]);
    }
}

static spec_error_t check_pocket(const spec_nds_item_slot_t *item_slots, spec_nds_pocket_t pocket,
                                 const spec_nds_layout_t *layout) {
    size_t capacity = spec_nds_pocket_capacity(layout->type, pocket);
    size_t filled_slot_count = 0;
    for (size_t index = 0; index < SPEC_NDS_POCKET_MAX_CAPACITY; ++index) {
        if (spec_is_item_slot_empty(&item_slots[index])) {
            continue;
        }
        if (filled_slot_count == capacity) {
            (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                            "the pocket holds more items than this game allows");
            return spec_locate_error(SPEC_ERROR_LOCATION_ITEMS, pocket, (uint32_t)index);
        }
        if (spec_nds_check_item_placement(layout->type, pocket, item_slots[index].item)
            != SPEC_OK) {
            return spec_locate_error(SPEC_ERROR_LOCATION_ITEMS, pocket, (uint32_t)index);
        }
        ++filled_slot_count;
    }
    return SPEC_OK;
}

spec_error_t spec_nds_check_items(const spec_nds_save_t *save, const spec_nds_layout_t *layout) {
    for (size_t pocket = 0; pocket < SPEC_NDS_POCKET_COUNT; ++pocket) {
        spec_error_t error = check_pocket(save->items[pocket], (spec_nds_pocket_t)pocket, layout);
        if (error != SPEC_OK) {
            return error;
        }
    }
    return SPEC_OK;
}

spec_error_t spec_nds_check_item_placement(spec_game_type_t type, spec_nds_pocket_t pocket,
                                           uint16_t item) {
    if (item == 0) {
        return SPEC_OK;
    }
    const spec_nds_item_data_t *item_data = item_data_of(item);
    if (!is_in_game(item_data, type)) {
        return spec_fail(SPEC_ERROR_INVALID_ITEM, "the item is not in this game");
    }
    if (pocket_in(item_data, type) != pocket) {
        return spec_fail(SPEC_ERROR_WRONG_POCKET, "the item belongs in another pocket");
    }
    return SPEC_OK;
}

spec_error_t spec_nds_get_pocket_for_item(spec_nds_pocket_t *pocket, spec_game_type_t type,
                                          uint16_t item) {
    const spec_nds_item_data_t *item_data = item_data_of(item);
    if (!is_in_game(item_data, type)) {
        return spec_fail(SPEC_ERROR_INVALID_ITEM, "the item is not in this game");
    }
    *pocket = (spec_nds_pocket_t)pocket_in(item_data, type);
    return SPEC_OK;
}

// The English names are the game's own; the other languages take the 3DS games' names, whose
// numbers are the same.
const char *spec_nds_item_name(uint16_t item, spec_language_t language) {
    const spec_nds_item_data_t *item_data = item_data_of(item);
    if (item_data == nullptr) {
        return nullptr;
    }
    if (language == SPEC_LANGUAGE_ENGLISH) {
        return item_data->english_name;
    }
    return spec_item_name(item, language);
}

static const size_t *pocket_capacities_of(spec_game_type_t type) {
    switch (type) {
        case SPEC_GAME_TYPE_DIAMOND_PEARL:
        case SPEC_GAME_TYPE_PLATINUM:
            return DIAMOND_PEARL_PLATINUM_POCKET_CAPACITIES;
        case SPEC_GAME_TYPE_HEARTGOLD_SOULSILVER:
            return HEARTGOLD_SOULSILVER_POCKET_CAPACITIES;
        default:
            return nullptr;
    }
}

size_t spec_nds_pocket_capacity(spec_game_type_t type, spec_nds_pocket_t pocket) {
    const size_t *capacities = pocket_capacities_of(type);
    if (capacities == nullptr || pocket >= SPEC_NDS_POCKET_COUNT) {
        return 0;
    }
    return capacities[pocket];
}

size_t spec_nds_pocket_item_count(const spec_nds_save_t *save, spec_nds_pocket_t pocket) {
    if (pocket >= SPEC_NDS_POCKET_COUNT) {
        return 0;
    }
    return spec_count_filled_slots(save->items[pocket], SPEC_NDS_POCKET_MAX_CAPACITY);
}
