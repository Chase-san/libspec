// Where each Gen 3 game keeps its fields: Ruby and Sapphire, Emerald, FireRed and LeafGreen.

#include "gba/gba_internal.h"

// pret/pokeemerald SECTOR_ID_SAVEBLOCK2
constexpr size_t SAVE_BLOCK_2 = 0 * SPEC_GBA_SECTION_DATA_SIZE;
// pret/pokeemerald SECTOR_ID_SAVEBLOCK1_START
constexpr size_t SAVE_BLOCK_1 = 1 * SPEC_GBA_SECTION_DATA_SIZE;
// pret/pokeemerald SECTOR_ID_PKMN_STORAGE_START
constexpr size_t STORAGE = 5 * SPEC_GBA_SECTION_DATA_SIZE;

constexpr spec_gba_layout_t RUBY_SAPPHIRE_LAYOUT = {
    .type = SPEC_GAME_TYPE_RUBY_SAPPHIRE,
    // pret/pokeruby sSaveBlockChunks
    .section_sizes = {0x890, 0xF80, 0xF80, 0xF80, 0xC40, 0xF80, 0xF80, 0xF80, 0xF80, 0xF80, 0xF80,
                      0xF80, 0xF80, 0x7D0},

    .trainer_name_offset = SAVE_BLOCK_2 + 0x0,   // pret/pokeruby SaveBlock2 playerName
    .trainer_gender_offset = SAVE_BLOCK_2 + 0x8, // pret/pokeruby SaveBlock2 playerGender
    .trainer_id_offset = SAVE_BLOCK_2 + 0xA,     // pret/pokeruby SaveBlock2 playerTrainerId
    .secret_id_offset = SAVE_BLOCK_2 + 0xC,      // pret/pokeruby SaveBlock2 playerTrainerId[2]
    .play_time_offset = SAVE_BLOCK_2 + 0xE,      // pret/pokeruby SaveBlock2 playTimeHours
    .button_mode_offset = SAVE_BLOCK_2 + 0x13,   // pret/pokeruby SaveBlock2 optionsButtonMode
    .options_offset = SAVE_BLOCK_2 + 0x14,       // pret/pokeruby SaveBlock2 optionsTextSpeed
    .window_frame_count = 20,
    .has_security_key = false,
    .money_offset = SAVE_BLOCK_1 + 0x490, // pret/pokeruby SaveBlock1 money
    .coins_offset = SAVE_BLOCK_1 + 0x494, // pret/pokeruby SaveBlock1 coins
    .has_battle_points = false,
    .has_rival_name = false,
    .flags_offset = SAVE_BLOCK_1 + 0x1220, // pret/pokeruby SaveBlock1 flags
    .first_badge_flag = 0x807,             // pret/pokeruby FLAG_BADGE01_GET

    .pokedex_flag = 0x801,                       // pret/pokeruby FLAG_SYS_POKEDEX_GET
    .pokedex_order_offset = SAVE_BLOCK_2 + 0x18, // pret/pokeruby SaveBlock2 pokedex.order
    .pokedex_mode_offset = SAVE_BLOCK_2 + 0x19,  // pret/pokeruby SaveBlock2 pokedex.mode
    // pret/pokeruby SaveBlock2 pokedex.unownPersonality
    .unown_personality_offset = SAVE_BLOCK_2 + 0x1C,
    // pret/pokeruby SaveBlock2 pokedex.spindaPersonality
    .spinda_personality_offset = SAVE_BLOCK_2 + 0x20,
    .pokedex_caught_offset = SAVE_BLOCK_2 + 0x28, // pret/pokeruby SaveBlock2 pokedex.owned
    .pokedex_seen_offsets =
        {
            SAVE_BLOCK_2 + 0x5C,   // pret/pokeruby SaveBlock2 pokedex.seen
            SAVE_BLOCK_1 + 0x938,  // pret/pokeruby SaveBlock1 dexSeen2
            SAVE_BLOCK_1 + 0x3A8C, // pret/pokeruby SaveBlock1 dexSeen3
        },
    .national_dex =
        {
            SAVE_BLOCK_2 + 0x1A,   // pret/pokeruby SaveBlock2 pokedex.nationalMagic
            0xDA,                  // pret/pokeruby EnableNationalPokedex
            SAVE_BLOCK_1 + 0x13CC, // pret/pokeruby SaveBlock1 vars VAR_NATIONAL_DEX
            0x302,                 // pret/pokeruby EnableNationalPokedex
            0x836,                 // pret/pokeruby FLAG_SYS_NATIONAL_DEX
            true,                  // pret/pokeruby EnableNationalPokedex
        },

    .party_count_offset = SAVE_BLOCK_1 + 0x234, // pret/pokeruby SaveBlock1 playerPartyCount
    .party_offset = SAVE_BLOCK_1 + 0x238,       // pret/pokeruby SaveBlock1 playerParty
    .current_box_offset = STORAGE + 0x0,        // pret/pokeruby PokemonStorage currentBox
    .box_records_offset = STORAGE + 0x4,        // pret/pokeruby PokemonStorage boxes
    .box_names_offset = STORAGE + 0x8344,       // pret/pokeruby PokemonStorage boxNames
    .wallpapers_offset = STORAGE + 0x83C2,      // pret/pokeruby PokemonStorage wallpaper
    .wallpaper_count = 16,
    .daycare =
        {
            .slot_count = 2,
            .record_offsets =
                {
                    SAVE_BLOCK_1 + 0x2F9C, // pret/pokeruby SaveBlock1 daycare.mons[0]
                    SAVE_BLOCK_1 + 0x2FEC, // pret/pokeruby SaveBlock1 daycare.mons[1]
                },
            .steps_offsets =
                {
                    // pret/pokeruby SaveBlock1 daycare.misc.countersEtc.steps[0]
                    SAVE_BLOCK_1 + 0x30AC,
                    // pret/pokeruby SaveBlock1 daycare.misc.countersEtc.steps[1]
                    SAVE_BLOCK_1 + 0x30B0,
                },
            .egg_waiting_flag = 0x86, // pret/pokeruby FLAG_PENDING_DAYCARE_EGG
            // pret/pokeruby SaveBlock1 daycare.misc.countersEtc.pendingEggPersonality
            .egg_personality_offset = SAVE_BLOCK_1 + 0x30B4,
            .egg_personality_size = 2,
            // pret/pokeruby SaveBlock1 daycare.misc.countersEtc.eggCycleStepsRemaining
            .step_counter_offset = SAVE_BLOCK_1 + 0x30B6,
        },

    .pocket_offsets =
        {
            // pret/pokeruby SaveBlock1 bagPocket_Items
            [SPEC_GBA_POCKET_ITEMS] = SAVE_BLOCK_1 + 0x560,
            // pret/pokeruby SaveBlock1 bagPocket_KeyItems
            [SPEC_GBA_POCKET_KEY_ITEMS] = SAVE_BLOCK_1 + 0x5B0,
            // pret/pokeruby SaveBlock1 bagPocket_PokeBalls
            [SPEC_GBA_POCKET_POKE_BALLS] = SAVE_BLOCK_1 + 0x600,
            // pret/pokeruby SaveBlock1 bagPocket_TMHM
            [SPEC_GBA_POCKET_TMS_HMS] = SAVE_BLOCK_1 + 0x640,
            // pret/pokeruby SaveBlock1 bagPocket_Berries
            [SPEC_GBA_POCKET_BERRIES] = SAVE_BLOCK_1 + 0x740,
            // pret/pokeruby SaveBlock1 pcItems
            [SPEC_GBA_POCKET_PC] = SAVE_BLOCK_1 + 0x498,
        },
};

