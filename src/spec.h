#ifndef SPEC_H
#define SPEC_H

#include <stdint.h>

enum [[nodiscard]] spec_error {
    SPEC_OK,
    SPEC_ERROR_INVALID_SAVE,
    SPEC_ERROR_VALUE_OUT_OF_RANGE,
    SPEC_ERROR_INVALID_UTF8,
    SPEC_ERROR_UNENCODABLE_CHARACTER,
    SPEC_ERROR_NAME_TOO_LONG,
    SPEC_ERROR_INVALID_ITEM,
    SPEC_ERROR_WRONG_POCKET,
};
typedef enum spec_error spec_error_t;

enum spec_error_location : uint8_t {
    SPEC_ERROR_LOCATION_NONE,
    SPEC_ERROR_LOCATION_PARTY,
    SPEC_ERROR_LOCATION_BOX,
    SPEC_ERROR_LOCATION_ITEMS,
};
typedef enum spec_error_location spec_error_location_t;

// index0 and index1: the party slot, the box and slot, or the pocket and slot.
struct spec_error_data {
    const char *message;
    spec_error_t error;
    spec_error_location_t location;
    uint32_t index0;
    uint32_t index1;
};
typedef struct spec_error_data spec_error_data_t;

enum spec_language : uint8_t {
    SPEC_LANGUAGE_UNKNOWN = 0,
    SPEC_LANGUAGE_JAPANESE = 1,
    SPEC_LANGUAGE_ENGLISH = 2,
    SPEC_LANGUAGE_FRENCH = 3,
    SPEC_LANGUAGE_ITALIAN = 4,
    SPEC_LANGUAGE_GERMAN = 5,
    SPEC_LANGUAGE_SPANISH = 7,
};
typedef enum spec_language spec_language_t;

enum spec_game_type : uint8_t {
    SPEC_GAME_TYPE_RUBY_SAPPHIRE,
    SPEC_GAME_TYPE_EMERALD,
    SPEC_GAME_TYPE_FIRERED_LEAFGREEN,
};
typedef enum spec_game_type spec_game_type_t;

enum spec_version : uint8_t {
    SPEC_VERSION_SAPPHIRE = 1,
    SPEC_VERSION_RUBY = 2,
    SPEC_VERSION_EMERALD = 3,
    SPEC_VERSION_FIRERED = 4,
    SPEC_VERSION_LEAFGREEN = 5,
    SPEC_VERSION_COLOSSEUM_XD = 15,
};
typedef enum spec_version spec_version_t;

enum spec_ball : uint8_t {
    SPEC_BALL_MASTER = 1,
    SPEC_BALL_ULTRA = 2,
    SPEC_BALL_GREAT = 3,
    SPEC_BALL_POKE = 4,
    SPEC_BALL_SAFARI = 5,
    SPEC_BALL_NET = 6,
    SPEC_BALL_DIVE = 7,
    SPEC_BALL_NEST = 8,
    SPEC_BALL_REPEAT = 9,
    SPEC_BALL_TIMER = 10,
    SPEC_BALL_LUXURY = 11,
    SPEC_BALL_PREMIER = 12,
};
typedef enum spec_ball spec_ball_t;

enum spec_nature : uint8_t {
    SPEC_NATURE_HARDY,
    SPEC_NATURE_LONELY,
    SPEC_NATURE_BRAVE,
    SPEC_NATURE_ADAMANT,
    SPEC_NATURE_NAUGHTY,
    SPEC_NATURE_BOLD,
    SPEC_NATURE_DOCILE,
    SPEC_NATURE_RELAXED,
    SPEC_NATURE_IMPISH,
    SPEC_NATURE_LAX,
    SPEC_NATURE_TIMID,
    SPEC_NATURE_HASTY,
    SPEC_NATURE_SERIOUS,
    SPEC_NATURE_JOLLY,
    SPEC_NATURE_NAIVE,
    SPEC_NATURE_MODEST,
    SPEC_NATURE_MILD,
    SPEC_NATURE_QUIET,
    SPEC_NATURE_BASHFUL,
    SPEC_NATURE_RASH,
    SPEC_NATURE_CALM,
    SPEC_NATURE_GENTLE,
    SPEC_NATURE_SASSY,
    SPEC_NATURE_CAREFUL,
    SPEC_NATURE_QUIRKY,
    SPEC_NATURE_COUNT,
};
typedef enum spec_nature spec_nature_t;

enum spec_gender : uint8_t {
    SPEC_GENDER_MALE,
    SPEC_GENDER_FEMALE,
    SPEC_GENDER_GENDERLESS,
};
typedef enum spec_gender spec_gender_t;

enum spec_stat {
    SPEC_STAT_HP,
    SPEC_STAT_ATTACK,
    SPEC_STAT_DEFENSE,
    SPEC_STAT_SPEED,
    SPEC_STAT_SPECIAL_ATTACK,
    SPEC_STAT_SPECIAL_DEFENSE,
    SPEC_STAT_COUNT,
};
typedef enum spec_stat spec_stat_t;

const char *spec_error_string(spec_error_t error);
spec_error_data_t spec_last_error(void);
const char *spec_species_name(uint16_t national_number);

#endif
