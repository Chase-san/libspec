// Where Gen 2 saves keep their fields: Gold and Silver, and Crystal, in Japan and elsewhere.

#include "gbc/gbc_internal.h"

constexpr spec_gb_list_shape_t PARTY_SHAPE = {SPEC_GBC_PARTY_CAPACITY, SPEC_GBC_PARTY_RECORD_SIZE,
                                              SPEC_GBC_NAME_SIZE};
constexpr spec_gb_list_shape_t JAPANESE_PARTY_SHAPE = {
    SPEC_GBC_PARTY_CAPACITY, SPEC_GBC_PARTY_RECORD_SIZE, SPEC_GB_JAPANESE_NAME_SIZE};
constexpr spec_gb_list_shape_t BOX_SHAPE = {20, SPEC_GBC_BOX_RECORD_SIZE, SPEC_GBC_NAME_SIZE};
constexpr spec_gb_list_shape_t JAPANESE_BOX_SHAPE = {30, SPEC_GBC_BOX_RECORD_SIZE,
                                                     SPEC_GB_JAPANESE_NAME_SIZE};

// The backup copy is split in five.
constexpr spec_gbc_layout_t GOLD_SILVER_LAYOUT = {
    .save_size = SPEC_GBC_SAVE_SIZE,
    .name_size = SPEC_GBC_NAME_SIZE,
    .box_count = 14,
    .boxes_per_bank = 7,
    .party_shape = PARTY_SHAPE,
    .box_shape = BOX_SHAPE,

    .game_data_size = 0xD60,             // pret/pokegold sGameData
    .checksum_offset = 0x2D69,           // pret/pokegold sChecksum
    .backup_check_value_offset = 0x7E38, // pret/pokegold sBackupCheckValue1
    .backup_checksum_offset = 0x7E6D,    // pret/pokegold sBackupChecksum
    .backup_chunk_count = 5,
    .backup_chunks =
        {
            {0x2009, 0x226, 0x15C7}, // pret/pokegold sPlayerData1 sBackupPlayerData1
            {0x222F, 0x1AA, 0x3D96}, // pret/pokegold sPlayerData2 sBackupPlayerData2
            {0x23D9, 0x47D, 0x0C6B}, // pret/pokegold sPlayerData3 sBackupPlayerData3
            {0x2856, 0x34, 0x7E39},  // pret/pokegold sCurMapData sBackupCurMapData
            {0x288A, 0x4DF, 0x10E8}, // pret/pokegold sPokemonData sBackupPokemonData
        },

    .mail_size = 0x2F, // pret/pokegold mailmsg
    .mail_author_size = SPEC_GBC_MAIL_AUTHOR_SIZE,
    .has_mail_nationality = true,
    .party_mail_offset = 0x0600,        // pret/pokegold sPartyMail
    .party_mail_backup_offset = 0x071A, // pret/pokegold sPartyMailBackup
    .mailbox_offset = 0x0834,           // pret/pokegold sMailboxCount
    .mailbox_backup_offset = 0x0A0B,    // pret/pokegold sMailboxCountBackup

    .rival_name_offset = 0x2021,     // pret/pokegold sPlayerData1 wRivalName
    .play_time_offset = 0x2052,      // pret/pokegold sPlayerData1 wGameTimeCap
    .status_flags_offset = 0x23D9,   // pret/pokegold sPlayerData3 wStatusFlags
    .money_offset = 0x23DB,          // pret/pokegold sPlayerData3 wMoney
    .coins_offset = 0x23E2,          // pret/pokegold sPlayerData3 wCoins
    .badges_offset = 0x23E4,         // pret/pokegold sPlayerData3 wBadges
    .tms_hms_offset = 0x23E6,        // pret/pokegold sPlayerData3 wTMsHMs
    .items_offset = 0x241F,          // pret/pokegold sPlayerData3 wNumItems
    .key_items_offset = 0x2449,      // pret/pokegold sPlayerData3 wNumKeyItems
    .balls_offset = 0x2464,          // pret/pokegold sPlayerData3 wNumBalls
    .pc_items_offset = 0x247E,       // pret/pokegold sPlayerData3 wNumPCItems
    .current_box_offset = 0x2724,    // pret/pokegold sPlayerData3 wCurBox
    .box_names_offset = 0x2727,      // pret/pokegold sPlayerData3 wBoxNames
    .party_offset = 0x288A,          // pret/pokegold sPokemonData wPartyCount
    .pokedex_caught_offset = 0x2A4C, // pret/pokegold sPokemonData wPokedexCaught
    .pokedex_seen_offset = 0x2A6C,   // pret/pokegold sPokemonData wPokedexSeen
    .unown_dex_offset = 0x2A8C,      // pret/pokegold sPokemonData wUnownDex
    .daycare_offset = 0x2AA8,        // pret/pokegold sPokemonData wDayCareMan
    .active_box_offset = 0x2D6C,     // pret/pokegold sBox
    .has_player_gender = false,
};

