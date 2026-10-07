// Each game type's move data, and the PP that PP Ups give.

#include "spec.h"
#include "spec_tables.h"

constexpr uint8_t MAX_PP_UPS = 3;
constexpr uint8_t PP_UP_SHARE = 5;
constexpr uint8_t GAME_BOY_MAX_PP_UP_BONUS = 7;

// Game types count up generation by generation, so the latest row begun by the type is in force.
const spec_move_data_t *spec_get_move_data(spec_game_type_t type, uint16_t move) {
    const spec_move_data_row_t *latest = nullptr;
    for (size_t index = 0; index < spec_move_data_row_count; ++index) {
        const spec_move_data_row_t *row = &spec_move_data_rows[index];
        bool applies = row->move == move && row->from_game <= type;
        if (applies && (latest == nullptr || row->from_game >= latest->from_game)) {
            latest = row;
        }
    }
    if (latest == nullptr) {
        return nullptr;
    }
    return &latest->data;
}

static bool is_game_boy_game(spec_game_type_t type) {
    return type <= SPEC_GAME_TYPE_CRYSTAL;
}

// Each PP Up adds a fifth of the base PP. Gen 1 and 2 round each fifth down and cap it at 7, which
// keeps 40 PP from passing 61 (pret AddBonusPP, ComputeMaxPP); later games round the whole bonus
// down (pret CalculatePPWithBonus).
uint8_t spec_move_max_pp(spec_game_type_t type, uint16_t move, uint8_t pp_ups) {
    const spec_move_data_t *move_data = spec_get_move_data(type, move);
    if (move_data == nullptr || pp_ups > MAX_PP_UPS) {
        return 0;
    }
    if (is_game_boy_game(type)) {
        uint8_t bonus = move_data->pp / PP_UP_SHARE;
        if (bonus > GAME_BOY_MAX_PP_UP_BONUS) {
            bonus = GAME_BOY_MAX_PP_UP_BONUS;
        }
        return (uint8_t)(move_data->pp + bonus * pp_ups);
    }
    return (uint8_t)(move_data->pp + move_data->pp * pp_ups / PP_UP_SHARE);
}
