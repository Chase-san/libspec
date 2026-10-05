// Gen 3 items: item data, pocket rules, and the bag and PC codec.

#include "gba/gba.h"
#include "gba/gba_internal.h"
#include "gba/tables.h"
#include "spec_internal.h"

constexpr size_t ITEM_SLOT_SIZE = 4;
constexpr size_t ITEM_QUANTITY_OFFSET = 2;

constexpr size_t RUBY_SAPPHIRE_POCKET_CAPACITIES[SPEC_GBA_POCKET_COUNT] = {
    [SPEC_GBA_POCKET_ITEMS] = 20,      [SPEC_GBA_POCKET_KEY_ITEMS] = 20,
    [SPEC_GBA_POCKET_POKE_BALLS] = 16, [SPEC_GBA_POCKET_TMS_HMS] = 64,
    [SPEC_GBA_POCKET_BERRIES] = 46,    [SPEC_GBA_POCKET_PC] = 50,
};

constexpr size_t EMERALD_POCKET_CAPACITIES[SPEC_GBA_POCKET_COUNT] = {
    [SPEC_GBA_POCKET_ITEMS] = 30,      [SPEC_GBA_POCKET_KEY_ITEMS] = 30,
    [SPEC_GBA_POCKET_POKE_BALLS] = 16, [SPEC_GBA_POCKET_TMS_HMS] = 64,
    [SPEC_GBA_POCKET_BERRIES] = 46,    [SPEC_GBA_POCKET_PC] = 50,
};

constexpr size_t FIRERED_LEAFGREEN_POCKET_CAPACITIES[SPEC_GBA_POCKET_COUNT] = {
    [SPEC_GBA_POCKET_ITEMS] = 42,      [SPEC_GBA_POCKET_KEY_ITEMS] = 30,
    [SPEC_GBA_POCKET_POKE_BALLS] = 13, [SPEC_GBA_POCKET_TMS_HMS] = 58,
    [SPEC_GBA_POCKET_BERRIES] = 43,    [SPEC_GBA_POCKET_PC] = 30,
};

static const size_t *pocket_capacities_of(spec_game_type_t type) {
    switch (type) {
        case SPEC_GAME_TYPE_RUBY_SAPPHIRE:
            return RUBY_SAPPHIRE_POCKET_CAPACITIES;
        case SPEC_GAME_TYPE_EMERALD:
            return EMERALD_POCKET_CAPACITIES;
        case SPEC_GAME_TYPE_FIRERED_LEAFGREEN:
            return FIRERED_LEAFGREEN_POCKET_CAPACITIES;
        default:
            return nullptr;
    }
}

static const spec_gba_item_data_t *item_data_of(uint16_t item) {
    if (item >= SPEC_GBA_ITEM_COUNT || spec_gba_items[item].english_name == nullptr) {
        return nullptr;
    }
    return &spec_gba_items[item];
}

static uint8_t pocket_in(const spec_gba_item_data_t *item_data, spec_game_type_t type) {
    switch (type) {
        case SPEC_GAME_TYPE_RUBY_SAPPHIRE:
            return item_data->ruby_sapphire_pocket;
        case SPEC_GAME_TYPE_EMERALD:
            return item_data->emerald_pocket;
        case SPEC_GAME_TYPE_FIRERED_LEAFGREEN:
            return item_data->firered_leafgreen_pocket;
        default:
            return SPEC_GBA_NO_POCKET;
    }
}

static bool is_in_game(const spec_gba_item_data_t *item_data, spec_game_type_t type) {
    return item_data != nullptr && pocket_in(item_data, type) != SPEC_GBA_NO_POCKET;
}

spec_error_t spec_gba_check_item_placement(spec_game_type_t type, spec_gba_pocket_t pocket,
                                           uint16_t item) {
    if (item == 0) {
        return SPEC_OK;
    }
    const spec_gba_item_data_t *item_data = item_data_of(item);
    if (!is_in_game(item_data, type)) {
        return spec_fail(SPEC_ERROR_INVALID_ITEM, "the item is not in this game");
    }
    if (pocket == SPEC_GBA_POCKET_PC) {
        if (item_data->is_important) {
            return spec_fail(SPEC_ERROR_WRONG_POCKET, "the PC cannot store important items");
        }
        return SPEC_OK;
    }
    if (pocket_in(item_data, type) != pocket) {
        return spec_fail(SPEC_ERROR_WRONG_POCKET, "the item belongs in another pocket");
    }
    return SPEC_OK;
}

// The security key hides the bag's quantities with its low half, but not the PC's.
static uint16_t quantity_key_of(uint32_t security_key, spec_gba_pocket_t pocket) {
    return pocket == SPEC_GBA_POCKET_PC ? 0 : (uint16_t)security_key;
}

static size_t item_slot_offset(const spec_gba_layout_t *layout, spec_gba_pocket_t pocket,
                               size_t index) {
    return layout->pocket_offsets[pocket] + index * ITEM_SLOT_SIZE;
}

static void write_item_slot(uint8_t *data, const spec_gba_slot_t *slot, size_t offset,
                            const spec_gba_item_slot_t *item_slot, uint16_t quantity_key) {
    spec_gba_write_slot_u16(data, slot, offset, item_slot->item);
    spec_gba_write_slot_u16(data, slot, offset + ITEM_QUANTITY_OFFSET,
                            (uint16_t)(item_slot->quantity ^ quantity_key));
}

