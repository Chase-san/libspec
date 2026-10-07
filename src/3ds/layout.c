// Where each Gen 6 and 7 game keeps its fields: X and Y, Omega Ruby and Alpha Sapphire, Sun and
// Moon, Ultra Sun and Ultra Moon.

#include "3ds/3ds_internal.h"

// The block lists are the community's, which every footer of the 3DS saves in hand confirms, and
// each block starts where the last one's length, rounded up to 0x200, ends; so are the labels.
constexpr spec_3ds_layout_t X_Y_LAYOUT = {
    .type = SPEC_GAME_TYPE_X_Y,
    .save_size = 0x65600,
    .block_count = 55,
    .blocks =
        {
            {0x00000, 0x002C8}, // 0 Puff
            {0x00400, 0x00B88}, // 1 MyItem
            {0x01000, 0x0002C}, // 2 ItemInfo
            {0x01200, 0x00038}, // 3 GameTime
            {0x01400, 0x00150}, // 4 Situation
            {0x01600, 0x00004}, // 5 RandomGroup
            {0x01800, 0x00008}, // 6 PlayTime
            {0x01A00, 0x001C0}, // 7 Fashion
            {0x01C00, 0x000BE}, // 8 Amie minigame records
            {0x01E00, 0x00024}, // 9 temporary variables
            {0x02000, 0x02100}, // 10 FieldMoveModelSave
            {0x04200, 0x00140}, // 11 Misc
            {0x04400, 0x00440}, // 12 BOX
            {0x04A00, 0x00574}, // 13 BattleBox
            {0x05000, 0x04E28}, // 14 PSS1
            {0x0A000, 0x04E28}, // 15 PSS2
            {0x0F000, 0x04E28}, // 16 PSS3
            {0x14000, 0x00170}, // 17 MyStatus
            {0x14200, 0x0061C}, // 18 PokePartySave
            {0x14A00, 0x00504}, // 19 EventWork
            {0x15000, 0x006A0}, // 20 ZukanData
            {0x15800, 0x00644}, // 21 hologram clips
            {0x16000, 0x00104}, // 22 UnionPokemon
            {0x16200, 0x00004}, // 23 ConfigSave
            {0x16400, 0x00420}, // 24 Amie decorations
            {0x16A00, 0x00064}, // 25 OPower
            {0x16C00, 0x003F0}, // 26 Strength boulder positions
            {0x17000, 0x0070C}, // 27 Trainer PR Video
            {0x17800, 0x00180}, // 28 GtsData
            {0x17A00, 0x00004}, // 29 Packed Menu Bits
            {0x17C00, 0x0000C}, // 30 PSS profile questions and answers
            {0x17E00, 0x00048}, // 31 Repel, roamers and other overworld state
            {0x18000, 0x00054}, // 32 BOSS download history
            {0x18200, 0x00644}, // 33 Streetpass history
            {0x18A00, 0x005C8}, // 34 LiveMatchData/BattleSpotData
            {0x19000, 0x002F8}, // 35 network connection log
            {0x19400, 0x01B40}, // 36 Dendou
            {0x1B000, 0x001F4}, // 37 BattleHouse
            {0x1B200, 0x001F0}, // 38 Sodateya
            {0x1B400, 0x00216}, // 39 TrialHouse
            {0x1B800, 0x00390}, // 40 BerryField
            {0x1BC00, 0x01A90}, // 41 MysteryGiftSave
            {0x1D800, 0x00308}, // 42 SubEvent log
            {0x1DC00, 0x00618}, // 43 PokeDiarySave
            {0x1E400, 0x0025C}, // 44 Record
            {0x1E800, 0x00834}, // 45 Friend Safari
            {0x1F200, 0x00318}, // 46 SuperTrain
            {0x1F600, 0x007D0}, // 47 unused
            {0x1FE00, 0x00C48}, // 48 LinkInfo
            {0x20C00, 0x00078}, // 49 PSS usage info
            {0x20E00, 0x00200}, // 50 GameSyncSave
            {0x21000, 0x00C84}, // 51 PSS icon
            {0x21E00, 0x00628}, // 52 ValidationSave
            {0x22600, 0x34AD0}, // 53 Box
            {0x57200, 0x0E058}, // 54 JPEG
        },

    .trainer_offset = 0x14000,
    .misc_offset = 0x04200,
    .battle_points_offset = 0x3C,
    .play_time_offset = 0x01800,
    .options_offset = 0x16200,
    .pokedex_offset = 0x15000,
    .has_bank_caught_flags = true,

    .party_offset = 0x14200,
    .box_info_offset = 0x04400,
    .boxes_offset = 0x22600,
    .box_count = 31,
    .daycare_offset = 0x1B200,
    .daycare_count = 1,

    .bag_offset = 0x00400,
    .pocket_offsets =
        {
            [SPEC_3DS_POCKET_ITEMS] = 0x000,
            [SPEC_3DS_POCKET_KEY_ITEMS] = 0x640,
            [SPEC_3DS_POCKET_TMS_HMS] = 0x7C0,
            [SPEC_3DS_POCKET_MEDICINE] = 0x968,
            [SPEC_3DS_POCKET_BERRIES] = 0xA68,
        },
    .pocket_capacities =
        {
            [SPEC_3DS_POCKET_ITEMS] = 400,
            [SPEC_3DS_POCKET_KEY_ITEMS] = 96,
            [SPEC_3DS_POCKET_TMS_HMS] = 106,
            [SPEC_3DS_POCKET_MEDICINE] = 64,
            [SPEC_3DS_POCKET_BERRIES] = 72,
        },
};

