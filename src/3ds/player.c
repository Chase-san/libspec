// The Gen 6 and 7 player: trainer, region, play time, wallet, badges or stamps, and options.

#include "3ds/3ds.h"
#include "3ds/3ds_internal.h"
#include "nds/nds_internal.h"
#include "spec_internal.h"

// The community's. TODO: Verify against the carts.
constexpr size_t TRAINER_ID_OFFSET = 0x00;
constexpr size_t SECRET_ID_OFFSET = 0x02;
constexpr size_t VERSION_OFFSET = 0x04;
constexpr size_t TRAINER_GENDER_OFFSET = 0x05;

// The community's. TODO: Verify against the carts.
constexpr size_t PLAY_HOURS_OFFSET = 0x0;
constexpr size_t PLAY_MINUTES_OFFSET = 0x2;
constexpr size_t PLAY_SECONDS_OFFSET = 0x3;

// The community's. TODO: Verify against the carts.
constexpr size_t GEN6_MONEY_OFFSET = 0x08;
constexpr size_t GEN6_BADGES_OFFSET = 0x0C;
constexpr size_t GEN7_MONEY_OFFSET = 0x04;
constexpr size_t GEN7_STAMPS_OFFSET = 0x08;
constexpr unsigned GEN7_FIRST_STAMP_BIT = 4;

constexpr unsigned TEXT_SPEED_BIT = 0;
constexpr unsigned TEXT_SPEED_BIT_COUNT = 2;
constexpr unsigned BATTLE_SCENE_OFF_BIT = 2;
constexpr unsigned BATTLE_STYLE_SET_BIT = 3;
constexpr unsigned BUTTON_MODE_BIT = 13;
constexpr unsigned BUTTON_MODE_BIT_COUNT = 2;

// Where the trainer's block keeps what Gen 7 moved.
struct trainer_offsets {
    size_t subregion;
    size_t country;
    size_t console_region;
    size_t language;
    size_t name;
};
typedef struct trainer_offsets trainer_offsets_t;

constexpr trainer_offsets_t GEN6_TRAINER = {
    .subregion = 0x26,
    .country = 0x27,
    .console_region = 0x2C,
    .language = 0x2D,
    .name = 0x48,
};
constexpr trainer_offsets_t GEN7_TRAINER = {
    .subregion = 0x2E,
    .country = 0x2F,
    .console_region = 0x34,
    .language = 0x35,
    .name = 0x38,
};

static const trainer_offsets_t *trainer_offsets_of(const spec_3ds_layout_t *layout) {
    return layout->is_gen7 ? &GEN7_TRAINER : &GEN6_TRAINER;
}

static void decode_trainer(spec_3ds_save_t *save, const uint8_t *trainer_block,
                           const spec_3ds_layout_t *layout) {
    const trainer_offsets_t *offsets = trainer_offsets_of(layout);
    spec_nds_read_text(save->trainer.name, &trainer_block[offsets->name], SPEC_3DS_NAME_SIZE);
    save->trainer.id = spec_read_u16_le(&trainer_block[TRAINER_ID_OFFSET]);
    save->trainer.secret_id = spec_read_u16_le(&trainer_block[SECRET_ID_OFFSET]);
    save->trainer.is_female = trainer_block[TRAINER_GENDER_OFFSET] != 0;
    save->version = (spec_version_t)trainer_block[VERSION_OFFSET];
    save->language = (spec_language_t)trainer_block[offsets->language];
    save->region = (spec_3ds_region_t){
        .country = trainer_block[offsets->country],
        .subregion = trainer_block[offsets->subregion],
        .console_region = trainer_block[offsets->console_region],
    };
}

static void decode_wallet(spec_3ds_save_t *save, const uint8_t *misc,
                          const spec_3ds_layout_t *layout) {
    if (layout->is_gen7) {
        save->money = spec_read_u32_le(&misc[GEN7_MONEY_OFFSET]);
        save->battle_points = spec_read_u32_le(&misc[layout->battle_points_offset]);
        uint32_t stamps = spec_read_u32_le(&misc[GEN7_STAMPS_OFFSET]) >> GEN7_FIRST_STAMP_BIT;
        spec_decode_flags(save->stamps, SPEC_3DS_STAMP_COUNT, stamps);
        return;
    }
    save->money = spec_read_u32_le(&misc[GEN6_MONEY_OFFSET]);
    save->battle_points = spec_read_u16_le(&misc[layout->battle_points_offset]);
    spec_decode_flags(save->badges, SPEC_3DS_BADGE_COUNT, misc[GEN6_BADGES_OFFSET]);
}

static void decode_options(spec_3ds_options_t *options, uint32_t word) {
    *options = (spec_3ds_options_t){
        .text_speed =
            (spec_3ds_text_speed_t)spec_get_bits(word, TEXT_SPEED_BIT, TEXT_SPEED_BIT_COUNT),
        .is_battle_scene_off = spec_get_flag(word, BATTLE_SCENE_OFF_BIT),
        .is_battle_style_set = spec_get_flag(word, BATTLE_STYLE_SET_BIT),
        .button_mode = (uint8_t)spec_get_bits(word, BUTTON_MODE_BIT, BUTTON_MODE_BIT_COUNT),
    };
}

void spec_3ds_decode_player(spec_3ds_save_t *save, const uint8_t *data,
                            const spec_3ds_layout_t *layout) {
    decode_trainer(save, &data[layout->trainer_offset], layout);
    const uint8_t *play_time = &data[layout->play_time_offset];
    save->play_time = (spec_3ds_play_time_t){
        .hours = spec_read_u16_le(&play_time[PLAY_HOURS_OFFSET]),
        .minutes = play_time[PLAY_MINUTES_OFFSET],
        .seconds = play_time[PLAY_SECONDS_OFFSET],
    };
    decode_wallet(save, &data[layout->misc_offset], layout);
    decode_options(&save->options, spec_read_u32_le(&data[layout->options_offset]));
}

