// Where Gen 1 saves keep their fields: Japan's layout and everyone else's (pret wram.asm).

#include "gb/gb_internal.h"

constexpr spec_gb_layout_t INTERNATIONAL_LAYOUT = {
    .name_size = SPEC_GB_NAME_SIZE,
    .box_count = 12,
    .boxes_per_bank = 6,
    .party_shape = {SPEC_GB_PARTY_CAPACITY, SPEC_GB_PARTY_RECORD_SIZE, SPEC_GB_NAME_SIZE},
    .box_shape = {20, SPEC_GB_BOX_RECORD_SIZE, SPEC_GB_NAME_SIZE},
    .has_box_checksums = true,
    .does_bank_checksum_sum_itself = false,

    .pokedex_caught_offset = 0x25A3,
    .pokedex_seen_offset = 0x25B6,
    .bag_offset = 0x25C9,
    .money_offset = 0x25F3,
    .rival_name_offset = 0x25F6,
    .badges_offset = 0x2602,
    .trainer_id_offset = 0x2605,
    .pikachu_friendship_offset = 0x271C,
    .pc_items_offset = 0x27E6,
    .current_box_offset = 0x284C,
    .coins_offset = 0x2850,
    .player_starter_offset = 0x29C3,
    .event_flags_offset = 0x29F3,
    .play_time_offset = 0x2CED,
    .daycare_offset = 0x2CF4,
    .party_offset = 0x2F2C,
    .current_box_list_offset = 0x30C0,
    .checksum_offset = 0x3523,
};

constexpr spec_gb_layout_t JAPANESE_LAYOUT = {
    .name_size = SPEC_GB_JAPANESE_NAME_SIZE,
    .box_count = 8,
    .boxes_per_bank = 4,
    .party_shape = {SPEC_GB_PARTY_CAPACITY, SPEC_GB_PARTY_RECORD_SIZE, SPEC_GB_JAPANESE_NAME_SIZE},
    .box_shape = {30, SPEC_GB_BOX_RECORD_SIZE, SPEC_GB_JAPANESE_NAME_SIZE},
    .has_box_checksums = false,
    // pret: "BUG: the checked data area contains the Checksum byte".
    .does_bank_checksum_sum_itself = true,

    .pokedex_caught_offset = 0x259E,
    .pokedex_seen_offset = 0x25B1,
    .bag_offset = 0x25C4,
    .money_offset = 0x25EE,
    .rival_name_offset = 0x25F1,
    .badges_offset = 0x25F8,
    .trainer_id_offset = 0x25FB,
    .pikachu_friendship_offset = 0x2712,
    .pc_items_offset = 0x27DC,
    .current_box_offset = 0x2842,
    .coins_offset = 0x2846,
    .player_starter_offset = 0x29B9,
    .event_flags_offset = 0x29E9,
    .play_time_offset = 0x2CA0,
    .daycare_offset = 0x2CA7,
    .party_offset = 0x2ED5,
    .current_box_list_offset = 0x302D,
    .checksum_offset = 0x3594,
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
