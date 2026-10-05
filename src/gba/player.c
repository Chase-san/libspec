// The Gen 3 player: trainer, play time, wallet, badges, rival name and options.

#include "gba/gba.h"
#include "gba/gba_internal.h"
#include "spec_internal.h"

constexpr unsigned TEXT_SPEED_BIT = 0;
constexpr unsigned TEXT_SPEED_BIT_COUNT = 3;
constexpr unsigned WINDOW_FRAME_BIT = 3;
constexpr unsigned WINDOW_FRAME_BIT_COUNT = 5;
constexpr unsigned STEREO_BIT = 8;
constexpr unsigned BATTLE_STYLE_SET_BIT = 9;
constexpr unsigned BATTLE_SCENE_OFF_BIT = 10;
constexpr unsigned REGION_MAP_ZOOMED_BIT = 11;

static bool get_flag(uint32_t word, unsigned bit) {
    return spec_get_bits(word, bit, 1) != 0;
}

static uint32_t set_flag(uint32_t word, unsigned bit, bool is_set) {
    return spec_set_bits(word, bit, 1, is_set);
}

static bool is_all_zero(const uint8_t *bytes, size_t size) {
    for (size_t index = 0; index < size; ++index) {
        if (bytes[index] != 0) {
            return false;
        }
    }
    return true;
}

static void decode_trainer(spec_gba_trainer_t *trainer, const uint8_t *data,
                           const spec_gba_slot_t *slot, const spec_gba_layout_t *layout) {
    spec_gba_read_slot_bytes(trainer->name, data, slot, layout->trainer_name_offset,
                             SPEC_GBA_TRAINER_NAME_SIZE);
    trainer->is_female = spec_gba_read_slot_u8(data, slot, layout->trainer_gender_offset) != 0;
    trainer->id = spec_gba_read_slot_u16(data, slot, layout->trainer_id_offset);
    trainer->secret_id = spec_gba_read_slot_u16(data, slot, layout->secret_id_offset);
}

static void encode_trainer(uint8_t *data, const spec_gba_slot_t *slot,
                           const spec_gba_layout_t *layout, const spec_gba_trainer_t *trainer) {
    spec_gba_write_slot_name(data, slot, layout->trainer_name_offset, trainer->name,
                             SPEC_GBA_TRAINER_NAME_SIZE);
    spec_gba_write_slot_u8(data, slot, layout->trainer_gender_offset, trainer->is_female ? 1 : 0);
    spec_gba_write_slot_u16(data, slot, layout->trainer_id_offset, trainer->id);
    spec_gba_write_slot_u16(data, slot, layout->secret_id_offset, trainer->secret_id);
}

static void decode_play_time(spec_gba_play_time_t *play_time, const uint8_t *data,
                             const spec_gba_slot_t *slot, const spec_gba_layout_t *layout) {
    play_time->hours = spec_gba_read_slot_u16(data, slot, layout->play_time_offset);
    play_time->minutes = spec_gba_read_slot_u8(data, slot, layout->play_time_offset + 2);
    play_time->seconds = spec_gba_read_slot_u8(data, slot, layout->play_time_offset + 3);
    play_time->frames = spec_gba_read_slot_u8(data, slot, layout->play_time_offset + 4);
}

static void encode_play_time(uint8_t *data, const spec_gba_slot_t *slot,
                             const spec_gba_layout_t *layout,
                             const spec_gba_play_time_t *play_time) {
    spec_gba_write_slot_u16(data, slot, layout->play_time_offset, play_time->hours);
    spec_gba_write_slot_u8(data, slot, layout->play_time_offset + 2, play_time->minutes);
    spec_gba_write_slot_u8(data, slot, layout->play_time_offset + 3, play_time->seconds);
    spec_gba_write_slot_u8(data, slot, layout->play_time_offset + 4, play_time->frames);
}

static void decode_options(spec_gba_options_t *options, const uint8_t *data,
                           const spec_gba_slot_t *slot, const spec_gba_layout_t *layout) {
    uint16_t word = spec_gba_read_slot_u16(data, slot, layout->options_offset);
    options->button_mode =
        (spec_gba_button_mode_t)spec_gba_read_slot_u8(data, slot, layout->button_mode_offset);
    options->text_speed =
        (spec_gba_text_speed_t)spec_get_bits(word, TEXT_SPEED_BIT, TEXT_SPEED_BIT_COUNT);
    options->window_frame = (uint8_t)spec_get_bits(word, WINDOW_FRAME_BIT, WINDOW_FRAME_BIT_COUNT);
    options->is_stereo = get_flag(word, STEREO_BIT);
    options->is_battle_style_set = get_flag(word, BATTLE_STYLE_SET_BIT);
    options->is_battle_scene_off = get_flag(word, BATTLE_SCENE_OFF_BIT);
    options->is_region_map_zoomed = get_flag(word, REGION_MAP_ZOOMED_BIT);
}

