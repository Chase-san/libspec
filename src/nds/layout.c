// Where each Gen 4 game keeps its fields: Diamond and Pearl, Platinum, HeartGold and SoulSilver.

#include "nds/nds_internal.h"

constexpr size_t DIAMOND_PEARL_BAG = 0x624;
constexpr size_t PLATINUM_BAG = 0x630;
constexpr size_t HEARTGOLD_SOULSILVER_BAG = 0x644;

constexpr spec_nds_layout_t DIAMOND_PEARL_LAYOUT = {
    .type = SPEC_GAME_TYPE_DIAMOND_PEARL,
    .general_size = 0xC100,
    .storage_offset = 0xC100,
    .storage_size = 0x121E0,
    .footer_size = 0x14,
    .are_blocks_paired = false,

    .player_offset = 0x60,
    .has_kanto_badges = false,
    .rival_name_offset = 0x25A8,
    .battle_points_offset = 0x65F8,
    .pokedex =
        {
            .offset = 0x12DC,
            .records_every_species_language = false,
            .languages_offset = 0x129,
            .form_view_offset = 0x128,
            .language_view_offset = 0x137,
            .obtained_offset = 0x138,
            .national_dex_offset = 0x139,
            .has_platinum_forms = false,
            .has_heartgold_soulsilver_forms = false,
        },

    .party_offset = 0x90,
    .daycare_offset = 0x141C,
    .current_box_offset = 0x0,
    .box_records_offset = 0x4,
    .box_stride = 0xFF0,
    .box_names_offset = 0x11EE4,
    .wallpapers_offset = 0x121B4,
    .wallpaper_count = 24,
    .has_box_modified_flags = false,

    .pocket_offsets =
        {
            [SPEC_NDS_POCKET_ITEMS] = DIAMOND_PEARL_BAG + 0x0,
            [SPEC_NDS_POCKET_MEDICINE] = DIAMOND_PEARL_BAG + 0x51C,
            [SPEC_NDS_POCKET_POKE_BALLS] = DIAMOND_PEARL_BAG + 0x6BC,
            [SPEC_NDS_POCKET_TMS_HMS] = DIAMOND_PEARL_BAG + 0x35C,
            [SPEC_NDS_POCKET_BERRIES] = DIAMOND_PEARL_BAG + 0x5BC,
            [SPEC_NDS_POCKET_MAIL] = DIAMOND_PEARL_BAG + 0x4EC,
            [SPEC_NDS_POCKET_BATTLE_ITEMS] = DIAMOND_PEARL_BAG + 0x6F8,
            [SPEC_NDS_POCKET_KEY_ITEMS] = DIAMOND_PEARL_BAG + 0x294,
        },
};

constexpr spec_nds_layout_t PLATINUM_LAYOUT = {
    .type = SPEC_GAME_TYPE_PLATINUM,
    .general_size = 0xCF2C,
    .storage_offset = 0xCF2C,
    .storage_size = 0x121E4,
    .footer_size = 0x14,
    .are_blocks_paired = false,

    .player_offset = 0x64,
    .has_kanto_badges = false,
    .rival_name_offset = 0x27E8,
    .battle_points_offset = 0x7234,
    .pokedex =
        {
            .offset = 0x1328,
            .records_every_species_language = true,
            .languages_offset = 0x128,
            .form_view_offset = 0x318,
            .language_view_offset = 0x319,
            .obtained_offset = 0x31A,
            .national_dex_offset = 0x31B,
            .has_platinum_forms = true,
            .rotom_offset = 0x31C,
            .shaymin_offset = 0x320,
            .giratina_offset = 0x321,
            .has_heartgold_soulsilver_forms = false,
        },

    .party_offset = 0x98,
    .daycare_offset = 0x1654,
    .current_box_offset = 0x0,
    .box_records_offset = 0x4,
    .box_stride = 0xFF0,
    .box_names_offset = 0x11EE4,
    .wallpapers_offset = 0x121B4,
    .wallpaper_count = 32,
    .has_box_modified_flags = false,

    .pocket_offsets =
        {
            [SPEC_NDS_POCKET_ITEMS] = PLATINUM_BAG + 0x0,
            [SPEC_NDS_POCKET_MEDICINE] = PLATINUM_BAG + 0x51C,
            [SPEC_NDS_POCKET_POKE_BALLS] = PLATINUM_BAG + 0x6BC,
            [SPEC_NDS_POCKET_TMS_HMS] = PLATINUM_BAG + 0x35C,
            [SPEC_NDS_POCKET_BERRIES] = PLATINUM_BAG + 0x5BC,
            [SPEC_NDS_POCKET_MAIL] = PLATINUM_BAG + 0x4EC,
            [SPEC_NDS_POCKET_BATTLE_ITEMS] = PLATINUM_BAG + 0x6F8,
            [SPEC_NDS_POCKET_KEY_ITEMS] = PLATINUM_BAG + 0x294,
        },
};

