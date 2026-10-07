// Where each Gen 4 game keeps its fields: Diamond and Pearl, Platinum, HeartGold and SoulSilver.

#include "nds/nds_internal.h"

constexpr size_t DIAMOND_PEARL_BAG = 0x624;        // pret/pokediamond Bag
constexpr size_t PLATINUM_BAG = 0x630;             // pret/pokeplatinum SAVE_TABLE_ENTRY_BAG
constexpr size_t HEARTGOLD_SOULSILVER_BAG = 0x644; // pret/pokeheartgold SAVE_BAG

constexpr spec_nds_layout_t DIAMOND_PEARL_LAYOUT = {
    .type = SPEC_GAME_TYPE_DIAMOND_PEARL,
    .general_size = 0xC100,   // pret/pokediamond sub_02023160
    .storage_offset = 0xC100, // pret/pokediamond sub_02023160
    .storage_size = 0x121E0,  // pret/pokediamond sub_02023160
    .footer_size = 0x14,      // pret/pokediamond struct SaveChunkFooter
    .are_blocks_paired = false,

    .player_offset = 0x60, // pret/pokediamond PlayerData
    .has_kanto_badges = false,
    .rival_name_offset = 0x25A8,    // pret/pokediamond UnkStruct_02024E64 rival_name_buf
    .battle_points_offset = 0x65F8, // pret/pokediamond SaveStruct23 frontierData.u_0
    .pokedex =
        {
            .offset = 0x12DC, // pret/pokediamond Pokedex
            .records_every_species_language = false,
            .languages_offset = 0x129,     // pret/pokediamond Pokedex meister
            .form_view_offset = 0x128,     // pret/pokediamond Pokedex unlockedGenderEntries
            .language_view_offset = 0x137, // pret/pokediamond Pokedex unlockedForeignEntries
            .obtained_offset = 0x138,      // pret/pokediamond Pokedex unlockedSinnohDex
            .national_dex_offset = 0x139,  // pret/pokediamond Pokedex unlockedNationalDex
            .has_platinum_forms = false,
            .has_heartgold_soulsilver_forms = false,
        },

    .party_offset = 0x90,         // pret/pokediamond Party
    .daycare_offset = 0x141C,     // pret/pokediamond Daycare
    .current_box_offset = 0x0,    // pret/pokediamond PCStorage curBox
    .box_records_offset = 0x4,    // pret/pokediamond PCStorage boxes
    .box_stride = 0xFF0,          // pret/pokediamond PCStorage boxes[1]
    .box_names_offset = 0x11EE4,  // pret/pokediamond PCStorage names
    .wallpapers_offset = 0x121B4, // pret/pokediamond PCStorage wallpapers
    .wallpaper_count = 24,
    .has_box_modified_flags = false,

    .pocket_offsets =
        {
            // pret/pokediamond Bag items
            [SPEC_NDS_POCKET_ITEMS] = DIAMOND_PEARL_BAG + 0x0,
            // pret/pokediamond Bag medicine
            [SPEC_NDS_POCKET_MEDICINE] = DIAMOND_PEARL_BAG + 0x51C,
            // pret/pokediamond Bag balls
            [SPEC_NDS_POCKET_POKE_BALLS] = DIAMOND_PEARL_BAG + 0x6BC,
            // pret/pokediamond Bag TMsHMs
            [SPEC_NDS_POCKET_TMS_HMS] = DIAMOND_PEARL_BAG + 0x35C,
            // pret/pokediamond Bag berries
            [SPEC_NDS_POCKET_BERRIES] = DIAMOND_PEARL_BAG + 0x5BC,
            // pret/pokediamond Bag mail
            [SPEC_NDS_POCKET_MAIL] = DIAMOND_PEARL_BAG + 0x4EC,
            // pret/pokediamond Bag battleItems
            [SPEC_NDS_POCKET_BATTLE_ITEMS] = DIAMOND_PEARL_BAG + 0x6F8,
            // pret/pokediamond Bag keyItems
            [SPEC_NDS_POCKET_KEY_ITEMS] = DIAMOND_PEARL_BAG + 0x294,
        },
};

