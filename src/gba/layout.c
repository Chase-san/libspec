// Where each Gen 3 game keeps its fields: Ruby and Sapphire, Emerald, FireRed and LeafGreen.

#include "gba/gba_internal.h"

constexpr size_t SAVE_BLOCK_2 = 0 * SPEC_GBA_SECTION_DATA_SIZE;
constexpr size_t SAVE_BLOCK_1 = 1 * SPEC_GBA_SECTION_DATA_SIZE;
constexpr size_t STORAGE = 5 * SPEC_GBA_SECTION_DATA_SIZE;

constexpr spec_gba_layout_t RUBY_SAPPHIRE_LAYOUT = {
    .type = SPEC_GAME_TYPE_RUBY_SAPPHIRE,
    .section_sizes = {0x890, 0xF80, 0xF80, 0xF80, 0xC40, 0xF80, 0xF80, 0xF80, 0xF80, 0xF80, 0xF80,
                      0xF80, 0xF80, 0x7D0},

    .trainer_name_offset = SAVE_BLOCK_2 + 0x0,
    .trainer_gender_offset = SAVE_BLOCK_2 + 0x8,
    .trainer_id_offset = SAVE_BLOCK_2 + 0xA,
    .secret_id_offset = SAVE_BLOCK_2 + 0xC,
    .play_time_offset = SAVE_BLOCK_2 + 0xE,
    .button_mode_offset = SAVE_BLOCK_2 + 0x13,
    .options_offset = SAVE_BLOCK_2 + 0x14,
    .window_frame_count = 20,
    .has_security_key = false,
    .money_offset = SAVE_BLOCK_1 + 0x490,
    .coins_offset = SAVE_BLOCK_1 + 0x494,
    .has_battle_points = false,
    .has_rival_name = false,
    .flags_offset = SAVE_BLOCK_1 + 0x1220,
    .first_badge_flag = 0x807,

    .pokedex_flag = 0x801,
    .pokedex_order_offset = SAVE_BLOCK_2 + 0x18,
    .pokedex_mode_offset = SAVE_BLOCK_2 + 0x19,
    .unown_personality_offset = SAVE_BLOCK_2 + 0x1C,
    .spinda_personality_offset = SAVE_BLOCK_2 + 0x20,
    .pokedex_caught_offset = SAVE_BLOCK_2 + 0x28,
    .pokedex_seen_offsets = {SAVE_BLOCK_2 + 0x5C, SAVE_BLOCK_1 + 0x938, SAVE_BLOCK_1 + 0x3A8C},
    .national_dex = {SAVE_BLOCK_2 + 0x1A, 0xDA, SAVE_BLOCK_1 + 0x13CC, 0x302, 0x836, true},

    .party_count_offset = SAVE_BLOCK_1 + 0x234,
    .party_offset = SAVE_BLOCK_1 + 0x238,
    .current_box_offset = STORAGE + 0x0,
    .box_records_offset = STORAGE + 0x4,
    .box_names_offset = STORAGE + 0x8344,
    .wallpapers_offset = STORAGE + 0x83C2,
    .wallpaper_count = 16,
    .daycare =
        {
            .slot_count = 2,
            .record_offsets = {SAVE_BLOCK_1 + 0x2F9C, SAVE_BLOCK_1 + 0x2FEC},
            .steps_offsets = {SAVE_BLOCK_1 + 0x30AC, SAVE_BLOCK_1 + 0x30B0},
            .egg_waiting_flag = 0x86,
            .egg_personality_offset = SAVE_BLOCK_1 + 0x30B4,
            .egg_personality_size = 2,
            .step_counter_offset = SAVE_BLOCK_1 + 0x30B6,
        },

    .pocket_offsets =
        {
            [SPEC_GBA_POCKET_ITEMS] = SAVE_BLOCK_1 + 0x560,
            [SPEC_GBA_POCKET_KEY_ITEMS] = SAVE_BLOCK_1 + 0x5B0,
            [SPEC_GBA_POCKET_POKE_BALLS] = SAVE_BLOCK_1 + 0x600,
            [SPEC_GBA_POCKET_TMS_HMS] = SAVE_BLOCK_1 + 0x640,
            [SPEC_GBA_POCKET_BERRIES] = SAVE_BLOCK_1 + 0x740,
            [SPEC_GBA_POCKET_PC] = SAVE_BLOCK_1 + 0x498,
        },
};

