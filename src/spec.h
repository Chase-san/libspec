// What every console shares: errors, common enums, names, game and move data, and the Gen 3-4
// RNG.

#ifndef SPEC_H
#define SPEC_H

#include <stddef.h>
#include <stdint.h>
#include <uchar.h>

// MinGW's <uchar.h> lacks C23's char8_t; repeating the standard's typedef is harmless.
typedef unsigned char char8_t;

constexpr size_t SPEC_EXPECTED_IV_SETS_MAX = 3;
constexpr size_t SPEC_SPECIES_TYPE_COUNT = 2;

// Errors

enum [[nodiscard]] spec_error {
    SPEC_OK,
    SPEC_ERROR_INVALID_SAVE,
    SPEC_ERROR_VALUE_OUT_OF_RANGE,
    SPEC_ERROR_INVALID_UTF8,
    SPEC_ERROR_UNENCODABLE_CHARACTER,
    SPEC_ERROR_NAME_TOO_LONG,
    SPEC_ERROR_INVALID_ITEM,
    SPEC_ERROR_WRONG_POCKET,
    SPEC_ERROR_UNKNOWN_NAME,
};
typedef enum spec_error spec_error_t;

enum spec_error_location : uint8_t {
    SPEC_ERROR_LOCATION_NONE,
    SPEC_ERROR_LOCATION_PARTY,
    SPEC_ERROR_LOCATION_BOX,
    SPEC_ERROR_LOCATION_WALLPAPER,
    SPEC_ERROR_LOCATION_DAYCARE,
    SPEC_ERROR_LOCATION_ITEMS,
    SPEC_ERROR_LOCATION_POKEDEX,
};
typedef enum spec_error_location spec_error_location_t;

// index0 and index1: the party slot, the box and slot, the box, the daycare slot, the pocket and
// slot, or the National Dex number.
struct spec_error_data {
    const char *message;
    spec_error_t error;
    spec_error_location_t location;
    uint32_t index0;
    uint32_t index1;
};
typedef struct spec_error_data spec_error_data_t;

// Games

enum spec_language : uint8_t {
    SPEC_LANGUAGE_UNKNOWN = 0,
    SPEC_LANGUAGE_JAPANESE = 1,
    SPEC_LANGUAGE_ENGLISH = 2,
    SPEC_LANGUAGE_FRENCH = 3,
    SPEC_LANGUAGE_ITALIAN = 4,
    SPEC_LANGUAGE_GERMAN = 5,
    SPEC_LANGUAGE_SPANISH = 7,
    SPEC_LANGUAGE_KOREAN = 8,
    SPEC_LANGUAGE_CHINESE_SIMPLIFIED = 9,   // Gen 7
    SPEC_LANGUAGE_CHINESE_TRADITIONAL = 10, // Gen 7
};
typedef enum spec_language spec_language_t;

enum spec_game_type : uint8_t {
    SPEC_GAME_TYPE_RED_BLUE, // Japanese Green too
    SPEC_GAME_TYPE_YELLOW,
    SPEC_GAME_TYPE_GOLD_SILVER,
    SPEC_GAME_TYPE_CRYSTAL,
    SPEC_GAME_TYPE_RUBY_SAPPHIRE,
    SPEC_GAME_TYPE_EMERALD,
    SPEC_GAME_TYPE_FIRERED_LEAFGREEN,
    SPEC_GAME_TYPE_DIAMOND_PEARL,
    SPEC_GAME_TYPE_PLATINUM,
    SPEC_GAME_TYPE_HEARTGOLD_SOULSILVER,
    SPEC_GAME_TYPE_BLACK_WHITE,
    SPEC_GAME_TYPE_BLACK2_WHITE2,
    SPEC_GAME_TYPE_X_Y,
    SPEC_GAME_TYPE_OMEGA_RUBY_ALPHA_SAPPHIRE,
    SPEC_GAME_TYPE_SUN_MOON,
    SPEC_GAME_TYPE_ULTRA_SUN_ULTRA_MOON,
    SPEC_GAME_TYPE_COUNT,
};
typedef enum spec_game_type spec_game_type_t;

enum spec_version : uint8_t {
    SPEC_VERSION_SAPPHIRE = 1,
    SPEC_VERSION_RUBY = 2,
    SPEC_VERSION_EMERALD = 3,
    SPEC_VERSION_FIRERED = 4,
    SPEC_VERSION_LEAFGREEN = 5,
    SPEC_VERSION_HEARTGOLD = 7,
    SPEC_VERSION_SOULSILVER = 8,
    SPEC_VERSION_DIAMOND = 10,
    SPEC_VERSION_PEARL = 11,
    SPEC_VERSION_PLATINUM = 12,
    SPEC_VERSION_COLOSSEUM_XD = 15,
    SPEC_VERSION_WHITE = 20,
    SPEC_VERSION_BLACK = 21,
    SPEC_VERSION_WHITE2 = 22,
    SPEC_VERSION_BLACK2 = 23,
    SPEC_VERSION_X = 24,
    SPEC_VERSION_Y = 25,
    SPEC_VERSION_ALPHA_SAPPHIRE = 26,
    SPEC_VERSION_OMEGA_RUBY = 27,
    SPEC_VERSION_SUN = 30,
    SPEC_VERSION_MOON = 31,
    SPEC_VERSION_ULTRA_SUN = 32,
    SPEC_VERSION_ULTRA_MOON = 33,
    // The Virtual Console games Poké Transporter moves Pokémon from into Gen 7.
    SPEC_VERSION_RED = 35,
    SPEC_VERSION_GREEN = 36, // Japanese Green, and Blue elsewhere
    SPEC_VERSION_BLUE = 37,  // Japanese Blue
    SPEC_VERSION_YELLOW = 38,
    SPEC_VERSION_GOLD = 39,
    SPEC_VERSION_SILVER = 40,
    SPEC_VERSION_CRYSTAL = 41,
};
typedef enum spec_version spec_version_t;

