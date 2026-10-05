// The Gen 4 player: trainer, play time, wallet, badges, rival name and options.

#include "nds/nds.h"
#include "nds/nds_internal.h"
#include "spec_internal.h"

constexpr size_t OPTIONS_OFFSET = 0x00;
constexpr size_t TRAINER_NAME_OFFSET = 0x04;
constexpr size_t TRAINER_ID_OFFSET = 0x14;
constexpr size_t SECRET_ID_OFFSET = 0x16;
constexpr size_t MONEY_OFFSET = 0x18;
constexpr size_t TRAINER_GENDER_OFFSET = 0x1C;
constexpr size_t LANGUAGE_OFFSET = 0x1D;
constexpr size_t BADGES_OFFSET = 0x1E;
constexpr size_t KANTO_BADGES_OFFSET = 0x23;
constexpr size_t COINS_OFFSET = 0x24;
constexpr size_t PLAY_HOURS_OFFSET = 0x26;
constexpr size_t PLAY_MINUTES_OFFSET = 0x28;
constexpr size_t PLAY_SECONDS_OFFSET = 0x29;

constexpr unsigned TEXT_SPEED_BIT = 0;
constexpr unsigned TEXT_SPEED_BIT_COUNT = 4;
constexpr unsigned SOUND_MODE_BIT = 4;
constexpr unsigned SOUND_MODE_BIT_COUNT = 2;
constexpr unsigned BATTLE_STYLE_SET_BIT = 6;
constexpr unsigned BATTLE_SCENE_OFF_BIT = 7;
constexpr unsigned BUTTON_MODE_BIT = 8;
constexpr unsigned BUTTON_MODE_BIT_COUNT = 2;
constexpr unsigned WINDOW_FRAME_BIT = 10;
constexpr unsigned WINDOW_FRAME_BIT_COUNT = 5;
constexpr uint32_t SOUND_MODE_STEREO = 0;
constexpr uint32_t SOUND_MODE_MONO = 1;
constexpr uint8_t WINDOW_FRAME_COUNT = 20;
constexpr size_t BADGES_PER_REGION = 8;

static void decode_trainer(spec_nds_trainer_t *trainer, const uint8_t *player) {
    spec_nds_read_text(trainer->name, &player[TRAINER_NAME_OFFSET], SPEC_NDS_TRAINER_NAME_SIZE);
    trainer->id = spec_read_u16_le(&player[TRAINER_ID_OFFSET]);
    trainer->secret_id = spec_read_u16_le(&player[SECRET_ID_OFFSET]);
    trainer->is_female = player[TRAINER_GENDER_OFFSET] != 0;
}

static void decode_play_time(spec_nds_play_time_t *play_time, const uint8_t *player) {
    play_time->hours = spec_read_u16_le(&player[PLAY_HOURS_OFFSET]);
    play_time->minutes = player[PLAY_MINUTES_OFFSET];
    play_time->seconds = player[PLAY_SECONDS_OFFSET];
}

// The sound mode goes to NNS_SndSetMonoFlag, which takes any mode but stereo as mono.
static void decode_options(spec_nds_options_t *options, const uint8_t *player) {
    uint16_t word = spec_read_u16_le(&player[OPTIONS_OFFSET]);
    options->button_mode =
        (spec_nds_button_mode_t)spec_get_bits(word, BUTTON_MODE_BIT, BUTTON_MODE_BIT_COUNT);
    options->text_speed =
        (spec_nds_text_speed_t)spec_get_bits(word, TEXT_SPEED_BIT, TEXT_SPEED_BIT_COUNT);
    options->window_frame = (uint8_t)spec_get_bits(word, WINDOW_FRAME_BIT, WINDOW_FRAME_BIT_COUNT);
    options->is_stereo =
        spec_get_bits(word, SOUND_MODE_BIT, SOUND_MODE_BIT_COUNT) == SOUND_MODE_STEREO;
    options->is_battle_style_set = spec_get_flag(word, BATTLE_STYLE_SET_BIT);
    options->is_battle_scene_off = spec_get_flag(word, BATTLE_SCENE_OFF_BIT);
}