constexpr spec_gba_layout_t EMERALD_LAYOUT = {
    .type = SPEC_GAME_TYPE_EMERALD,
    .section_sizes = {0xF2C, 0xF80, 0xF80, 0xF80, 0xF08, 0xF80, 0xF80, 0xF80, 0xF80, 0xF80, 0xF80,
                      0xF80, 0xF80, 0x7D0},

    .trainer_name_offset = SAVE_BLOCK_2 + 0x0,
    .trainer_gender_offset = SAVE_BLOCK_2 + 0x8,
    .trainer_id_offset = SAVE_BLOCK_2 + 0xA,
    .secret_id_offset = SAVE_BLOCK_2 + 0xC,
    .play_time_offset = SAVE_BLOCK_2 + 0xE,
    .button_mode_offset = SAVE_BLOCK_2 + 0x13,
    .options_offset = SAVE_BLOCK_2 + 0x14,
    .window_frame_count = 20,
    .has_security_key = true,
    .security_key_offset = SAVE_BLOCK_2 + 0xAC,
    .money_offset = SAVE_BLOCK_1 + 0x490,
    .coins_offset = SAVE_BLOCK_1 + 0x494,
    .has_battle_points = true,
    .battle_points_offset = SAVE_BLOCK_2 + 0xEB8,
    .has_rival_name = false,
    .flags_offset = SAVE_BLOCK_1 + 0x1270,
    .first_badge_flag = 0x867,

    .pokedex_flag = 0x861,
    .pokedex_order_offset = SAVE_BLOCK_2 + 0x18,
    .pokedex_mode_offset = SAVE_BLOCK_2 + 0x19,
    .unown_personality_offset = SAVE_BLOCK_2 + 0x1C,
    .spinda_personality_offset = SAVE_BLOCK_2 + 0x20,
    .pokedex_caught_offset = SAVE_BLOCK_2 + 0x28,
    .pokedex_seen_offsets = {SAVE_BLOCK_2 + 0x5C, SAVE_BLOCK_1 + 0x988, SAVE_BLOCK_1 + 0x3B24},
    .national_dex = {SAVE_BLOCK_2 + 0x1A, 0xDA, SAVE_BLOCK_1 + 0x1428, 0x302, 0x896, true},

    .party_count_offset = SAVE_BLOCK_1 + 0x234,
    .party_offset = SAVE_BLOCK_1 + 0x238,
    .current_box_offset = STORAGE + 0x0,
    .box_records_offset = STORAGE + 0x4,
    .box_names_offset = STORAGE + 0x8344,
    .wallpapers_offset = STORAGE + 0x83C2,
    .wallpaper_count = 17,
    .daycare =
        {
            .slot_count = 2,
            .record_offsets = {SAVE_BLOCK_1 + 0x3030, SAVE_BLOCK_1 + 0x30BC},
            .steps_offsets = {SAVE_BLOCK_1 + 0x30B8, SAVE_BLOCK_1 + 0x3144},
            .egg_waiting_flag = 0x86,
            .egg_personality_offset = SAVE_BLOCK_1 + 0x3148,
            .egg_personality_size = 4,
            .step_counter_offset = SAVE_BLOCK_1 + 0x314C,
        },

    .pocket_offsets =
        {
            [SPEC_GBA_POCKET_ITEMS] = SAVE_BLOCK_1 + 0x560,
            [SPEC_GBA_POCKET_KEY_ITEMS] = SAVE_BLOCK_1 + 0x5D8,
            [SPEC_GBA_POCKET_POKE_BALLS] = SAVE_BLOCK_1 + 0x650,
            [SPEC_GBA_POCKET_TMS_HMS] = SAVE_BLOCK_1 + 0x690,
            [SPEC_GBA_POCKET_BERRIES] = SAVE_BLOCK_1 + 0x790,
            [SPEC_GBA_POCKET_PC] = SAVE_BLOCK_1 + 0x498,
        },
};