static spec_error_t check_pocket(const spec_gba_item_slot_t *item_slots, spec_gba_pocket_t pocket,
                                 const spec_gba_layout_t *layout) {
    size_t capacity = spec_gba_pocket_capacity(layout->type, pocket);
    size_t filled_slot_count = 0;
    for (size_t index = 0; index < SPEC_GBA_POCKET_MAX_CAPACITY; ++index) {
        if (spec_is_item_slot_empty(&item_slots[index])) {
            continue;
        }
        if (filled_slot_count == capacity) {
            (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                            "the pocket holds more items than this game allows");
            return spec_locate_error(SPEC_ERROR_LOCATION_ITEMS, pocket, (uint32_t)index);
        }
        if (spec_gba_check_item_placement(layout->type, pocket, item_slots[index].item)
            != SPEC_OK) {
            return spec_locate_error(SPEC_ERROR_LOCATION_ITEMS, pocket, (uint32_t)index);
        }
        ++filled_slot_count;
    }
    return SPEC_OK;
}

static void encode_pocket(uint8_t *data, const spec_gba_slot_t *slot,
                          const spec_gba_layout_t *layout, spec_gba_pocket_t pocket,
                          const spec_gba_item_slot_t *item_slots, uint16_t quantity_key) {
    spec_gba_item_slot_t condensed[SPEC_GBA_POCKET_MAX_CAPACITY];
    spec_condense_pocket(condensed, item_slots, SPEC_GBA_POCKET_MAX_CAPACITY);
    size_t capacity = spec_gba_pocket_capacity(layout->type, pocket);
    for (size_t index = 0; index < capacity; ++index) {
        write_item_slot(data, slot, item_slot_offset(layout, pocket, index), &condensed[index],
                        quantity_key);
    }
}

void spec_gba_decode_items(spec_gba_save_t *save, const uint8_t *data, const spec_gba_slot_t *slot,
                           const spec_gba_layout_t *layout) {
    uint32_t security_key = spec_gba_read_security_key(data, slot, layout);
    for (size_t pocket = 0; pocket < SPEC_GBA_POCKET_COUNT; ++pocket) {
        uint16_t quantity_key = quantity_key_of(security_key, (spec_gba_pocket_t)pocket);
        size_t capacity = spec_gba_pocket_capacity(layout->type, (spec_gba_pocket_t)pocket);
        for (size_t index = 0; index < capacity; ++index) {
            size_t offset = item_slot_offset(layout, (spec_gba_pocket_t)pocket, index);
            spec_gba_item_slot_t *item_slot = &save->items[pocket][index];
            item_slot->item = spec_gba_read_slot_u16(data, slot, offset);
            item_slot->quantity =
                (uint16_t)(spec_gba_read_slot_u16(data, slot, offset + ITEM_QUANTITY_OFFSET)
                           ^ quantity_key);
        }
    }
}

spec_error_t spec_gba_check_items(const spec_gba_save_t *save, const spec_gba_layout_t *layout) {
    for (size_t pocket = 0; pocket < SPEC_GBA_POCKET_COUNT; ++pocket) {
        spec_error_t error = check_pocket(save->items[pocket], (spec_gba_pocket_t)pocket, layout);
        if (error != SPEC_OK) {
            return error;
        }
    }
    return SPEC_OK;
}

void spec_gba_encode_items(uint8_t *data, const spec_gba_slot_t *slot,
                           const spec_gba_layout_t *layout, const spec_gba_save_t *save) {
    uint32_t security_key = spec_gba_read_security_key(data, slot, layout);
    for (size_t pocket = 0; pocket < SPEC_GBA_POCKET_COUNT; ++pocket) {
        encode_pocket(data, slot, layout, (spec_gba_pocket_t)pocket, save->items[pocket],
                      quantity_key_of(security_key, (spec_gba_pocket_t)pocket));
    }
}

// TODO: Item names for other languages; pret has only the English and German builds.
const char *spec_gba_item_name(uint16_t item, spec_language_t language) {
    const spec_gba_item_data_t *item_data = item_data_of(item);
    if (item_data == nullptr) {
        return nullptr;
    }
    switch (language) {
        case SPEC_LANGUAGE_ENGLISH:
            return item_data->english_name;
        case SPEC_LANGUAGE_GERMAN:
            return item_data->german_name;
        default:
            return nullptr;
    }
}

spec_error_t spec_gba_get_pocket_for_item(spec_gba_pocket_t *pocket, spec_game_type_t type,
                                          uint16_t item) {
    const spec_gba_item_data_t *item_data = item_data_of(item);
    if (!is_in_game(item_data, type)) {
        return spec_fail(SPEC_ERROR_INVALID_ITEM, "the item is not in this game");
    }
    *pocket = (spec_gba_pocket_t)pocket_in(item_data, type);
    return SPEC_OK;
}

size_t spec_gba_pocket_capacity(spec_game_type_t type, spec_gba_pocket_t pocket) {
    const size_t *capacities = pocket_capacities_of(type);
    if (capacities == nullptr || pocket >= SPEC_GBA_POCKET_COUNT) {
        return 0;
    }
    return capacities[pocket];
}

size_t spec_gba_pocket_item_count(const spec_gba_save_t *save, spec_gba_pocket_t pocket) {
    if (pocket >= SPEC_GBA_POCKET_COUNT) {
        return 0;
    }
    return spec_count_filled_slots(save->items[pocket], SPEC_GBA_POCKET_MAX_CAPACITY);
}