constexpr spec_nds_layout_t PLATINUM_LAYOUT = {
    .type = SPEC_GAME_TYPE_PLATINUM,
    .general_size = 0xCF2C,   // pret/pokeplatinum SaveBlockInfo_Init
    .storage_offset = 0xCF2C, // pret/pokeplatinum SaveBlockInfo_Init
    .storage_size = 0x121E4,  // pret/pokeplatinum SaveBlockInfo_Init
    .footer_size = 0x14,      // pret/pokeplatinum SaveBlockFooter
    .are_blocks_paired = false,

    .player_offset = 0x64, // pret/pokeplatinum SAVE_TABLE_ENTRY_PLAYER
    .has_kanto_badges = false,
    .rival_name_offset = 0x27E8, // pret/pokeplatinum MiscSaveBlock rivalName
    // pret/pokeplatinum BattleFrontierSave unk_950.wifiBattleTowerRecord.battlePoints
    .battle_points_offset = 0x7234,
    .pokedex =
        {
            .offset = 0x1328, // pret/pokeplatinum SAVE_TABLE_ENTRY_POKEDEX
            .records_every_species_language = true,
            .languages_offset = 0x128,     // pret/pokeplatinum Pokedex recordedLanguages
            .form_view_offset = 0x318,     // pret/pokeplatinum Pokedex canDetectForms
            .language_view_offset = 0x319, // pret/pokeplatinum Pokedex canDetectLanguages
            .obtained_offset = 0x31A,      // pret/pokeplatinum Pokedex pokedexObtained
            .national_dex_offset = 0x31B,  // pret/pokeplatinum Pokedex nationalDexObtained
            .has_platinum_forms = true,
            .rotom_offset = 0x31C,    // pret/pokeplatinum Pokedex rotomFormsSeen
            .shaymin_offset = 0x320,  // pret/pokeplatinum Pokedex shayminFormsSeen
            .giratina_offset = 0x321, // pret/pokeplatinum Pokedex giratinaFormsSeen
            .has_heartgold_soulsilver_forms = false,
        },

    .party_offset = 0x98,         // pret/pokeplatinum SAVE_TABLE_ENTRY_PARTY
    .daycare_offset = 0x1654,     // pret/pokeplatinum SAVE_TABLE_ENTRY_DAYCARE
    .current_box_offset = 0x0,    // pret/pokeplatinum PCBoxes currentBoxID
    .box_records_offset = 0x4,    // pret/pokeplatinum PCBoxes boxMons
    .box_stride = 0xFF0,          // pret/pokeplatinum PCBoxes boxMons[1]
    .box_names_offset = 0x11EE4,  // pret/pokeplatinum PCBoxes names
    .wallpapers_offset = 0x121B4, // pret/pokeplatinum PCBoxes wallpapers
    .wallpaper_count = 32,
    .has_box_modified_flags = false,

    .pocket_offsets =
        {
            // pret/pokeplatinum Bag items
            [SPEC_NDS_POCKET_ITEMS] = PLATINUM_BAG + 0x0,
            // pret/pokeplatinum Bag medicine
            [SPEC_NDS_POCKET_MEDICINE] = PLATINUM_BAG + 0x51C,
            // pret/pokeplatinum Bag pokeballs
            [SPEC_NDS_POCKET_POKE_BALLS] = PLATINUM_BAG + 0x6BC,
            // pret/pokeplatinum Bag tmHms
            [SPEC_NDS_POCKET_TMS_HMS] = PLATINUM_BAG + 0x35C,
            // pret/pokeplatinum Bag berries
            [SPEC_NDS_POCKET_BERRIES] = PLATINUM_BAG + 0x5BC,
            // pret/pokeplatinum Bag mail
            [SPEC_NDS_POCKET_MAIL] = PLATINUM_BAG + 0x4EC,
            // pret/pokeplatinum Bag battleItems
            [SPEC_NDS_POCKET_BATTLE_ITEMS] = PLATINUM_BAG + 0x6F8,
            // pret/pokeplatinum Bag keyItems
            [SPEC_NDS_POCKET_KEY_ITEMS] = PLATINUM_BAG + 0x294,
        },
};

