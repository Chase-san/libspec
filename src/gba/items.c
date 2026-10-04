#include "gba/gba.h"
#include "gba/gba_internal.h"
#include "gba/tables.h"
#include "spec_internal.h"

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
    }
    return nullptr;
}

static const spec_gba_item_data_t *item_data_of(uint16_t item) {
    if (item >= SPEC_GBA_ITEM_COUNT || spec_gba_items[item].name == nullptr) {
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
    }
    return SPEC_GBA_NO_POCKET;
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

// The game clears the item when its quantity reaches zero.
bool spec_gba_is_item_slot_empty(const spec_gba_item_slot_t *item_slot) {
    return item_slot->item == 0 || item_slot->quantity == 0;
}

const char *spec_gba_item_name(uint16_t item) {
    const spec_gba_item_data_t *item_data = item_data_of(item);
    return item_data == nullptr ? nullptr : item_data->name;
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
    size_t item_count = 0;
    for (size_t index = 0; index < SPEC_GBA_POCKET_MAX_CAPACITY; ++index) {
        if (!spec_gba_is_item_slot_empty(&save->items[pocket][index])) {
            ++item_count;
        }
    }
    return item_count;
}