constexpr spec_nds_layout_t HEARTGOLD_SOULSILVER_LAYOUT = {
    .type = SPEC_GAME_TYPE_HEARTGOLD_SOULSILVER,
    .general_size = 0xF628,
    .storage_offset = 0xF700,
    .storage_size = 0x12310,
    .footer_size = 0x10,
    .are_blocks_paired = true,

    .player_offset = 0x60,
    .has_kanto_badges = true,
    .rival_name_offset = 0x22D4,
    .battle_points_offset = 0x5BB8,
    .pokedex =
        {
            .offset = 0x12B8,
            .records_every_species_language = true,
            .languages_offset = 0x144,
            .form_view_offset = 0x334,
            .language_view_offset = 0x335,
            .obtained_offset = 0x336,
            .national_dex_offset = 0x337,
            .has_platinum_forms = true,
            .rotom_offset = 0x338,
            .shaymin_offset = 0x33C,
            .giratina_offset = 0x33D,
            .has_heartgold_soulsilver_forms = true,
            .unown_caught_offset = 0x128,
            .pichu_offset = 0x33E,
        },

    .party_offset = 0x90,
    .daycare_offset = 0x15FC,
    .current_box_offset = 0x12000,
    .box_records_offset = 0x0,
    .box_stride = 0x1000,
    .box_names_offset = 0x12008,
    .wallpapers_offset = 0x122D8,
    .wallpaper_count = 40,
    .has_box_modified_flags = true,
    .box_modified_flags_offset = 0x12004,

    .pocket_offsets =
        {
            [SPEC_NDS_POCKET_ITEMS] = HEARTGOLD_SOULSILVER_BAG + 0x0,
            [SPEC_NDS_POCKET_MEDICINE] = HEARTGOLD_SOULSILVER_BAG + 0x520,
            [SPEC_NDS_POCKET_POKE_BALLS] = HEARTGOLD_SOULSILVER_BAG + 0x6C0,
            [SPEC_NDS_POCKET_TMS_HMS] = HEARTGOLD_SOULSILVER_BAG + 0x35C,
            [SPEC_NDS_POCKET_BERRIES] = HEARTGOLD_SOULSILVER_BAG + 0x5C0,
            [SPEC_NDS_POCKET_MAIL] = HEARTGOLD_SOULSILVER_BAG + 0x4F0,
            [SPEC_NDS_POCKET_BATTLE_ITEMS] = HEARTGOLD_SOULSILVER_BAG + 0x720,
            [SPEC_NDS_POCKET_KEY_ITEMS] = HEARTGOLD_SOULSILVER_BAG + 0x294,
        },
};

const spec_nds_layout_t *spec_nds_get_layout(spec_game_type_t type) {
    switch (type) {
        case SPEC_GAME_TYPE_DIAMOND_PEARL:
            return &DIAMOND_PEARL_LAYOUT;
        case SPEC_GAME_TYPE_PLATINUM:
            return &PLATINUM_LAYOUT;
        case SPEC_GAME_TYPE_HEARTGOLD_SOULSILVER:
            return &HEARTGOLD_SOULSILVER_LAYOUT;
        default:
            return nullptr;
    }
}