static void encode_options(uint8_t *data, const spec_gba_slot_t *slot,
                           const spec_gba_layout_t *layout, const spec_gba_options_t *options) {
    uint32_t word = 0;
    word = spec_set_bits(word, TEXT_SPEED_BIT, TEXT_SPEED_BIT_COUNT, options->text_speed);
    word = spec_set_bits(word, WINDOW_FRAME_BIT, WINDOW_FRAME_BIT_COUNT, options->window_frame);
    word = set_flag(word, STEREO_BIT, options->is_stereo);
    word = set_flag(word, BATTLE_STYLE_SET_BIT, options->is_battle_style_set);
    word = set_flag(word, BATTLE_SCENE_OFF_BIT, options->is_battle_scene_off);
    word = set_flag(word, REGION_MAP_ZOOMED_BIT, options->is_region_map_zoomed);
    spec_gba_write_slot_u8(data, slot, layout->button_mode_offset, options->button_mode);
    spec_gba_write_slot_u16(data, slot, layout->options_offset, (uint16_t)word);
}

static void decode_badges(bool badges[static SPEC_GBA_BADGE_COUNT], const uint8_t *data,
                          const spec_gba_slot_t *slot, const spec_gba_layout_t *layout) {
    for (size_t badge = 0; badge < SPEC_GBA_BADGE_COUNT; ++badge) {
        uint16_t flag = (uint16_t)(layout->first_badge_flag + badge);
        badges[badge] = spec_gba_read_slot_flag(data, slot, layout->flags_offset, flag);
    }
}

static void encode_badges(uint8_t *data, const spec_gba_slot_t *slot,
                          const spec_gba_layout_t *layout,
                          const bool badges[static SPEC_GBA_BADGE_COUNT]) {
    for (size_t badge = 0; badge < SPEC_GBA_BADGE_COUNT; ++badge) {
        uint16_t flag = (uint16_t)(layout->first_badge_flag + badge);
        spec_gba_write_slot_flag(data, slot, layout->flags_offset, flag, badges[badge]);
    }
}

uint32_t spec_gba_read_security_key(const uint8_t *data, const spec_gba_slot_t *slot,
                                    const spec_gba_layout_t *layout) {
    if (!layout->has_security_key) {
        return 0;
    }
    return spec_gba_read_slot_u32(data, slot, layout->security_key_offset);
}

void spec_gba_decode_player(spec_gba_save_t *save, const uint8_t *data, const spec_gba_slot_t *slot,
                            const spec_gba_layout_t *layout) {
    uint32_t security_key = spec_gba_read_security_key(data, slot, layout);
    decode_trainer(&save->trainer, data, slot, layout);
    decode_play_time(&save->play_time, data, slot, layout);
    save->money = spec_gba_read_slot_u32(data, slot, layout->money_offset) ^ security_key;
    save->coins =
        (uint16_t)(spec_gba_read_slot_u16(data, slot, layout->coins_offset) ^ security_key);
    if (layout->has_battle_points) {
        save->battle_points = spec_gba_read_slot_u16(data, slot, layout->battle_points_offset);
    }
    decode_badges(save->badges, data, slot, layout);
    if (layout->has_rival_name) {
        spec_gba_read_slot_bytes(save->rival_name, data, slot, layout->rival_name_offset,
                                 SPEC_GBA_TRAINER_NAME_SIZE);
    }
    decode_options(&save->options, data, slot, layout);
}

spec_error_t spec_gba_check_player(const spec_gba_save_t *save, const spec_gba_layout_t *layout) {
    const spec_gba_options_t *options = &save->options;
    if (options->button_mode > SPEC_GBA_BUTTON_MODE_L_EQUALS_A) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "options.button_mode is not the game's");
    }
    if (options->text_speed > SPEC_GBA_TEXT_SPEED_FAST) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "options.text_speed is not the game's");
    }
    if (options->window_frame >= layout->window_frame_count) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                         "options.window_frame is beyond this game's frames");
    }
    if (!layout->has_battle_points && save->battle_points != 0) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "this game has no battle points");
    }
    if (!layout->has_rival_name && !is_all_zero(save->rival_name, SPEC_GBA_TRAINER_NAME_SIZE)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "this game has no rival name");
    }
    return SPEC_OK;
}

void spec_gba_encode_player(uint8_t *data, const spec_gba_slot_t *slot,
                            const spec_gba_layout_t *layout, const spec_gba_save_t *save) {
    uint32_t security_key = spec_gba_read_security_key(data, slot, layout);
    encode_trainer(data, slot, layout, &save->trainer);
    encode_play_time(data, slot, layout, &save->play_time);
    spec_gba_write_slot_u32(data, slot, layout->money_offset, save->money ^ security_key);
    spec_gba_write_slot_u16(data, slot, layout->coins_offset,
                            (uint16_t)(save->coins ^ security_key));
    if (layout->has_battle_points) {
        spec_gba_write_slot_u16(data, slot, layout->battle_points_offset, save->battle_points);
    }
    encode_badges(data, slot, layout, save->badges);
    if (layout->has_rival_name) {
        spec_gba_write_slot_name(data, slot, layout->rival_name_offset, save->rival_name,
                                 SPEC_GBA_TRAINER_NAME_SIZE);
    }
    encode_options(data, slot, layout, &save->options);
}