void spec_nds_decode_player(spec_nds_save_t *save, const uint8_t *general,
                            const spec_nds_layout_t *layout) {
    const uint8_t *player = &general[layout->player_offset];
    decode_trainer(&save->trainer, player);
    save->language = (spec_language_t)player[LANGUAGE_OFFSET];
    decode_play_time(&save->play_time, player);
    save->money = spec_read_u32_le(&player[MONEY_OFFSET]);
    save->coins = spec_read_u16_le(&player[COINS_OFFSET]);
    save->battle_points = spec_read_u16_le(&general[layout->battle_points_offset]);
    spec_decode_flags(&save->badges[0], BADGES_PER_REGION, player[BADGES_OFFSET]);
    if (layout->has_kanto_badges) {
        spec_decode_flags(&save->badges[BADGES_PER_REGION], BADGES_PER_REGION,
                          player[KANTO_BADGES_OFFSET]);
    }
    spec_nds_read_text(save->rival_name, &general[layout->rival_name_offset],
                       SPEC_NDS_TRAINER_NAME_SIZE);
    decode_options(&save->options, player);
}

static void encode_trainer(uint8_t *player, const spec_nds_trainer_t *trainer) {
    spec_nds_write_text(&player[TRAINER_NAME_OFFSET], trainer->name, SPEC_NDS_TRAINER_NAME_SIZE);
    spec_write_u16_le(&player[TRAINER_ID_OFFSET], trainer->id);
    spec_write_u16_le(&player[SECRET_ID_OFFSET], trainer->secret_id);
    player[TRAINER_GENDER_OFFSET] = trainer->is_female ? 1 : 0;
}

static void encode_play_time(uint8_t *player, const spec_nds_play_time_t *play_time) {
    spec_write_u16_le(&player[PLAY_HOURS_OFFSET], play_time->hours);
    player[PLAY_MINUTES_OFFSET] = play_time->minutes;
    player[PLAY_SECONDS_OFFSET] = play_time->seconds;
}

static void encode_options(uint8_t *player, const spec_nds_options_t *options) {
    uint32_t sound_mode = options->is_stereo ? SOUND_MODE_STEREO : SOUND_MODE_MONO;
    uint32_t word = 0;
    word = spec_set_bits(word, TEXT_SPEED_BIT, TEXT_SPEED_BIT_COUNT, options->text_speed);
    word = spec_set_bits(word, SOUND_MODE_BIT, SOUND_MODE_BIT_COUNT, sound_mode);
    word = spec_set_flag(word, BATTLE_STYLE_SET_BIT, options->is_battle_style_set);
    word = spec_set_flag(word, BATTLE_SCENE_OFF_BIT, options->is_battle_scene_off);
    word = spec_set_bits(word, BUTTON_MODE_BIT, BUTTON_MODE_BIT_COUNT, options->button_mode);
    word = spec_set_bits(word, WINDOW_FRAME_BIT, WINDOW_FRAME_BIT_COUNT, options->window_frame);
    spec_write_u16_le(&player[OPTIONS_OFFSET], (uint16_t)word);
}

void spec_nds_encode_player(uint8_t *general, const spec_nds_layout_t *layout,
                            const spec_nds_save_t *save) {
    uint8_t *player = &general[layout->player_offset];
    encode_trainer(player, &save->trainer);
    player[LANGUAGE_OFFSET] = save->language;
    encode_play_time(player, &save->play_time);
    spec_write_u32_le(&player[MONEY_OFFSET], save->money);
    spec_write_u16_le(&player[COINS_OFFSET], save->coins);
    spec_write_u16_le(&general[layout->battle_points_offset], save->battle_points);
    player[BADGES_OFFSET] = (uint8_t)spec_encode_flags(&save->badges[0], BADGES_PER_REGION);
    if (layout->has_kanto_badges) {
        player[KANTO_BADGES_OFFSET] =
            (uint8_t)spec_encode_flags(&save->badges[BADGES_PER_REGION], BADGES_PER_REGION);
    }
    spec_nds_write_text(&general[layout->rival_name_offset], save->rival_name,
                        SPEC_NDS_TRAINER_NAME_SIZE);
    encode_options(player, &save->options);
}

static bool has_any_badge(const bool *badges) {
    for (size_t badge = 0; badge < BADGES_PER_REGION; ++badge) {
        if (badges[badge]) {
            return true;
        }
    }
    return false;
}

spec_error_t spec_nds_check_player(const spec_nds_save_t *save, const spec_nds_layout_t *layout) {
    const spec_nds_options_t *options = &save->options;
    if (options->button_mode > SPEC_NDS_BUTTON_MODE_L_IS_A) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "options.button_mode is not the game's");
    }
    if (options->text_speed > SPEC_NDS_TEXT_SPEED_FAST) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "options.text_speed is not the game's");
    }
    if (options->window_frame >= WINDOW_FRAME_COUNT) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                         "options.window_frame is beyond the game's frames");
    }
    if (!layout->has_kanto_badges && has_any_badge(&save->badges[BADGES_PER_REGION])) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "this game has no Kanto badges");
    }
    return SPEC_OK;
}