// The player's gender lies outside the checksummed data.
constexpr spec_gbc_layout_t CRYSTAL_LAYOUT = {
    .save_size = SPEC_GBC_SAVE_SIZE,
    .name_size = SPEC_GBC_NAME_SIZE,
    .box_count = 14,
    .boxes_per_bank = 7,
    .party_shape = PARTY_SHAPE,
    .box_shape = BOX_SHAPE,

    .game_data_size = 0xB7A,             // pret/pokecrystal sGameData
    .checksum_offset = 0x2D0D,           // pret/pokecrystal sChecksum
    .backup_check_value_offset = 0x1208, // pret/pokecrystal sBackupCheckValue1
    .backup_checksum_offset = 0x1F0D,    // pret/pokecrystal sBackupChecksum
    .backup_chunk_count = 1,
    .backup_chunks = {{0x2009, 0xB7A, 0x1209}}, // pret/pokecrystal sGameData sBackupGameData

    .mail_size = 0x2F, // pret/pokecrystal mailmsg
    .mail_author_size = SPEC_GBC_MAIL_AUTHOR_SIZE,
    .has_mail_nationality = true,
    .party_mail_offset = 0x0600,        // pret/pokecrystal sPartyMail
    .party_mail_backup_offset = 0x071A, // pret/pokecrystal sPartyMailBackup
    .mailbox_offset = 0x0834,           // pret/pokecrystal sMailboxCount
    .mailbox_backup_offset = 0x0A0B,    // pret/pokecrystal sMailboxCountBackup

    .rival_name_offset = 0x2021,     // pret/pokecrystal sPlayerData wRivalName
    .play_time_offset = 0x2051,      // pret/pokecrystal sPlayerData wGameTimeCap
    .status_flags_offset = 0x23DA,   // pret/pokecrystal sPlayerData wStatusFlags
    .money_offset = 0x23DC,          // pret/pokecrystal sPlayerData wMoney
    .coins_offset = 0x23E3,          // pret/pokecrystal sPlayerData wCoins
    .badges_offset = 0x23E5,         // pret/pokecrystal sPlayerData wBadges
    .tms_hms_offset = 0x23E7,        // pret/pokecrystal sPlayerData wTMsHMs
    .items_offset = 0x2420,          // pret/pokecrystal sPlayerData wNumItems
    .key_items_offset = 0x244A,      // pret/pokecrystal sPlayerData wNumKeyItems
    .balls_offset = 0x2465,          // pret/pokecrystal sPlayerData wNumBalls
    .pc_items_offset = 0x247F,       // pret/pokecrystal sPlayerData wNumPCItems
    .current_box_offset = 0x2700,    // pret/pokecrystal sPlayerData wCurBox
    .box_names_offset = 0x2703,      // pret/pokecrystal sPlayerData wBoxNames
    .party_offset = 0x2865,          // pret/pokecrystal sPokemonData wPartyCount
    .pokedex_caught_offset = 0x2A27, // pret/pokecrystal sPokemonData wPokedexCaught
    .pokedex_seen_offset = 0x2A47,   // pret/pokecrystal sPokemonData wPokedexSeen
    .unown_dex_offset = 0x2A67,      // pret/pokecrystal sPokemonData wUnownDex
    .daycare_offset = 0x2A83,        // pret/pokecrystal sPokemonData wDayCareMan
    .active_box_offset = 0x2D10,     // pret/pokecrystal sBox
    .has_player_gender = true,
    .player_gender_offset = 0x3E3D, // pret/pokecrystal sCrystalData wPlayerGender
};