constexpr spec_gba_layout_t EMERALD_LAYOUT = {
    .type = SPEC_GAME_TYPE_EMERALD,
    // pret/pokeemerald sSaveSlotLayout
    .section_sizes = {0xF2C, 0xF80, 0xF80, 0xF80, 0xF08, 0xF80, 0xF80, 0xF80, 0xF80, 0xF80, 0xF80,
                      0xF80, 0xF80, 0x7D0},

    .trainer_name_offset = SAVE_BLOCK_2 + 0x0,   // pret/pokeemerald SaveBlock2 playerName
    .trainer_gender_offset = SAVE_BLOCK_2 + 0x8, // pret/pokeemerald SaveBlock2 playerGender
    .trainer_id_offset = SAVE_BLOCK_2 + 0xA,     // pret/pokeemerald SaveBlock2 playerTrainerId
    .secret_id_offset = SAVE_BLOCK_2 + 0xC,      // pret/pokeemerald SaveBlock2 playerTrainerId[2]
    .play_time_offset = SAVE_BLOCK_2 + 0xE,      // pret/pokeemerald SaveBlock2 playTimeHours
    .button_mode_offset = SAVE_BLOCK_2 + 0x13,   // pret/pokeemerald SaveBlock2 optionsButtonMode
    .options_offset = SAVE_BLOCK_2 + 0x14,       // pret/pokeemerald SaveBlock2 optionsTextSpeed
    .window_frame_count = 20,
    .has_security_key = true,
    .security_key_offset = SAVE_BLOCK_2 + 0xAC, // pret/pokeemerald SaveBlock2 encryptionKey
    .money_offset = SAVE_BLOCK_1 + 0x490,       // pret/pokeemerald SaveBlock1 money
    .coins_offset = SAVE_BLOCK_1 + 0x494,       // pret/pokeemerald SaveBlock1 coins
    .has_battle_points = true,
    // pret/pokeemerald SaveBlock2 frontier.battlePoints
    .battle_points_offset = SAVE_BLOCK_2 + 0xEB8,
    .has_rival_name = false,
    .flags_offset = SAVE_BLOCK_1 + 0x1270, // pret/pokeemerald SaveBlock1 flags
    .first_badge_flag = 0x867,             // pret/pokeemerald FLAG_BADGE01_GET

    .pokedex_flag = 0x861,                       // pret/pokeemerald FLAG_SYS_POKEDEX_GET
    .pokedex_order_offset = SAVE_BLOCK_2 + 0x18, // pret/pokeemerald SaveBlock2 pokedex.order
    .pokedex_mode_offset = SAVE_BLOCK_2 + 0x19,  // pret/pokeemerald SaveBlock2 pokedex.mode
    // pret/pokeemerald SaveBlock2 pokedex.unownPersonality
    .unown_personality_offset = SAVE_BLOCK_2 + 0x1C,
    // pret/pokeemerald SaveBlock2 pokedex.spindaPersonality
    .spinda_personality_offset = SAVE_BLOCK_2 + 0x20,
    .pokedex_caught_offset = SAVE_BLOCK_2 + 0x28, // pret/pokeemerald SaveBlock2 pokedex.owned
    .pokedex_seen_offsets =
        {
            SAVE_BLOCK_2 + 0x5C,   // pret/pokeemerald SaveBlock2 pokedex.seen
            SAVE_BLOCK_1 + 0x988,  // pret/pokeemerald SaveBlock1 seen1
            SAVE_BLOCK_1 + 0x3B24, // pret/pokeemerald SaveBlock1 seen2
        },
    .national_dex =
        {
            SAVE_BLOCK_2 + 0x1A,   // pret/pokeemerald SaveBlock2 pokedex.nationalMagic
            0xDA,                  // pret/pokeemerald EnableNationalPokedex
            SAVE_BLOCK_1 + 0x1428, // pret/pokeemerald SaveBlock1 vars VAR_NATIONAL_DEX
            0x302,                 // pret/pokeemerald EnableNationalPokedex
            0x896,                 // pret/pokeemerald FLAG_SYS_NATIONAL_DEX
            true,                  // pret/pokeemerald EnableNationalPokedex
        },

    .party_count_offset = SAVE_BLOCK_1 + 0x234, // pret/pokeemerald SaveBlock1 playerPartyCount
    .party_offset = SAVE_BLOCK_1 + 0x238,       // pret/pokeemerald SaveBlock1 playerParty
    .current_box_offset = STORAGE + 0x0,        // pret/pokeemerald PokemonStorage currentBox
    .box_records_offset = STORAGE + 0x4,        // pret/pokeemerald PokemonStorage boxes
    .box_names_offset = STORAGE + 0x8344,       // pret/pokeemerald PokemonStorage boxNames
    .wallpapers_offset = STORAGE + 0x83C2,      // pret/pokeemerald PokemonStorage boxWallpapers
    .wallpaper_count = 17,
    .daycare =
        {
            .slot_count = 2,
            .record_offsets =
                {
                    SAVE_BLOCK_1 + 0x3030, // pret/pokeemerald SaveBlock1 daycare.mons[0].mon
                    SAVE_BLOCK_1 + 0x30BC, // pret/pokeemerald SaveBlock1 daycare.mons[1].mon
                },
            .steps_offsets =
                {
                    SAVE_BLOCK_1 + 0x30B8, // pret/pokeemerald SaveBlock1 daycare.mons[0].steps
                    SAVE_BLOCK_1 + 0x3144, // pret/pokeemerald SaveBlock1 daycare.mons[1].steps
                },
            .egg_waiting_flag = 0x86, // pret/pokeemerald FLAG_PENDING_DAYCARE_EGG
            // pret/pokeemerald SaveBlock1 daycare.offspringPersonality
            .egg_personality_offset = SAVE_BLOCK_1 + 0x3148,
            .egg_personality_size = 4,
            // pret/pokeemerald SaveBlock1 daycare.stepCounter
            .step_counter_offset = SAVE_BLOCK_1 + 0x314C,
        },

    .pocket_offsets =
        {
            // pret/pokeemerald SaveBlock1 bagPocket_Items
            [SPEC_GBA_POCKET_ITEMS] = SAVE_BLOCK_1 + 0x560,
            // pret/pokeemerald SaveBlock1 bagPocket_KeyItems
            [SPEC_GBA_POCKET_KEY_ITEMS] = SAVE_BLOCK_1 + 0x5D8,
            // pret/pokeemerald SaveBlock1 bagPocket_PokeBalls
            [SPEC_GBA_POCKET_POKE_BALLS] = SAVE_BLOCK_1 + 0x650,
            // pret/pokeemerald SaveBlock1 bagPocket_TMHM
            [SPEC_GBA_POCKET_TMS_HMS] = SAVE_BLOCK_1 + 0x690,
            // pret/pokeemerald SaveBlock1 bagPocket_Berries
            [SPEC_GBA_POCKET_BERRIES] = SAVE_BLOCK_1 + 0x790,
            // pret/pokeemerald SaveBlock1 pcItems
            [SPEC_GBA_POCKET_PC] = SAVE_BLOCK_1 + 0x498,
        },
};