constexpr spec_gba_layout_t FIRERED_LEAFGREEN_LAYOUT = {
    .type = SPEC_GAME_TYPE_FIRERED_LEAFGREEN,
    .section_sizes = {0xF24, 0xF80, 0xF80, 0xF80, 0xEE8, 0xF80, 0xF80, 0xF80, 0xF80, 0xF80, 0xF80,
                      0xF80, 0xF80, 0x7D0},

    .trainer_name_offset = SAVE_BLOCK_2 + 0x0,
    .trainer_gender_offset = SAVE_BLOCK_2 + 0x8,
    .trainer_id_offset = SAVE_BLOCK_2 + 0xA,
    .secret_id_offset = SAVE_BLOCK_2 + 0xC,
    .play_time_offset = SAVE_BLOCK_2 + 0xE,
    .button_mode_offset = SAVE_BLOCK_2 + 0x13,
    .options_offset = SAVE_BLOCK_2 + 0x14,
    .window_frame_count = 10,
    .has_security_key = true,
    .security_key_offset = SAVE_BLOCK_2 + 0xF20,
    .money_offset = SAVE_BLOCK_1 + 0x290,
    .coins_offset = SAVE_BLOCK_1 + 0x294,
    .has_battle_points = false,
    .has_rival_name = true,
    .rival_name_offset = SAVE_BLOCK_1 + 0x3A4C,
    .flags_offset = SAVE_BLOCK_1 + 0xEE0,
    .first_badge_flag = 0x820,

    .pokedex_flag = 0x829,
    .pokedex_order_offset = SAVE_BLOCK_2 + 0x18,
    .pokedex_mode_offset = SAVE_BLOCK_2 + 0x19,
    .unown_personality_offset = SAVE_BLOCK_2 + 0x1C,
    .spinda_personality_offset = SAVE_BLOCK_2 + 0x20,
    .pokedex_caught_offset = SAVE_BLOCK_2 + 0x28,
    .pokedex_seen_offsets = {SAVE_BLOCK_2 + 0x5C, SAVE_BLOCK_1 + 0x5F8, SAVE_BLOCK_1 + 0x3A18},
    .national_dex = {SAVE_BLOCK_2 + 0x1B, 0xB9, SAVE_BLOCK_1 + 0x109C, 0x6258, 0x840, false},

    .party_count_offset = SAVE_BLOCK_1 + 0x34,
    .party_offset = SAVE_BLOCK_1 + 0x38,
    .current_box_offset = STORAGE + 0x0,
    .box_records_offset = STORAGE + 0x4,
    .box_names_offset = STORAGE + 0x8344,
    .wallpapers_offset = STORAGE + 0x83C2,
    .wallpaper_count = 16,
    .daycare =
        {
            .slot_count = 3,
            .record_offsets = {SAVE_BLOCK_1 + 0x2F80, SAVE_BLOCK_1 + 0x300C, SAVE_BLOCK_1 + 0x3C98},
            .steps_offsets = {SAVE_BLOCK_1 + 0x3008, SAVE_BLOCK_1 + 0x3094, SAVE_BLOCK_1 + 0x3D20},
            .egg_waiting_flag = 0x266,
            .egg_personality_offset = SAVE_BLOCK_1 + 0x3098,
            .egg_personality_size = 2,
            .step_counter_offset = SAVE_BLOCK_1 + 0x309A,
        },

    .pocket_offsets =
        {
            [SPEC_GBA_POCKET_ITEMS] = SAVE_BLOCK_1 + 0x310,
            [SPEC_GBA_POCKET_KEY_ITEMS] = SAVE_BLOCK_1 + 0x3B8,
            [SPEC_GBA_POCKET_POKE_BALLS] = SAVE_BLOCK_1 + 0x430,
            [SPEC_GBA_POCKET_TMS_HMS] = SAVE_BLOCK_1 + 0x464,
            [SPEC_GBA_POCKET_BERRIES] = SAVE_BLOCK_1 + 0x54C,
            [SPEC_GBA_POCKET_PC] = SAVE_BLOCK_1 + 0x298,
        },
};

const spec_gba_layout_t *spec_gba_get_layout(spec_game_type_t type) {
    switch (type) {
        case SPEC_GAME_TYPE_RUBY_SAPPHIRE:
            return &RUBY_SAPPHIRE_LAYOUT;
        case SPEC_GAME_TYPE_EMERALD:
            return &EMERALD_LAYOUT;
        case SPEC_GAME_TYPE_FIRERED_LEAFGREEN:
            return &FIRERED_LEAFGREEN_LAYOUT;
        default:
            return nullptr;
    }
}
