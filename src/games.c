// The highest species, move and ability numbers each game type uses.

#include "spec.h"
#include "spec_tables.h"

uint16_t spec_last_ability(spec_game_type_t type) {
    if (type >= SPEC_GAME_TYPE_COUNT) {
        return 0;
    }
    return spec_game_limits[type].last_ability;
}

uint16_t spec_last_move(spec_game_type_t type) {
    if (type >= SPEC_GAME_TYPE_COUNT) {
        return 0;
    }
    return spec_game_limits[type].last_move;
}

uint16_t spec_last_species(spec_game_type_t type) {
    if (type >= SPEC_GAME_TYPE_COUNT) {
        return 0;
    }
    return spec_game_limits[type].last_species;
}