constexpr spec_gba_layout_t FIRERED_LEAFGREEN_LAYOUT = {
    .type = SPEC_GAME_TYPE_FIRERED_LEAFGREEN,
    // pret/pokefirered sSaveSlotLayout
    .section_sizes = {0xF24, 0xF80, 0xF80, 0xF80, 0xEE8, 0xF80, 0xF80, 0xF80, 0xF80, 0xF80, 0xF80,
                      0xF80, 0xF80, 0x7D0},

    .trainer_name_offset = SAVE_BLOCK_2 + 0x0,   // pret/pokefirered SaveBlock2 playerName
    .trainer_gender_offset = SAVE_BLOCK_2 + 0x8, // pret/pokefirered SaveBlock2 playerGender
    .trainer_id_offset = SAVE_BLOCK_2 + 0xA,     // pret/pokefirered SaveBlock2 playerTrainerId
    .secret_id_offset = SAVE_BLOCK_2 + 0xC,      // pret/pokefirered SaveBlock2 playerTrainerId[2]
    .play_time_offset = SAVE_BLOCK_2 + 0xE,      // pret/pokefirered SaveBlock2 playTimeHours
    .button_mode_offset = SAVE_BLOCK_2 + 0x13,   // pret/pokefirered SaveBlock2 optionsButtonMode
    .options_offset = SAVE_BLOCK_2 + 0x14,       // pret/pokefirered SaveBlock2 optionsTextSpeed
    .window_frame_count = 10,
    .has_security_key = true,
    .security_key_offset = SAVE_BLOCK_2 + 0xF20, // pret/pokefirered SaveBlock2 encryptionKey
    .money_offset = SAVE_BLOCK_1 + 0x290,        // pret/pokefirered SaveBlock1 money
    .coins_offset = SAVE_BLOCK_1 + 0x294,        // pret/pokefirered SaveBlock1 coins
    .has_battle_points = false,
    .has_rival_name = true,
    .rival_name_offset = SAVE_BLOCK_1 + 0x3A4C, // pret/pokefirered SaveBlock1 rivalName
    .flags_offset = SAVE_BLOCK_1 + 0xEE0,       // pret/pokefirered SaveBlock1 flags
    .first_badge_flag = 0x820,                  // pret/pokefirered FLAG_BADGE01_GET

    .pokedex_flag = 0x829,                       // pret/pokefirered FLAG_SYS_POKEDEX_GET
    .pokedex_order_offset = SAVE_BLOCK_2 + 0x18, // pret/pokefirered SaveBlock2 pokedex.order
    .pokedex_mode_offset = SAVE_BLOCK_2 + 0x19,  // pret/pokefirered SaveBlock2 pokedex.mode
    // pret/pokefirered SaveBlock2 pokedex.unownPersonality
    .unown_personality_offset = SAVE_BLOCK_2 + 0x1C,
    // pret/pokefirered SaveBlock2 pokedex.spindaPersonality
    .spinda_personality_offset = SAVE_BLOCK_2 + 0x20,
    .pokedex_caught_offset = SAVE_BLOCK_2 + 0x28, // pret/pokefirered SaveBlock2 pokedex.owned
    .pokedex_seen_offsets =
        {
            SAVE_BLOCK_2 + 0x5C,   // pret/pokefirered SaveBlock2 pokedex.seen
            SAVE_BLOCK_1 + 0x5F8,  // pret/pokefirered SaveBlock1 seen1
            SAVE_BLOCK_1 + 0x3A18, // pret/pokefirered SaveBlock1 seen2
        },
    .national_dex =
        {
            SAVE_BLOCK_2 + 0x1B,   // pret/pokefirered SaveBlock2 pokedex.nationalMagic
            0xB9,                  // pret/pokefirered EnableNationalPokedex
            SAVE_BLOCK_1 + 0x109C, // pret/pokefirered SaveBlock1 vars VAR_NATIONAL_DEX
            0x6258,                // pret/pokefirered EnableNationalPokedex
            0x840,                 // pret/pokefirered FLAG_SYS_NATIONAL_DEX
            false,                 // pret/pokefirered EnableNationalPokedex
        },

    .party_count_offset = SAVE_BLOCK_1 + 0x34, // pret/pokefirered SaveBlock1 playerPartyCount
    .party_offset = SAVE_BLOCK_1 + 0x38,       // pret/pokefirered SaveBlock1 playerParty
    .current_box_offset = STORAGE + 0x0,       // pret/pokefirered PokemonStorage currentBox
    .box_records_offset = STORAGE + 0x4,       // pret/pokefirered PokemonStorage boxes
    .box_names_offset = STORAGE + 0x8344,      // pret/pokefirered PokemonStorage boxNames
    .wallpapers_offset = STORAGE + 0x83C2,     // pret/pokefirered PokemonStorage boxWallpapers
    .wallpaper_count = 16,
    .daycare =
        {
            .slot_count = 3,
            .record_offsets =
                {
                    SAVE_BLOCK_1 + 0x2F80, // pret/pokefirered SaveBlock1 daycare.mons[0].mon
                    SAVE_BLOCK_1 + 0x300C, // pret/pokefirered SaveBlock1 daycare.mons[1].mon
                    SAVE_BLOCK_1 + 0x3C98, // pret/pokefirered SaveBlock1 route5DayCareMon.mon
                },
            .steps_offsets =
                {
                    SAVE_BLOCK_1 + 0x3008, // pret/pokefirered SaveBlock1 daycare.mons[0].steps
                    SAVE_BLOCK_1 + 0x3094, // pret/pokefirered SaveBlock1 daycare.mons[1].steps
                    SAVE_BLOCK_1 + 0x3D20, // pret/pokefirered SaveBlock1 route5DayCareMon.steps
                },
            .egg_waiting_flag = 0x266, // pret/pokefirered FLAG_PENDING_DAYCARE_EGG
            // pret/pokefirered SaveBlock1 daycare.offspringPersonality
            .egg_personality_offset = SAVE_BLOCK_1 + 0x3098,
            .egg_personality_size = 2,
            // pret/pokefirered SaveBlock1 daycare.stepCounter
            .step_counter_offset = SAVE_BLOCK_1 + 0x309A,
        },

    .pocket_offsets =
        {
            // pret/pokefirered SaveBlock1 bagPocket_Items
            [SPEC_GBA_POCKET_ITEMS] = SAVE_BLOCK_1 + 0x310,
            // pret/pokefirered SaveBlock1 bagPocket_KeyItems
            [SPEC_GBA_POCKET_KEY_ITEMS] = SAVE_BLOCK_1 + 0x3B8,
            // pret/pokefirered SaveBlock1 bagPocket_PokeBalls
            [SPEC_GBA_POCKET_POKE_BALLS] = SAVE_BLOCK_1 + 0x430,
            // pret/pokefirered SaveBlock1 bagPocket_TMHM
            [SPEC_GBA_POCKET_TMS_HMS] = SAVE_BLOCK_1 + 0x464,
            // pret/pokefirered SaveBlock1 bagPocket_Berries
            [SPEC_GBA_POCKET_BERRIES] = SAVE_BLOCK_1 + 0x54C,
            // pret/pokefirered SaveBlock1 pcItems
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