// Pokémon

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
    SPEC_BALL_DUSK = 13,
    SPEC_BALL_HEAL = 14,
    SPEC_BALL_QUICK = 15,
    SPEC_BALL_CHERISH = 16,
    SPEC_BALL_FAST = 17,
    SPEC_BALL_LEVEL = 18,
    SPEC_BALL_LURE = 19,
    SPEC_BALL_HEAVY = 20,
    SPEC_BALL_LOVE = 21,
    SPEC_BALL_FRIEND = 22,
    SPEC_BALL_MOON = 23,
    SPEC_BALL_SPORT = 24,
    SPEC_BALL_PARK = 25,  // Gen 4
    SPEC_BALL_DREAM = 25, // Gen 5 on
    SPEC_BALL_BEAST = 26, // Gen 7
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

// In Gen 5 to 7's order, then ???; Gen 1 to 4 number types their own way.
enum spec_type : uint8_t {
    SPEC_TYPE_NORMAL,
    SPEC_TYPE_FIGHTING,
    SPEC_TYPE_FLYING,
    SPEC_TYPE_POISON,
    SPEC_TYPE_GROUND,
    SPEC_TYPE_ROCK,
    SPEC_TYPE_BUG,
    SPEC_TYPE_GHOST,
    SPEC_TYPE_STEEL,
    SPEC_TYPE_FIRE,
    SPEC_TYPE_WATER,
    SPEC_TYPE_GRASS,
    SPEC_TYPE_ELECTRIC,
    SPEC_TYPE_PSYCHIC,
    SPEC_TYPE_ICE,
    SPEC_TYPE_DRAGON,
    SPEC_TYPE_DARK,
    SPEC_TYPE_FAIRY,
    SPEC_TYPE_MYSTERY, // aka ???, Curse's in Gen 2 to 4
    SPEC_TYPE_COUNT,
};
typedef enum spec_type spec_type_t;

enum spec_growth_rate : uint8_t {
    SPEC_GROWTH_RATE_MEDIUM_FAST,
    SPEC_GROWTH_RATE_ERRATIC,
    SPEC_GROWTH_RATE_FLUCTUATING,
    SPEC_GROWTH_RATE_MEDIUM_SLOW,
    SPEC_GROWTH_RATE_FAST,
    SPEC_GROWTH_RATE_SLOW,
    SPEC_GROWTH_RATE_COUNT,
};
typedef enum spec_growth_rate spec_growth_rate_t;

typedef uint32_t spec_pid_t;

// How the stored IVs follow the PID in the Gen 3 and Gen 4 generator.
enum spec_iv_method : uint8_t {
    SPEC_IV_METHOD_NONE,
    SPEC_IV_METHOD_STRAIGHT,     // aka method 1
    SPEC_IV_METHOD_SKIP_BEFORE,  // aka method 2
    SPEC_IV_METHOD_SKIP_BETWEEN, // aka method 4
};
typedef enum spec_iv_method spec_iv_method_t;

// How Gen 4 and 5 write a nickname; every Gen 3 naming screen writes it the same way.
enum spec_naming : uint8_t {
    SPEC_NAMING_CAUGHT_OR_HATCHED, // written over the old name
    SPEC_NAMING_NAME_RATER,        // the field naming screen: the Name Rater and gifts
};
typedef enum spec_naming spec_naming_t;

// Moves

struct spec_move_data {
    spec_type_t type;
    uint8_t pp;
};
typedef struct spec_move_data spec_move_data_t;

// Items

struct spec_item_slot {
    uint16_t item;
    uint16_t quantity;
};
typedef struct spec_item_slot spec_item_slot_t;

// Error functions

const char *spec_error_string(spec_error_t error);
spec_error_data_t spec_last_error(void);

// Name functions: the 3DS games' names in each language, nullptr when there is none. Items are
// numbered per generation, so each console names its own.

const char *spec_ability_name(uint16_t ability, spec_language_t language);
// Forms are numbered as the type's games number them.
const char *spec_form_name(spec_game_type_t type, uint16_t national_number, uint8_t form,
                           spec_language_t language);
const char *spec_move_name(uint16_t move, spec_language_t language);
const char *spec_nature_name(spec_nature_t nature, spec_language_t language);
const char *spec_species_name(uint16_t national_number, spec_language_t language);
const char *spec_type_name(spec_type_t type, spec_language_t language);

// Game functions: the highest number each type's games use, 0 for none.

uint16_t spec_last_ability(spec_game_type_t type);
uint16_t spec_last_move(spec_game_type_t type);
uint16_t spec_last_species(spec_game_type_t type);

// Move functions: nullptr or 0 for a move the type's games lack.

const spec_move_data_t *spec_get_move_data(spec_game_type_t type, uint16_t move);
// With pp_ups PP Ups, 0 to 3.
uint8_t spec_move_max_pp(spec_game_type_t type, uint16_t move, uint8_t pp_ups);

// Gen 3-4 RNG functions

size_t spec_find_expected_ivs(spec_pid_t pid, spec_iv_method_t method,
                              uint8_t iv_sets[static SPEC_EXPECTED_IV_SETS_MAX][SPEC_STAT_COUNT]);
spec_iv_method_t spec_find_iv_method(spec_pid_t pid, const uint8_t ivs[static SPEC_STAT_COUNT]);

// Advances the seed and returns its upper half.
uint16_t spec_random(uint32_t *seed);

#endif