constexpr spec_3ds_layout_t OMEGA_RUBY_ALPHA_SAPPHIRE_LAYOUT = {
    .type = SPEC_GAME_TYPE_OMEGA_RUBY_ALPHA_SAPPHIRE,
    .save_size = 0x76000,
    .block_count = 58,
    .blocks =
        {
            {0x00000, 0x002C8}, // 0 Puff
            {0x00400, 0x00B90}, // 1 MyItem
            {0x01000, 0x0002C}, // 2 ItemInfo
            {0x01200, 0x00038}, // 3 GameTime
            {0x01400, 0x00150}, // 4 Situation
            {0x01600, 0x00004}, // 5 RandomGroup
            {0x01800, 0x00008}, // 6 PlayTime
            {0x01A00, 0x001C0}, // 7 Fashion
            {0x01C00, 0x000BE}, // 8 Amie minigame records
            {0x01E00, 0x00024}, // 9 temporary variables
            {0x02000, 0x02100}, // 10 FieldMoveModelSave
            {0x04200, 0x00130}, // 11 Misc
            {0x04400, 0x00440}, // 12 BOX
            {0x04A00, 0x00574}, // 13 BattleBox
            {0x05000, 0x04E28}, // 14 PSS1
            {0x0A000, 0x04E28}, // 15 PSS2
            {0x0F000, 0x04E28}, // 16 PSS3
            {0x14000, 0x00170}, // 17 MyStatus
            {0x14200, 0x0061C}, // 18 PokePartySave
            {0x14A00, 0x00504}, // 19 EventWork
            {0x15000, 0x011CC}, // 20 ZukanData
            {0x16200, 0x00644}, // 21 hologram clips
            {0x16A00, 0x00104}, // 22 UnionPokemon
            {0x16C00, 0x00004}, // 23 ConfigSave
            {0x16E00, 0x00420}, // 24 Amie decorations
            {0x17400, 0x00064}, // 25 OPower
            {0x17600, 0x003F0}, // 26 Strength boulder positions
            {0x17A00, 0x0070C}, // 27 Trainer PR Video
            {0x18200, 0x00180}, // 28 GtsData
            {0x18400, 0x00004}, // 29 Packed Menu Bits
            {0x18600, 0x0000C}, // 30 PSS profile questions and answers
            {0x18800, 0x00048}, // 31 Repel, roamers and other overworld state
            {0x18A00, 0x00054}, // 32 BOSS download history
            {0x18C00, 0x00644}, // 33 Streetpass history
            {0x19400, 0x005C8}, // 34 LiveMatchData/BattleSpotData
            {0x19A00, 0x002F8}, // 35 network connection log
            {0x19E00, 0x01B40}, // 36 Dendou
            {0x1BA00, 0x001F4}, // 37 BattleHouse
            {0x1BC00, 0x003E0}, // 38 Sodateya
            {0x1C000, 0x00216}, // 39 TrialHouse
            {0x1C400, 0x00640}, // 40 BerryField
            {0x1CC00, 0x01A90}, // 41 MysteryGiftSave
            {0x1E800, 0x00400}, // 42 SubEvent log
            {0x1EC00, 0x00618}, // 43 PokeDiarySave
            {0x1F400, 0x0025C}, // 44 Record
            {0x1F800, 0x00834}, // 45 Friend Safari
            {0x20200, 0x00318}, // 46 SuperTrain
            {0x20600, 0x007D0}, // 47 unused
            {0x20E00, 0x00C48}, // 48 LinkInfo
            {0x21C00, 0x00078}, // 49 PSS usage info
            {0x21E00, 0x00200}, // 50 GameSyncSave
            {0x22000, 0x00C84}, // 51 PSS icon
            {0x22E00, 0x00628}, // 52 ValidationSave
            {0x23600, 0x00400}, // 53 Contest
            {0x23A00, 0x07AD0}, // 54 SecretBase
            {0x2B600, 0x078B0}, // 55 EonTicket
            {0x33000, 0x34AD0}, // 56 Box
            {0x67C00, 0x0E058}, // 57 JPEG
        },

    .trainer_offset = 0x14000,
    .misc_offset = 0x04200,
    .battle_points_offset = 0x30,
    .play_time_offset = 0x01800,
    .options_offset = 0x16C00,
    .pokedex_offset = 0x15000,

    .party_offset = 0x14200,
    .box_info_offset = 0x04400,
    .boxes_offset = 0x33000,
    .box_count = 31,
    .daycare_offset = 0x1BC00,
    .daycare_count = 2,

    .bag_offset = 0x00400,
    .pocket_offsets =
        {
            [SPEC_3DS_POCKET_ITEMS] = 0x000,
            [SPEC_3DS_POCKET_KEY_ITEMS] = 0x640,
            [SPEC_3DS_POCKET_TMS_HMS] = 0x7C0,
            [SPEC_3DS_POCKET_MEDICINE] = 0x970,
            [SPEC_3DS_POCKET_BERRIES] = 0xA70,
        },
    .pocket_capacities =
        {
            [SPEC_3DS_POCKET_ITEMS] = 400,
            [SPEC_3DS_POCKET_KEY_ITEMS] = 96,
            [SPEC_3DS_POCKET_TMS_HMS] = 108,
            [SPEC_3DS_POCKET_MEDICINE] = 64,
            [SPEC_3DS_POCKET_BERRIES] = 72,
        },
};

