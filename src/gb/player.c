// The Gen 1 player: trainer, rival name, play time, wallet, badges and Yellow's Pikachu.

#include <string.h>

#include "gb/gb.h"
#include "gb/gb_internal.h"
#include "spec_internal.h"

constexpr size_t MONEY_SIZE = 3;
constexpr size_t COINS_SIZE = 2;
constexpr uint32_t MAX_MONEY = 999'999;
constexpr uint16_t MAX_COINS = 9'999;
constexpr uint16_t MAX_PLAY_HOURS = 255;

// Hours, then the byte that marks the clock maxed out, which is left as it is.
constexpr size_t PLAY_HOURS_OFFSET = 0;
constexpr size_t PLAY_MINUTES_OFFSET = 2;
constexpr size_t PLAY_SECONDS_OFFSET = 3;
constexpr size_t PLAY_FRAMES_OFFSET = 4;

static bool is_all_zero(const uint8_t *bytes, size_t size) {
    for (size_t index = 0; index < size; ++index) {
        if (bytes[index] != 0) {
            return false;
        }
    }
    return true;
}

// Packed BCD, most significant digits first.
static uint32_t decode_bcd(const uint8_t *bytes, size_t size) {
    uint32_t value = 0;
    for (size_t index = 0; index < size; ++index) {
        value = value * 100 + spec_get_bits(bytes[index], 4, 4) * 10
                + spec_get_bits(bytes[index], 0, 4);
    }
    return value;
}

static void encode_bcd(uint8_t *bytes, size_t size, uint32_t value) {
    for (size_t index = size; index > 0; --index) {
        uint32_t pair = value % 100;
        bytes[index - 1] = (uint8_t)((pair / 10) << 4 | pair % 10);
        value /= 100;
    }
}

static void decode_badges(bool badges[static SPEC_GB_BADGE_COUNT], uint8_t badge_byte) {
    for (unsigned badge = 0; badge < SPEC_GB_BADGE_COUNT; ++badge) {
        badges[badge] = spec_get_bits(badge_byte, badge, 1) != 0;
    }
}

static uint8_t encode_badges(const bool badges[static SPEC_GB_BADGE_COUNT]) {
    uint32_t badge_byte = 0;
    for (unsigned badge = 0; badge < SPEC_GB_BADGE_COUNT; ++badge) {
        badge_byte = spec_set_bits(badge_byte, badge, 1, badges[badge]);
    }
    return (uint8_t)badge_byte;
}

static void decode_play_time(spec_gb_play_time_t *play_time, const uint8_t *bytes) {
    play_time->hours = bytes[PLAY_HOURS_OFFSET];
    play_time->minutes = bytes[PLAY_MINUTES_OFFSET];
    play_time->seconds = bytes[PLAY_SECONDS_OFFSET];
    play_time->frames = bytes[PLAY_FRAMES_OFFSET];
}

static void encode_play_time(uint8_t *bytes, const spec_gb_play_time_t *play_time) {
    bytes[PLAY_HOURS_OFFSET] = (uint8_t)play_time->hours;
    bytes[PLAY_MINUTES_OFFSET] = play_time->minutes;
    bytes[PLAY_SECONDS_OFFSET] = play_time->seconds;
    bytes[PLAY_FRAMES_OFFSET] = play_time->frames;
}

static const char *unencodable_field_of(const spec_gb_save_t *save,
                                        const spec_gb_layout_t *layout) {
    size_t unused_name_size = SPEC_GB_NAME_SIZE - layout->name_size;
    if (!is_all_zero(&save->trainer.name[layout->name_size], unused_name_size)
        || !is_all_zero(&save->rival_name[layout->name_size], unused_name_size)) {
        return "a name is longer than this game's names";
    }
    if (save->money > MAX_MONEY) {
        return "money is beyond 999999";
    }
    if (save->coins > MAX_COINS) {
        return "coins are beyond 9999";
    }
    if (save->play_time.hours > MAX_PLAY_HOURS) {
        return "play_time.hours is beyond 255";
    }
    if (save->type != SPEC_GAME_TYPE_YELLOW && save->pikachu_friendship != 0) {
        return "only Yellow has a Pikachu's friendship";
    }
    return nullptr;
}

void spec_gb_decode_player(spec_gb_save_t *save, const uint8_t *data,
                           const spec_gb_layout_t *layout) {
    memcpy(save->trainer.name, &data[SPEC_GB_GAME_DATA_OFFSET], layout->name_size);
    save->trainer.id = spec_read_u16_be(&data[layout->trainer_id_offset]);
    memcpy(save->rival_name, &data[layout->rival_name_offset], layout->name_size);
    decode_play_time(&save->play_time, &data[layout->play_time_offset]);
    save->money = decode_bcd(&data[layout->money_offset], MONEY_SIZE);
    save->coins = (uint16_t)decode_bcd(&data[layout->coins_offset], COINS_SIZE);
    decode_badges(save->badges, data[layout->badges_offset]);
    if (save->type == SPEC_GAME_TYPE_YELLOW) {
        save->pikachu_friendship = data[layout->pikachu_friendship_offset];
    }
}

spec_error_t spec_gb_check_player(const spec_gb_save_t *save, const spec_gb_layout_t *layout) {
    const char *unencodable_field = unencodable_field_of(save, layout);
    if (unencodable_field != nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, unencodable_field);
    }
    return SPEC_OK;
}

void spec_gb_encode_player(uint8_t *data, const spec_gb_layout_t *layout,
                           const spec_gb_save_t *save) {
    memcpy(&data[SPEC_GB_GAME_DATA_OFFSET], save->trainer.name, layout->name_size);
    spec_write_u16_be(&data[layout->trainer_id_offset], save->trainer.id);
    memcpy(&data[layout->rival_name_offset], save->rival_name, layout->name_size);
    encode_play_time(&data[layout->play_time_offset], &save->play_time);
    encode_bcd(&data[layout->money_offset], MONEY_SIZE, save->money);
    encode_bcd(&data[layout->coins_offset], COINS_SIZE, save->coins);
    data[layout->badges_offset] = encode_badges(save->badges);
    if (save->type == SPEC_GAME_TYPE_YELLOW) {
        data[layout->pikachu_friendship_offset] = save->pikachu_friendship;
    }
}