static void encode_trainer(uint8_t *trainer_block, const spec_3ds_layout_t *layout,
                           const spec_3ds_save_t *save) {
    const trainer_offsets_t *offsets = trainer_offsets_of(layout);
    spec_nds_write_text(&trainer_block[offsets->name], save->trainer.name, SPEC_3DS_NAME_SIZE);
    spec_write_u16_le(&trainer_block[TRAINER_ID_OFFSET], save->trainer.id);
    spec_write_u16_le(&trainer_block[SECRET_ID_OFFSET], save->trainer.secret_id);
    trainer_block[TRAINER_GENDER_OFFSET] = save->trainer.is_female ? 1 : 0;
    trainer_block[VERSION_OFFSET] = save->version;
    trainer_block[offsets->language] = save->language;
    trainer_block[offsets->country] = save->region.country;
    trainer_block[offsets->subregion] = save->region.subregion;
    trainer_block[offsets->console_region] = save->region.console_region;
}

// The stamp word's other bits are kept, their meaning unknown.
static void encode_wallet(uint8_t *misc, const spec_3ds_layout_t *layout,
                          const spec_3ds_save_t *save) {
    if (layout->is_gen7) {
        spec_write_u32_le(&misc[GEN7_MONEY_OFFSET], save->money);
        spec_write_u32_le(&misc[layout->battle_points_offset], save->battle_points);
        uint32_t stamps = spec_read_u32_le(&misc[GEN7_STAMPS_OFFSET]);
        stamps = spec_set_bits(stamps, GEN7_FIRST_STAMP_BIT, SPEC_3DS_STAMP_COUNT,
                               spec_encode_flags(save->stamps, SPEC_3DS_STAMP_COUNT));
        spec_write_u32_le(&misc[GEN7_STAMPS_OFFSET], stamps);
        return;
    }
    spec_write_u32_le(&misc[GEN6_MONEY_OFFSET], save->money);
    spec_write_u16_le(&misc[layout->battle_points_offset], (uint16_t)save->battle_points);
    misc[GEN6_BADGES_OFFSET] = (uint8_t)spec_encode_flags(save->badges, SPEC_3DS_BADGE_COUNT);
}

// The options word's other bits are kept, their meaning unknown.
static uint32_t encode_options(uint32_t word, const spec_3ds_options_t *options) {
    word = spec_set_bits(word, TEXT_SPEED_BIT, TEXT_SPEED_BIT_COUNT, options->text_speed);
    word = spec_set_flag(word, BATTLE_SCENE_OFF_BIT, options->is_battle_scene_off);
    word = spec_set_flag(word, BATTLE_STYLE_SET_BIT, options->is_battle_style_set);
    return spec_set_bits(word, BUTTON_MODE_BIT, BUTTON_MODE_BIT_COUNT, options->button_mode);
}

void spec_3ds_encode_player(uint8_t *data, const spec_3ds_layout_t *layout,
                            const spec_3ds_save_t *save) {
    encode_trainer(&data[layout->trainer_offset], layout, save);
    uint8_t *play_time = &data[layout->play_time_offset];
    spec_write_u16_le(&play_time[PLAY_HOURS_OFFSET], save->play_time.hours);
    play_time[PLAY_MINUTES_OFFSET] = save->play_time.minutes;
    play_time[PLAY_SECONDS_OFFSET] = save->play_time.seconds;
    encode_wallet(&data[layout->misc_offset], layout, save);
    uint32_t options = spec_read_u32_le(&data[layout->options_offset]);
    spec_write_u32_le(&data[layout->options_offset], encode_options(options, &save->options));
}

static bool is_version_of(spec_version_t version, spec_game_type_t type) {
    switch (type) {
        case SPEC_GAME_TYPE_X_Y:
            return version == SPEC_VERSION_X || version == SPEC_VERSION_Y;
        case SPEC_GAME_TYPE_OMEGA_RUBY_ALPHA_SAPPHIRE:
            return version == SPEC_VERSION_OMEGA_RUBY || version == SPEC_VERSION_ALPHA_SAPPHIRE;
        case SPEC_GAME_TYPE_SUN_MOON:
            return version == SPEC_VERSION_SUN || version == SPEC_VERSION_MOON;
        case SPEC_GAME_TYPE_ULTRA_SUN_ULTRA_MOON:
            return version == SPEC_VERSION_ULTRA_SUN || version == SPEC_VERSION_ULTRA_MOON;
        default:
            return false;
    }
}

static bool is_any_set(const bool *flags, size_t flag_count) {
    return spec_encode_flags(flags, flag_count) != 0;
}

spec_error_t spec_3ds_check_player(const spec_3ds_save_t *save, const spec_3ds_layout_t *layout) {
    if (!is_version_of(save->version, layout->type)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "version is not one of the type's games");
    }
    if (layout->is_gen7 && is_any_set(save->badges, SPEC_3DS_BADGE_COUNT)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "Gen 7 has stamps, not badges");
    }
    if (!layout->is_gen7 && is_any_set(save->stamps, SPEC_3DS_STAMP_COUNT)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "Gen 6 has badges, not stamps");
    }
    if (!layout->is_gen7 && save->battle_points > UINT16_MAX) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "battle_points does not fit in 16 bits");
    }
    if (!spec_fits_in_bits(save->options.text_speed, TEXT_SPEED_BIT_COUNT)
        || !spec_fits_in_bits(save->options.button_mode, BUTTON_MODE_BIT_COUNT)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "an option does not fit its bits");
    }
    return SPEC_OK;
}
