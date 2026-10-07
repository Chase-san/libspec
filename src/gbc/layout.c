// Where Gen 2 saves keep their fields: Gold and Silver, and Crystal, in Japan and elsewhere.

#include "gbc/gbc_internal.h"

constexpr spec_gb_list_shape_t PARTY_SHAPE = {SPEC_GBC_PARTY_CAPACITY, SPEC_GBC_PARTY_RECORD_SIZE,
                                              SPEC_GBC_NAME_SIZE};
constexpr spec_gb_list_shape_t JAPANESE_PARTY_SHAPE = {
    SPEC_GBC_PARTY_CAPACITY, SPEC_GBC_PARTY_RECORD_SIZE, SPEC_GB_JAPANESE_NAME_SIZE};
constexpr spec_gb_list_shape_t BOX_SHAPE = {20, SPEC_GBC_BOX_RECORD_SIZE, SPEC_GBC_NAME_SIZE};
constexpr spec_gb_list_shape_t JAPANESE_BOX_SHAPE = {30, SPEC_GBC_BOX_RECORD_SIZE,
                                                     SPEC_GB_JAPANESE_NAME_SIZE};

// pret pokegold; its backup is split in five, wPlayerData1-3, wCurMapData and wPokemonData.
constexpr spec_gbc_layout_t GOLD_SILVER_LAYOUT = {
    .save_size = SPEC_GBC_SAVE_SIZE,
    .name_size = SPEC_GBC_NAME_SIZE,
    .box_count = 14,
    .boxes_per_bank = 7,
    .party_shape = PARTY_SHAPE,
    .box_shape = BOX_SHAPE,

    .game_data_size = 0xD60,
    .checksum_offset = 0x2D69,
    .backup_check_value_offset = 0x7E38,
    .backup_checksum_offset = 0x7E6D,
    .backup_chunk_count = 5,
    .backup_chunks =
        {
            {0x2009, 0x226, 0x15C7},
            {0x222F, 0x1AA, 0x3D96},
            {0x23D9, 0x47D, 0x0C6B},
            {0x2856, 0x34, 0x7E39},
            {0x288A, 0x4DF, 0x10E8},
        },

    .mail_size = 0x2F,
    .mail_author_size = SPEC_GBC_MAIL_AUTHOR_SIZE,
    .has_mail_nationality = true,
    .party_mail_offset = 0x0600,
    .party_mail_backup_offset = 0x071A,
    .mailbox_offset = 0x0834,
    .mailbox_backup_offset = 0x0A0B,

    .rival_name_offset = 0x2021,
    .play_time_offset = 0x2052,
    .status_flags_offset = 0x23D9,
    .money_offset = 0x23DB,
    .coins_offset = 0x23E2,
    .badges_offset = 0x23E4,
    .tms_hms_offset = 0x23E6,
    .items_offset = 0x241F,
    .key_items_offset = 0x2449,
    .balls_offset = 0x2464,
    .pc_items_offset = 0x247E,
    .current_box_offset = 0x2724,
    .box_names_offset = 0x2727,
    .party_offset = 0x288A,
    .pokedex_caught_offset = 0x2A4C,
    .pokedex_seen_offset = 0x2A6C,
    .unown_dex_offset = 0x2A8C,
    .daycare_offset = 0x2AA8,
    .active_box_offset = 0x2D6C,
    .has_player_gender = false,
};

// pret pokecrystal; the player's gender lies outside the checksummed data.
constexpr spec_gbc_layout_t CRYSTAL_LAYOUT = {
    .save_size = SPEC_GBC_SAVE_SIZE,
    .name_size = SPEC_GBC_NAME_SIZE,
    .box_count = 14,
    .boxes_per_bank = 7,
    .party_shape = PARTY_SHAPE,
    .box_shape = BOX_SHAPE,

    .game_data_size = 0xB7A,
    .checksum_offset = 0x2D0D,
    .backup_check_value_offset = 0x1208,
    .backup_checksum_offset = 0x1F0D,
    .backup_chunk_count = 1,
    .backup_chunks = {{0x2009, 0xB7A, 0x1209}},

    .mail_size = 0x2F,
    .mail_author_size = SPEC_GBC_MAIL_AUTHOR_SIZE,
    .has_mail_nationality = true,
    .party_mail_offset = 0x0600,
    .party_mail_backup_offset = 0x071A,
    .mailbox_offset = 0x0834,
    .mailbox_backup_offset = 0x0A0B,

    .rival_name_offset = 0x2021,
    .play_time_offset = 0x2051,
    .status_flags_offset = 0x23DA,
    .money_offset = 0x23DC,
    .coins_offset = 0x23E3,
    .badges_offset = 0x23E5,
    .tms_hms_offset = 0x23E7,
    .items_offset = 0x2420,
    .key_items_offset = 0x244A,
    .balls_offset = 0x2465,
    .pc_items_offset = 0x247F,
    .current_box_offset = 0x2700,
    .box_names_offset = 0x2703,
    .party_offset = 0x2865,
    .pokedex_caught_offset = 0x2A27,
    .pokedex_seen_offset = 0x2A47,
    .unown_dex_offset = 0x2A67,
    .daycare_offset = 0x2A83,
    .active_box_offset = 0x2D10,
    .has_player_gender = true,
    .player_gender_offset = 0x3E3D,
};

