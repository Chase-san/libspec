// The Gen 2 player: trainer, rival name, play time, wallet and badges.

#include <string.h>

#include "gbc/gbc.h"
#include "gbc/gbc_internal.h"
#include "spec_internal.h"

constexpr uint32_t MAX_MONEY = 999'999;
constexpr uint16_t MAX_COINS = 9'999;
constexpr size_t TRAINER_ID_OFFSET = SPEC_GBC_GAME_DATA_OFFSET;
constexpr size_t TRAINER_NAME_OFFSET = SPEC_GBC_GAME_DATA_OFFSET + 2;
// Mom's savings follow the player's money.
constexpr size_t MOMS_MONEY_DISTANCE = 3;
constexpr size_t KANTO_BADGES_DISTANCE = 1;

// The byte that marks the clock capped comes first and is left as it is.
constexpr size_t PLAY_HOURS_OFFSET = 1;
constexpr size_t PLAY_MINUTES_OFFSET = 3;
constexpr size_t PLAY_SECONDS_OFFSET = 4;
constexpr size_t PLAY_FRAMES_OFFSET = 5;

static bool is_all_zero(const uint8_t *bytes, size_t size) {
    for (size_t index = 0; index < size; ++index) {
        if (bytes[index] != 0) {
            return false;
        }
    }
    return true;
}

static void decode_badges(bool badges[static SPEC_GBC_BADGE_COUNT], uint8_t badge_byte) {
    for (unsigned badge = 0; badge < SPEC_GBC_BADGE_COUNT; ++badge) {
        badges[badge] = spec_get_bits(badge_byte, badge, 1) != 0;
    }
}

static uint8_t encode_badges(const bool badges[static SPEC_GBC_BADGE_COUNT]) {
    uint32_t badge_byte = 0;
    for (unsigned badge = 0; badge < SPEC_GBC_BADGE_COUNT; ++badge) {
        badge_byte = spec_set_bits(badge_byte, badge, 1, badges[badge]);
    }
    return (uint8_t)badge_byte;
}

static void decode_play_time(spec_gbc_play_time_t *play_time, const uint8_t *bytes) {
    play_time->hours = spec_read_u16_be(&bytes[PLAY_HOURS_OFFSET]);
    play_time->minutes = bytes[PLAY_MINUTES_OFFSET];
    play_time->seconds = bytes[PLAY_SECONDS_OFFSET];
    play_time->frames = bytes[PLAY_FRAMES_OFFSET];
}

static void encode_play_time(uint8_t *bytes, const spec_gbc_play_time_t *play_time) {
    spec_write_u16_be(&bytes[PLAY_HOURS_OFFSET], play_time->hours);
    bytes[PLAY_MINUTES_OFFSET] = play_time->minutes;
    bytes[PLAY_SECONDS_OFFSET] = play_time->seconds;
    bytes[PLAY_FRAMES_OFFSET] = play_time->frames;
}

static const char *unencodable_field_of(const spec_gbc_save_t *save,
                                        const spec_gbc_layout_t *layout) {
    size_t unused_name_size = SPEC_GBC_NAME_SIZE - layout->name_size;
    if (!is_all_zero(&save->trainer.name[layout->name_size], unused_name_size)
        || !is_all_zero(&save->rival_name[layout->name_size], unused_name_size)) {
        return "a name is longer than this game's names";
    }
    if (!layout->has_player_gender && save->trainer.is_female) {
        return "only Crystal has the player's gender";
    }
    if (save->money > MAX_MONEY || save->moms_money > MAX_MONEY) {
        return "money is beyond 999999";
    }
    if (save->coins > MAX_COINS) {
        return "coins are beyond 9999";
    }
    return nullptr;
}

void spec_gbc_decode_player(spec_gbc_save_t *save, const uint8_t *data,
                            const spec_gbc_layout_t *layout) {
    save->trainer.id = spec_read_u16_be(&data[TRAINER_ID_OFFSET]);
    memcpy(save->trainer.name, &data[TRAINER_NAME_OFFSET], layout->name_size);
    if (layout->has_player_gender) {
        save->trainer.is_female = data[layout->player_gender_offset] != 0;
    }
    memcpy(save->rival_name, &data[layout->rival_name_offset], layout->name_size);
    decode_play_time(&save->play_time, &data[layout->play_time_offset]);
    save->money = spec_read_u24_be(&data[layout->money_offset]);
    save->moms_money = spec_read_u24_be(&data[layout->money_offset + MOMS_MONEY_DISTANCE]);
    save->coins = spec_read_u16_be(&data[layout->coins_offset]);
    decode_badges(save->johto_badges, data[layout->badges_offset]);
    decode_badges(save->kanto_badges, data[layout->badges_offset + KANTO_BADGES_DISTANCE]);
}

spec_error_t spec_gbc_check_player(const spec_gbc_save_t *save, const spec_gbc_layout_t *layout) {
    const char *unencodable_field = unencodable_field_of(save, layout);
    if (unencodable_field != nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, unencodable_field);
    }
    return SPEC_OK;
}

void spec_gbc_encode_player(uint8_t *data, const spec_gbc_layout_t *layout,
                            const spec_gbc_save_t *save) {
    spec_write_u16_be(&data[TRAINER_ID_OFFSET], save->trainer.id);
    memcpy(&data[TRAINER_NAME_OFFSET], save->trainer.name, layout->name_size);
    if (layout->has_player_gender) {
        data[layout->player_gender_offset] = save->trainer.is_female ? 1 : 0;
    }
    memcpy(&data[layout->rival_name_offset], save->rival_name, layout->name_size);
    encode_play_time(&data[layout->play_time_offset], &save->play_time);
    spec_write_u24_be(&data[layout->money_offset], save->money);
    spec_write_u24_be(&data[layout->money_offset + MOMS_MONEY_DISTANCE], save->moms_money);
    spec_write_u16_be(&data[layout->coins_offset], save->coins);
    data[layout->badges_offset] = encode_badges(save->johto_badges);
    data[layout->badges_offset + KANTO_BADGES_DISTANCE] = encode_badges(save->kanto_badges);
}