constexpr spec_gbc_layout_t JAPANESE_GOLD_SILVER_LAYOUT = {
    .save_size = SPEC_GBC_SAVE_SIZE,
    .name_size = SPEC_GB_JAPANESE_NAME_SIZE,
    .box_count = 9,
    .boxes_per_bank = 6,
    .party_shape = JAPANESE_PARTY_SHAPE,
    .box_shape = JAPANESE_BOX_SHAPE,

    .game_data_size = 0xC83,             // Narishma-gb/pokesilver sGameData
    .checksum_offset = 0x2D0D,           // Narishma-gb/pokesilver sChecksum
    .backup_check_value_offset = 0x7208, // Narishma-gb/pokesilver sBackupCheckValue1
    .backup_checksum_offset = 0x7F0D,    // Narishma-gb/pokesilver sBackupChecksum
    .backup_chunk_count = 1,
    .backup_chunks = {{0x2009, 0xC83, 0x7209}}, // Narishma-gb/pokesilver sGameData sBackupGameData

    .mail_size = 0x2A, // Narishma-gb/pokesilver mailmsg
    .mail_author_size = 5,
    .has_mail_nationality = false,
    .party_mail_offset = 0x0600,        // Narishma-gb/pokesilver sPartyMail
    .party_mail_backup_offset = 0x06FC, // Narishma-gb/pokesilver sPartyMailBackup
    .mailbox_offset = 0x07F8,           // Narishma-gb/pokesilver sMailboxCount
    .mailbox_backup_offset = 0x099D,    // Narishma-gb/pokesilver sMailboxCountBackup

    .rival_name_offset = 0x2017,     // Narishma-gb/pokesilver sPlayerData wRivalName
    .play_time_offset = 0x2033,      // Narishma-gb/pokesilver sPlayerData wGameTimeCap
    .status_flags_offset = 0x23BA,   // Narishma-gb/pokesilver sPlayerData wStatusFlags
    .money_offset = 0x23BC,          // Narishma-gb/pokesilver sPlayerData wMoney
    .coins_offset = 0x23C3,          // Narishma-gb/pokesilver sPlayerData wCoins
    .badges_offset = 0x23C5,         // Narishma-gb/pokesilver sPlayerData wBadges
    .tms_hms_offset = 0x23C7,        // Narishma-gb/pokesilver sPlayerData wTMsHMs
    .items_offset = 0x2400,          // Narishma-gb/pokesilver sPlayerData wNumItems
    .key_items_offset = 0x242A,      // Narishma-gb/pokesilver sPlayerData wNumKeyItems
    .balls_offset = 0x2445,          // Narishma-gb/pokesilver sPlayerData wNumBalls
    .pc_items_offset = 0x245F,       // Narishma-gb/pokesilver sPlayerData wNumPCItems
    .current_box_offset = 0x2705,    // Narishma-gb/pokesilver sPlayerData wCurBox
    .box_names_offset = 0x2708,      // Narishma-gb/pokesilver sPlayerData wBoxNames
    .party_offset = 0x283E,          // Narishma-gb/pokesilver sPokemonData wPartyCount
    .pokedex_caught_offset = 0x29CE, // Narishma-gb/pokesilver sPokemonData wPokedexCaught
    .pokedex_seen_offset = 0x29EE,   // Narishma-gb/pokesilver sPokemonData wPokedexSeen
    .unown_dex_offset = 0x2A0E,      // Narishma-gb/pokesilver sPokemonData wUnownDex
    .daycare_offset = 0x2A2A,        // Narishma-gb/pokesilver sPokemonData wDayCareMan
    .active_box_offset = 0x2D10,     // Narishma-gb/pokesilver sBox
    .has_player_gender = false,
};