// Narishma-gb/pokesilver.
constexpr spec_gbc_layout_t JAPANESE_GOLD_SILVER_LAYOUT = {
    .save_size = SPEC_GBC_SAVE_SIZE,
    .name_size = SPEC_GB_JAPANESE_NAME_SIZE,
    .box_count = 9,
    .boxes_per_bank = 6,
    .party_shape = JAPANESE_PARTY_SHAPE,
    .box_shape = JAPANESE_BOX_SHAPE,

    .game_data_size = 0xC83,
    .checksum_offset = 0x2D0D,
    .backup_check_value_offset = 0x7208,
    .backup_checksum_offset = 0x7F0D,
    .backup_chunk_count = 1,
    .backup_chunks = {{0x2009, 0xC83, 0x7209}},

    .mail_size = 0x2A,
    .mail_author_size = 5,
    .has_mail_nationality = false,
    .party_mail_offset = 0x0600,
    .party_mail_backup_offset = 0x06FC,
    .mailbox_offset = 0x07F8,
    .mailbox_backup_offset = 0x099D,

    .rival_name_offset = 0x2017,
    .play_time_offset = 0x2033,
    .status_flags_offset = 0x23BA,
    .money_offset = 0x23BC,
    .coins_offset = 0x23C3,
    .badges_offset = 0x23C5,
    .tms_hms_offset = 0x23C7,
    .items_offset = 0x2400,
    .key_items_offset = 0x242A,
    .balls_offset = 0x2445,
    .pc_items_offset = 0x245F,
    .current_box_offset = 0x2705,
    .box_names_offset = 0x2708,
    .party_offset = 0x283E,
    .pokedex_caught_offset = 0x29CE,
    .pokedex_seen_offset = 0x29EE,
    .unown_dex_offset = 0x2A0E,
    .daycare_offset = 0x2A2A,
    .active_box_offset = 0x2D10,
    .has_player_gender = false,
};

// No disassembly builds Japanese Crystal: each offset is where the cart's own routines
// (cgb-bxtj-jpn.gbc) read or write it, the mail's those of BackupPartyMonMail (#0x47B49).
constexpr spec_gbc_layout_t JAPANESE_CRYSTAL_LAYOUT = {
    .save_size = SPEC_GBC_JAPANESE_CRYSTAL_SAVE_SIZE,
    .name_size = SPEC_GB_JAPANESE_NAME_SIZE,
    .box_count = 9,
    .boxes_per_bank = 6,
    .party_shape = JAPANESE_PARTY_SHAPE,
    .box_shape = JAPANESE_BOX_SHAPE,

    .game_data_size = 0xADA,
    .checksum_offset = 0x2D0D,
    .backup_check_value_offset = 0x7208,
    .backup_checksum_offset = 0x7F0D,
    .backup_chunk_count = 1,
    .backup_chunks = {{0x2009, 0xADA, 0x7209}},

    .mail_size = 0x2A,
    .mail_author_size = 5,
    .has_mail_nationality = false,
    .party_mail_offset = 0x0600,
    .party_mail_backup_offset = 0x06FC,
    .mailbox_offset = 0x07F8,
    .mailbox_backup_offset = 0x099D,

    .rival_name_offset = 0x2017,
    .play_time_offset = 0x2033,
    .status_flags_offset = 0x23BC,
    .money_offset = 0x23BE,
    .coins_offset = 0x23C5,
    .badges_offset = 0x23C7,
    .tms_hms_offset = 0x23C9,
    .items_offset = 0x2402,
    .key_items_offset = 0x242C,
    .balls_offset = 0x2447,
    .pc_items_offset = 0x2461,
    .current_box_offset = 0x26E2,
    .box_names_offset = 0x26E5,
    .party_offset = 0x281A,
    .pokedex_caught_offset = 0x29AA,
    .pokedex_seen_offset = 0x29CA,
    .unown_dex_offset = 0x29EA,
    .daycare_offset = 0x2A06,
    .active_box_offset = 0x2D10,
    .has_player_gender = true,
    .player_gender_offset = 0x8000,
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