constexpr spec_3ds_layout_t SUN_MOON_LAYOUT = {
    .type = SPEC_GAME_TYPE_SUN_MOON,
    .save_size = 0x6BE00,
    .block_count = 37,
    .blocks =
        {
            {0x00000, 0x00DE0}, // 0 MyItem
            {0x00E00, 0x0007C}, // 1 Situation
            {0x01000, 0x00014}, // 2 RandomGroup
            {0x01200, 0x000C0}, // 3 MyStatus
            {0x01400, 0x0061C}, // 4 PokePartySave
            {0x01C00, 0x00E00}, // 5 EventWork
            {0x02A00, 0x00F78}, // 6 ZukanData
            {0x03A00, 0x00228}, // 7 GtsData
            {0x03E00, 0x00104}, // 8 UnionPokemon
            {0x04000, 0x00200}, // 9 Misc
            {0x04200, 0x00020}, // 10 FieldMenu
            {0x04400, 0x00004}, // 11 ConfigSave
            {0x04600, 0x00058}, // 12 GameTime
            {0x04800, 0x005E6}, // 13 BOX
            {0x04E00, 0x36600}, // 14 BoxPokemon
            {0x3B400, 0x0572C}, // 15 ResortSave
            {0x40C00, 0x00008}, // 16 PlayTime
            {0x40E00, 0x01080}, // 17 FieldMoveModelSave
            {0x42000, 0x01A08}, // 18 Fashion
            {0x43C00, 0x06408}, // 19 JoinFestaPersonalSave
            {0x4A200, 0x06408}, // 20 JoinFestaPersonalSave
            {0x50800, 0x03998}, // 21 JoinFestaDataSave
            {0x54200, 0x00100}, // 22 BerrySpot
            {0x54400, 0x00100}, // 23 FishingSpot
            {0x54600, 0x10528}, // 24 LiveMatchData
            {0x64C00, 0x00204}, // 25 BattleSpotData
            {0x65000, 0x00B60}, // 26 PokeFinderSave
            {0x65C00, 0x03F50}, // 27 MysteryGiftSave
            {0x69C00, 0x00358}, // 28 Record
            {0x6A000, 0x00728}, // 29 ValidationSave
            {0x6A800, 0x00200}, // 30 GameSyncSave
            {0x6AA00, 0x00718}, // 31 PokeDiarySave
            {0x6B200, 0x001FC}, // 32 BattleInstSave
            {0x6B400, 0x00200}, // 33 Sodateya
            {0x6B600, 0x00120}, // 34 WeatherSave
            {0x6B800, 0x001C8}, // 35 QRReaderSaveData
            {0x6BA00, 0x00200}, // 36 TurtleSalmonSave
        },
    .is_gen7 = true,
    .signed_block = 36,
    .hashed_footer_size = 0x140,

    .trainer_offset = 0x01200,
    .misc_offset = 0x04000,
    .battle_points_offset = 0x11C,
    .play_time_offset = 0x40C00,
    .options_offset = 0x04400,
    .pokedex_offset = 0x02A00,

    .party_offset = 0x01400,
    .box_info_offset = 0x04800,
    .boxes_offset = 0x04E00,
    .box_count = 32,
    .daycare_offset = 0x6B400,
    .daycare_count = 1,

    .bag_offset = 0x00000,
    .pocket_offsets =
        {
            [SPEC_3DS_POCKET_ITEMS] = 0x000,
            [SPEC_3DS_POCKET_KEY_ITEMS] = 0x6B8,
            [SPEC_3DS_POCKET_TMS_HMS] = 0x998,
            [SPEC_3DS_POCKET_MEDICINE] = 0xB48,
            [SPEC_3DS_POCKET_BERRIES] = 0xC48,
            [SPEC_3DS_POCKET_Z_CRYSTALS] = 0xD68,
        },
    .pocket_capacities =
        {
            [SPEC_3DS_POCKET_ITEMS] = 430,
            [SPEC_3DS_POCKET_KEY_ITEMS] = 184,
            [SPEC_3DS_POCKET_TMS_HMS] = 108,
            [SPEC_3DS_POCKET_MEDICINE] = 64,
            [SPEC_3DS_POCKET_BERRIES] = 72,
            [SPEC_3DS_POCKET_Z_CRYSTALS] = 30,
        },
};