// No disassembly builds Japanese Crystal: each offset is where the cart's own routines read or
// write it. Bulbapedia's Save data structure (Generation II) agrees with each one it lists but play
// time, where its hours are a byte late.
constexpr spec_gbc_layout_t JAPANESE_CRYSTAL_LAYOUT = {
    .save_size = SPEC_GBC_JAPANESE_CRYSTAL_SAVE_SIZE,
    .name_size = SPEC_GB_JAPANESE_NAME_SIZE,
    .box_count = 9,
    .boxes_per_bank = 6,
    .party_shape = JAPANESE_PARTY_SHAPE,
    .box_shape = JAPANESE_BOX_SHAPE,

    .game_data_size = 0xADA,             // Bulbapedia Checksums
    .checksum_offset = 0x2D0D,           // Bulbapedia Checksums
    .backup_check_value_offset = 0x7208, // cgb-bxtj-jpn.gbc
    .backup_checksum_offset = 0x7F0D,    // Bulbapedia Checksums
    .backup_chunk_count = 1,
    .backup_chunks = {{0x2009, 0xADA, 0x7209}}, // Bulbapedia Checksums

    .mail_size = 0x2A, // cgb-bxtj-jpn.gbc BackupPartyMonMail (#0x47B49)
    .mail_author_size = 5,
    .has_mail_nationality = false,
    .party_mail_offset = 0x0600,        // cgb-bxtj-jpn.gbc BackupPartyMonMail (#0x47B49)
    .party_mail_backup_offset = 0x06FC, // cgb-bxtj-jpn.gbc BackupPartyMonMail (#0x47B49)
    .mailbox_offset = 0x07F8,           // cgb-bxtj-jpn.gbc BackupPartyMonMail (#0x47B49)
    .mailbox_backup_offset = 0x099D,    // cgb-bxtj-jpn.gbc BackupPartyMonMail (#0x47B49)

    .rival_name_offset = 0x2017,     // Bulbapedia Rival name
    .play_time_offset = 0x2033,      // cgb-bxtj-jpn.gbc GameTimer (#0x207B)
    .status_flags_offset = 0x23BC,   // cgb-bxtj-jpn.gbc
    .money_offset = 0x23BE,          // Bulbapedia Money
    .coins_offset = 0x23C5,          // Bulbapedia Game Coins
    .badges_offset = 0x23C7,         // Bulbapedia Johto Badges
    .tms_hms_offset = 0x23C9,        // Bulbapedia TM pocket
    .items_offset = 0x2402,          // Bulbapedia Item pocket item list
    .key_items_offset = 0x242C,      // Bulbapedia Key item pocket item list
    .balls_offset = 0x2447,          // Bulbapedia Ball pocket item list
    .pc_items_offset = 0x2461,       // Bulbapedia PC item list
    .current_box_offset = 0x26E2,    // Bulbapedia Current PC Box number
    .box_names_offset = 0x26E5,      // Bulbapedia PC Box names
    .party_offset = 0x281A,          // Bulbapedia Party Pokémon list
    .pokedex_caught_offset = 0x29AA, // Bulbapedia Pokédex owned
    .pokedex_seen_offset = 0x29CA,   // Bulbapedia Pokédex seen
    .unown_dex_offset = 0x29EA,      // cgb-bxtj-jpn.gbc
    .daycare_offset = 0x2A06,        // cgb-bxtj-jpn.gbc
    .active_box_offset = 0x2D10,     // Bulbapedia Current Box Pokémon list
    .has_player_gender = true,
    .player_gender_offset = 0x8000, // Bulbapedia Player gender
};

static bool is_international(spec_language_t language) {
    return language == SPEC_LANGUAGE_ENGLISH || language == SPEC_LANGUAGE_FRENCH
           || language == SPEC_LANGUAGE_ITALIAN || language == SPEC_LANGUAGE_GERMAN
           || language == SPEC_LANGUAGE_SPANISH;
}

const spec_gbc_layout_t *spec_gbc_get_layout(spec_game_type_t type, spec_language_t language) {
    bool is_japanese = language == SPEC_LANGUAGE_JAPANESE;
    if (!is_japanese && !is_international(language)) {
        return nullptr;
    }
    switch (type) {
        case SPEC_GAME_TYPE_GOLD_SILVER:
            return is_japanese ? &JAPANESE_GOLD_SILVER_LAYOUT : &GOLD_SILVER_LAYOUT;
        case SPEC_GAME_TYPE_CRYSTAL:
            return is_japanese ? &JAPANESE_CRYSTAL_LAYOUT : &CRYSTAL_LAYOUT;
        default:
            return nullptr;
    }
}