constexpr spec_nds_layout_t HEARTGOLD_SOULSILVER_LAYOUT = {
    .type = SPEC_GAME_TYPE_HEARTGOLD_SOULSILVER,
    .general_size = 0xF628,   // pret/pokeheartgold SaveData_InitSlotSpecs
    .storage_offset = 0xF700, // pret/pokeheartgold SaveData_InitSubstructs
    .storage_size = 0x12310,  // pret/pokeheartgold SaveData_InitSlotSpecs
    .footer_size = 0x10,      // pret/pokeheartgold struct SaveChunkFooter
    .are_blocks_paired = true,

    .player_offset = 0x60, // pret/pokeheartgold SAVE_PLAYERDATA
    .has_kanto_badges = true,
    .rival_name_offset = 0x22D4, // pret/pokeheartgold SAVE_MISC_DATA rivalName
    // pret/pokeheartgold Save_FrontierData_Get, FrontierData_BattlePointAction
    .battle_points_offset = 0x5BB8,
    .pokedex =
        {
            .offset = 0x12B8, // pret/pokeheartgold SAVE_POKEDEX
            .records_every_species_language = true,
            .languages_offset = 0x144,     // pret/pokeheartgold Pokedex caughtLanguages
            .form_view_offset = 0x334,     // pret/pokeheartgold Pokedex canDetectForms
            .language_view_offset = 0x335, // pret/pokeheartgold Pokedex enabledInternational
            .obtained_offset = 0x336,      // pret/pokeheartgold Pokedex dexEnabled
            .national_dex_offset = 0x337,  // pret/pokeheartgold Pokedex nationalDex
            .has_platinum_forms = true,
            .rotom_offset = 0x338,    // pret/pokeheartgold Pokedex rotomFormOrder
            .shaymin_offset = 0x33C,  // pret/pokeheartgold Pokedex shayminFormOrder
            .giratina_offset = 0x33D, // pret/pokeheartgold Pokedex giratinaFormOrder
            .has_heartgold_soulsilver_forms = true,
            .unown_caught_offset = 0x128, // pret/pokeheartgold Pokedex unownCaughtOrder
            .pichu_offset = 0x33E,        // pret/pokeheartgold Pokedex pichuFormOrder
        },

    .party_offset = 0x90,          // pret/pokeheartgold SAVE_PARTY
    .daycare_offset = 0x15FC,      // pret/pokeheartgold SAVE_DAYCARE
    .current_box_offset = 0x12000, // pret/pokeheartgold PCStorage curBox
    .box_records_offset = 0x0,     // pret/pokeheartgold PCStorage boxes
    .box_stride = 0x1000,          // pret/pokeheartgold PCStorage boxes[1]
    .box_names_offset = 0x12008,   // pret/pokeheartgold PCStorage box_names
    .wallpapers_offset = 0x122D8,  // pret/pokeheartgold PCStorage wallpapers
    .wallpaper_count = 40,
    .has_box_modified_flags = true,
    .box_modified_flags_offset = 0x12004, // pret/pokeheartgold PCStorage boxModifiedFlag

    .pocket_offsets =
        {
            // pret/pokeheartgold Bag items
            [SPEC_NDS_POCKET_ITEMS] = HEARTGOLD_SOULSILVER_BAG + 0x0,
            // pret/pokeheartgold Bag medicine
            [SPEC_NDS_POCKET_MEDICINE] = HEARTGOLD_SOULSILVER_BAG + 0x520,
            // pret/pokeheartgold Bag balls
            [SPEC_NDS_POCKET_POKE_BALLS] = HEARTGOLD_SOULSILVER_BAG + 0x6C0,
            // pret/pokeheartgold Bag TMsHMs
            [SPEC_NDS_POCKET_TMS_HMS] = HEARTGOLD_SOULSILVER_BAG + 0x35C,
            // pret/pokeheartgold Bag berries
            [SPEC_NDS_POCKET_BERRIES] = HEARTGOLD_SOULSILVER_BAG + 0x5C0,
            // pret/pokeheartgold Bag mail
            [SPEC_NDS_POCKET_MAIL] = HEARTGOLD_SOULSILVER_BAG + 0x4F0,
            // pret/pokeheartgold Bag battleItems
            [SPEC_NDS_POCKET_BATTLE_ITEMS] = HEARTGOLD_SOULSILVER_BAG + 0x720,
            // pret/pokeheartgold Bag keyItems
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