constexpr spec_3ds_layout_t ULTRA_SUN_ULTRA_MOON_LAYOUT = {
    .type = SPEC_GAME_TYPE_ULTRA_SUN_ULTRA_MOON,
    .save_size = 0x6CC00,
    .block_count = 39,
    .blocks =
        {
            {0x00000, 0x00E28}, // 0 MyItem
            {0x01000, 0x0007C}, // 1 Situation
            {0x01200, 0x00014}, // 2 RandomGroup
            {0x01400, 0x000C0}, // 3 MyStatus
            {0x01600, 0x0061C}, // 4 PokePartySave
            {0x01E00, 0x00E00}, // 5 EventWork
            {0x02C00, 0x00F78}, // 6 ZukanData
            {0x03C00, 0x00228}, // 7 GtsData
            {0x04000, 0x0030C}, // 8 UnionPokemon
            {0x04400, 0x001FC}, // 9 Misc
            {0x04600, 0x0004C}, // 10 FieldMenu
            {0x04800, 0x00004}, // 11 ConfigSave
            {0x04A00, 0x00058}, // 12 GameTime
            {0x04C00, 0x005E6}, // 13 BOX
            {0x05200, 0x36600}, // 14 BoxPokemon
            {0x3B800, 0x0572C}, // 15 ResortSave
            {0x41000, 0x00008}, // 16 PlayTime
            {0x41200, 0x01218}, // 17 FieldMoveModelSave
            {0x42600, 0x01A08}, // 18 Fashion
            {0x44200, 0x06408}, // 19 JoinFestaPersonalSave
            {0x4A800, 0x06408}, // 20 JoinFestaPersonalSave
            {0x50E00, 0x03998}, // 21 JoinFestaDataSave
            {0x54800, 0x00100}, // 22 BerrySpot
            {0x54A00, 0x00100}, // 23 FishingSpot
            {0x54C00, 0x10528}, // 24 LiveMatchData
            {0x65200, 0x00204}, // 25 BattleSpotData
            {0x65600, 0x00B60}, // 26 PokeFinderSave
            {0x66200, 0x03F50}, // 27 MysteryGiftSave
            {0x6A200, 0x00358}, // 28 Record
            {0x6A600, 0x00728}, // 29 ValidationSave
            {0x6AE00, 0x00200}, // 30 GameSyncSave
            {0x6B000, 0x00718}, // 31 PokeDiarySave
            {0x6B800, 0x001FC}, // 32 BattleInstSave
            {0x6BA00, 0x00200}, // 33 Sodateya
            {0x6BC00, 0x00120}, // 34 WeatherSave
            {0x6BE00, 0x001C8}, // 35 QRReaderSaveData
            {0x6C000, 0x00200}, // 36 TurtleSalmonSave
            {0x6C200, 0x0039C}, // 37 BattleFesSave
            {0x6C600, 0x00400}, // 38 FinderStudioSave
        },
    .is_gen7 = true,
    .signed_block = 36,
    .hashed_footer_size = 0x150,

    .trainer_offset = 0x01400,
    .misc_offset = 0x04400,
    .battle_points_offset = 0x11C,
    .play_time_offset = 0x41000,
    .options_offset = 0x04800,
    .pokedex_offset = 0x02C00,

    .party_offset = 0x01600,
    .box_info_offset = 0x04C00,
    .boxes_offset = 0x05200,
    .box_count = 32,
    .daycare_offset = 0x6BA00,
    .daycare_count = 1,

    .bag_offset = 0x00000,
    .pocket_offsets =
        {
            [SPEC_3DS_POCKET_ITEMS] = 0x000,
            [SPEC_3DS_POCKET_KEY_ITEMS] = 0x6AC,
            [SPEC_3DS_POCKET_TMS_HMS] = 0x9C4,
            [SPEC_3DS_POCKET_MEDICINE] = 0xB74,
            [SPEC_3DS_POCKET_BERRIES] = 0xC64,
            [SPEC_3DS_POCKET_Z_CRYSTALS] = 0xD70,
            [SPEC_3DS_POCKET_ROTOM_POWERS] = 0xDFC,
        },
    .pocket_capacities =
        {
            [SPEC_3DS_POCKET_ITEMS] = 427,
            [SPEC_3DS_POCKET_KEY_ITEMS] = 198,
            [SPEC_3DS_POCKET_TMS_HMS] = 108,
            [SPEC_3DS_POCKET_MEDICINE] = 60,
            [SPEC_3DS_POCKET_BERRIES] = 67,
            [SPEC_3DS_POCKET_Z_CRYSTALS] = 35,
            [SPEC_3DS_POCKET_ROTOM_POWERS] = 11,
        },
};

static const spec_3ds_layout_t *const LAYOUTS[] = {
    &X_Y_LAYOUT,
    &OMEGA_RUBY_ALPHA_SAPPHIRE_LAYOUT,
    &SUN_MOON_LAYOUT,
    &ULTRA_SUN_ULTRA_MOON_LAYOUT,
};

// Every layout's save has a size of its own.
const spec_3ds_layout_t *spec_3ds_find_layout(size_t save_size) {
    for (size_t index = 0; index < sizeof LAYOUTS / sizeof LAYOUTS[0]; ++index) {
        if (LAYOUTS[index]->save_size == save_size) {
            return LAYOUTS[index];
        }
    }
    return nullptr;
}

const spec_3ds_layout_t *spec_3ds_get_layout(spec_game_type_t type) {
    for (size_t index = 0; index < sizeof LAYOUTS / sizeof LAYOUTS[0]; ++index) {
        if (LAYOUTS[index]->type == type) {
            return LAYOUTS[index];
        }
    }
    return nullptr;
}
