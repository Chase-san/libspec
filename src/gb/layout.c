// Where Gen 1 saves keep their fields: Japan's layout and everyone else's.

#include "gb/gb_internal.h"

constexpr spec_gb_layout_t INTERNATIONAL_LAYOUT = {
    .name_size = SPEC_GB_NAME_SIZE,
    .box_count = 12,
    .boxes_per_bank = 6,
    .party_shape = {SPEC_GB_PARTY_CAPACITY, SPEC_GB_PARTY_RECORD_SIZE, SPEC_GB_NAME_SIZE},
    .box_shape = {20, SPEC_GB_BOX_RECORD_SIZE, SPEC_GB_NAME_SIZE},
    .has_box_checksums = true,
    .does_bank_checksum_sum_itself = false,

    .pokedex_caught_offset = 0x25A3,     // pret/pokered sMainData wPokedexOwned
    .pokedex_seen_offset = 0x25B6,       // pret/pokered sMainData wPokedexSeen
    .bag_offset = 0x25C9,                // pret/pokered sMainData wNumBagItems
    .money_offset = 0x25F3,              // pret/pokered sMainData wPlayerMoney
    .rival_name_offset = 0x25F6,         // pret/pokered sMainData wRivalName
    .badges_offset = 0x2602,             // pret/pokered sMainData wObtainedBadges
    .trainer_id_offset = 0x2605,         // pret/pokered sMainData wPlayerID
    .pikachu_friendship_offset = 0x271C, // pret/pokeyellow sMainData wPikachuHappiness
    .pc_items_offset = 0x27E6,           // pret/pokered sMainData wNumBoxItems
    .current_box_offset = 0x284C,        // pret/pokered sMainData wCurrentBoxNum
    .coins_offset = 0x2850,              // pret/pokered sMainData wPlayerCoins
    .player_starter_offset = 0x29C3,     // pret/pokered sMainData wPlayerStarter
    .event_flags_offset = 0x29F3,        // pret/pokered sMainData wEventFlags
    .play_time_offset = 0x2CED,          // pret/pokered sMainData wPlayTimeHours
    .daycare_offset = 0x2CF4,            // pret/pokered sMainData wDayCareInUse
    .party_offset = 0x2F2C,              // pret/pokered sPartyData wPartyCount
    .current_box_list_offset = 0x30C0,   // pret/pokered sCurBoxData wBoxCount
    .checksum_offset = 0x3523,           // pret/pokered sMainDataCheckSum
};

constexpr spec_gb_layout_t JAPANESE_LAYOUT = {
    .name_size = SPEC_GB_JAPANESE_NAME_SIZE,
    .box_count = 8,
    .boxes_per_bank = 4,
    .party_shape = {SPEC_GB_PARTY_CAPACITY, SPEC_GB_PARTY_RECORD_SIZE, SPEC_GB_JAPANESE_NAME_SIZE},
    .box_shape = {30, SPEC_GB_BOX_RECORD_SIZE, SPEC_GB_JAPANESE_NAME_SIZE},
    .has_box_checksums = false,
    // Narishma-gb/pokegreen: "BUG: the checked data area contains the Checksum byte".
    .does_bank_checksum_sum_itself = true,

    .pokedex_caught_offset = 0x259E,     // Narishma-gb/pokegreen sMainData wPokedexOwned
    .pokedex_seen_offset = 0x25B1,       // Narishma-gb/pokegreen sMainData wPokedexSeen
    .bag_offset = 0x25C4,                // Narishma-gb/pokegreen sMainData wNumBagItems
    .money_offset = 0x25EE,              // Narishma-gb/pokegreen sMainData wPlayerMoney
    .rival_name_offset = 0x25F1,         // Narishma-gb/pokegreen sMainData wRivalName
    .badges_offset = 0x25F8,             // Narishma-gb/pokegreen sMainData wObtainedBadges
    .trainer_id_offset = 0x25FB,         // Narishma-gb/pokegreen sMainData wPlayerID
    .pikachu_friendship_offset = 0x2712, // Narishma-gb/pokeyellow-jp sMainData wPikachuHappiness
    .pc_items_offset = 0x27DC,           // Narishma-gb/pokegreen sMainData wNumBoxItems
    .current_box_offset = 0x2842,        // Narishma-gb/pokegreen sMainData wCurrentBoxNum
    .coins_offset = 0x2846,              // Narishma-gb/pokegreen sMainData wPlayerCoins
    .player_starter_offset = 0x29B9,     // Narishma-gb/pokegreen sMainData wPlayerStarter
    .event_flags_offset = 0x29E9,        // Narishma-gb/pokegreen sMainData wEventFlags
    .play_time_offset = 0x2CA0,          // Narishma-gb/pokegreen sMainData wPlayTimeHours
    .daycare_offset = 0x2CA7,            // Narishma-gb/pokegreen sMainData wDayCareInUse
    .party_offset = 0x2ED5,              // Narishma-gb/pokegreen sPartyData wPartyCount
    .current_box_list_offset = 0x302D,   // Narishma-gb/pokegreen sCurBoxData wBoxCount
    .checksum_offset = 0x3594,           // Narishma-gb/pokegreen sMainDataCheckSum
};

const spec_gb_layout_t *spec_gb_get_layout(spec_language_t language) {
    switch (language) {
        case SPEC_LANGUAGE_JAPANESE:
            return &JAPANESE_LAYOUT;
        case SPEC_LANGUAGE_ENGLISH:
        case SPEC_LANGUAGE_FRENCH:
        case SPEC_LANGUAGE_ITALIAN:
        case SPEC_LANGUAGE_GERMAN:
        case SPEC_LANGUAGE_SPANISH:
            return &INTERNATIONAL_LAYOUT;
        default:
            return nullptr;
    }
}
