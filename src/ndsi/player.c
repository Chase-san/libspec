// The Gen 5 player: trainer, play time, money, badges and rival name.

#include "nds/nds_internal.h"
#include "ndsi/ndsi.h"
#include "ndsi/ndsi_internal.h"
#include "spec_internal.h"

constexpr size_t TRAINER_NAME_OFFSET = 0x04;
constexpr size_t TRAINER_ID_OFFSET = 0x14;
constexpr size_t SECRET_ID_OFFSET = 0x16;
constexpr size_t LANGUAGE_OFFSET = 0x1E;
constexpr size_t TRAINER_GENDER_OFFSET = 0x21;
constexpr size_t PLAY_HOURS_OFFSET = 0x24;
constexpr size_t PLAY_MINUTES_OFFSET = 0x26;
constexpr size_t PLAY_SECONDS_OFFSET = 0x27;

constexpr size_t MONEY_OFFSET = 0x00;
constexpr size_t BADGES_OFFSET = 0x04;

static void decode_trainer(spec_ndsi_trainer_t *trainer, const uint8_t *player) {
    spec_nds_read_text(trainer->name, &player[TRAINER_NAME_OFFSET], SPEC_NDSI_TRAINER_NAME_SIZE);
    trainer->id = spec_read_u16_le(&player[TRAINER_ID_OFFSET]);
    trainer->secret_id = spec_read_u16_le(&player[SECRET_ID_OFFSET]);
    trainer->is_female = player[TRAINER_GENDER_OFFSET] != 0;
}

static void decode_play_time(spec_ndsi_play_time_t *play_time, const uint8_t *player) {
    play_time->hours = spec_read_u16_le(&player[PLAY_HOURS_OFFSET]);
    play_time->minutes = player[PLAY_MINUTES_OFFSET];
    play_time->seconds = player[PLAY_SECONDS_OFFSET];
}

void spec_ndsi_decode_player(spec_ndsi_save_t *save, const uint8_t *copy,
                             const spec_ndsi_layout_t *layout) {
    const uint8_t *player = &copy[layout->trainer_offset];
    const uint8_t *misc = &copy[layout->misc_offset];
    // TODO: Decode the options and Battle Points.
    decode_trainer(&save->trainer, player);
    save->language = (spec_language_t)player[LANGUAGE_OFFSET];
    decode_play_time(&save->play_time, player);
    save->money = spec_read_u32_le(&misc[MONEY_OFFSET]);
    spec_decode_flags(save->badges, SPEC_NDSI_BADGE_COUNT, misc[BADGES_OFFSET]);
    if (layout->has_rival_name) {
        spec_nds_read_text(save->rival_name, &copy[layout->rival_name_offset],
                           SPEC_NDSI_TRAINER_NAME_SIZE);
    }
}

static void encode_trainer(uint8_t *player, const spec_ndsi_trainer_t *trainer) {
    spec_nds_write_text(&player[TRAINER_NAME_OFFSET], trainer->name, SPEC_NDSI_TRAINER_NAME_SIZE);
    spec_write_u16_le(&player[TRAINER_ID_OFFSET], trainer->id);
    spec_write_u16_le(&player[SECRET_ID_OFFSET], trainer->secret_id);
    player[TRAINER_GENDER_OFFSET] = trainer->is_female ? 1 : 0;
}

static void encode_play_time(uint8_t *player, const spec_ndsi_play_time_t *play_time) {
    spec_write_u16_le(&player[PLAY_HOURS_OFFSET], play_time->hours);
    player[PLAY_MINUTES_OFFSET] = play_time->minutes;
    player[PLAY_SECONDS_OFFSET] = play_time->seconds;
}

void spec_ndsi_encode_player(uint8_t *copy, const spec_ndsi_layout_t *layout,
                             const spec_ndsi_save_t *save) {
    uint8_t *player = &copy[layout->trainer_offset];
    uint8_t *misc = &copy[layout->misc_offset];
    encode_trainer(player, &save->trainer);
    player[LANGUAGE_OFFSET] = save->language;
    encode_play_time(player, &save->play_time);
    spec_write_u32_le(&misc[MONEY_OFFSET], save->money);
    misc[BADGES_OFFSET] = (uint8_t)spec_encode_flags(save->badges, SPEC_NDSI_BADGE_COUNT);
    if (layout->has_rival_name) {
        spec_nds_write_text(&copy[layout->rival_name_offset], save->rival_name,
                            SPEC_NDSI_TRAINER_NAME_SIZE);
    }
}

spec_error_t spec_ndsi_check_player(const spec_ndsi_save_t *save,
                                    const spec_ndsi_layout_t *layout) {
    if (!layout->has_rival_name
        && !spec_is_all_zero((const uint8_t *)save->rival_name, sizeof save->rival_name)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "this game has no rival name");
    }
    return SPEC_OK;
}
