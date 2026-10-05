// The Gen 3 (Game Boy Advance) API: saves, Pokémon, species, personality, items and text.

#ifndef SPEC_GBA_H
#define SPEC_GBA_H

#include <stddef.h>
#include <stdint.h>
#include <uchar.h>

#include "spec.h"

constexpr size_t SPEC_GBA_SAVE_SIZE = 0x20000;
constexpr size_t SPEC_GBA_PARTY_CAPACITY = 6;
constexpr size_t SPEC_GBA_BOX_COUNT = 14;
constexpr size_t SPEC_GBA_BOX_CAPACITY = 30;
constexpr size_t SPEC_GBA_PARTY_RECORD_SIZE = 100;
constexpr size_t SPEC_GBA_BOX_RECORD_SIZE = 80;
constexpr size_t SPEC_GBA_NICKNAME_SIZE = 10;
constexpr size_t SPEC_GBA_TRAINER_NAME_SIZE = 7;
constexpr size_t SPEC_GBA_BOX_NAME_SIZE = 8;
constexpr size_t SPEC_GBA_DAYCARE_CAPACITY = 3;
constexpr size_t SPEC_GBA_MOVE_COUNT = 4;
constexpr size_t SPEC_GBA_BADGE_COUNT = 8;
// Indexed by National Dex number.
constexpr size_t SPEC_GBA_POKEDEX_SIZE = 387;
constexpr size_t SPEC_GBA_POCKET_MAX_CAPACITY = 64;
constexpr size_t SPEC_GBA_TEXT_MAX_SIZE = SPEC_GBA_NICKNAME_SIZE;
// Up to 3 UTF-8 bytes per character, plus NUL.
constexpr size_t SPEC_GBA_TEXT_BUFFER_SIZE = SPEC_GBA_TEXT_MAX_SIZE * 3 + 1;

enum spec_gba_contest_category {
    SPEC_GBA_CONTEST_COOL,
    SPEC_GBA_CONTEST_BEAUTY,
    SPEC_GBA_CONTEST_CUTE,
    SPEC_GBA_CONTEST_SMART,
    SPEC_GBA_CONTEST_TOUGH,
    SPEC_GBA_CONTEST_CATEGORY_COUNT,
};
typedef enum spec_gba_contest_category spec_gba_contest_category_t;

enum spec_gba_pocket {
    SPEC_GBA_POCKET_ITEMS,
    SPEC_GBA_POCKET_KEY_ITEMS,
    SPEC_GBA_POCKET_POKE_BALLS,
    SPEC_GBA_POCKET_TMS_HMS,
    SPEC_GBA_POCKET_BERRIES,
    SPEC_GBA_POCKET_PC,
    SPEC_GBA_POCKET_COUNT,
};
typedef enum spec_gba_pocket spec_gba_pocket_t;

enum spec_gba_button_mode : uint8_t {
    SPEC_GBA_BUTTON_MODE_NORMAL, // HELP in FireRed and LeafGreen
    SPEC_GBA_BUTTON_MODE_LR,
    SPEC_GBA_BUTTON_MODE_L_EQUALS_A,
};
typedef enum spec_gba_button_mode spec_gba_button_mode_t;

enum spec_gba_text_speed : uint8_t {
    SPEC_GBA_TEXT_SPEED_SLOW,
    SPEC_GBA_TEXT_SPEED_MEDIUM,
    SPEC_GBA_TEXT_SPEED_FAST,
};
typedef enum spec_gba_text_speed spec_gba_text_speed_t;

enum spec_gba_contest_rank : uint8_t {
    SPEC_GBA_CONTEST_RANK_NONE,
    SPEC_GBA_CONTEST_RANK_NORMAL,
    SPEC_GBA_CONTEST_RANK_SUPER,
    SPEC_GBA_CONTEST_RANK_HYPER,
    SPEC_GBA_CONTEST_RANK_MASTER,
};
typedef enum spec_gba_contest_rank spec_gba_contest_rank_t;

enum spec_gba_marking : uint8_t {
    SPEC_GBA_MARKING_CIRCLE = 1 << 0,
    SPEC_GBA_MARKING_SQUARE = 1 << 1,
    SPEC_GBA_MARKING_TRIANGLE = 1 << 2,
    SPEC_GBA_MARKING_HEART = 1 << 3,
};
typedef enum spec_gba_marking spec_gba_marking_t;

enum spec_gba_ribbon : uint16_t {
    SPEC_GBA_RIBBON_CHAMPION = 1 << 0,
    SPEC_GBA_RIBBON_WINNING = 1 << 1,
    SPEC_GBA_RIBBON_VICTORY = 1 << 2,
    SPEC_GBA_RIBBON_ARTIST = 1 << 3,
    SPEC_GBA_RIBBON_EFFORT = 1 << 4,
    SPEC_GBA_RIBBON_MARINE = 1 << 5,
    SPEC_GBA_RIBBON_LAND = 1 << 6,
    SPEC_GBA_RIBBON_SKY = 1 << 7,
    SPEC_GBA_RIBBON_COUNTRY = 1 << 8,
    SPEC_GBA_RIBBON_NATIONAL = 1 << 9,
    SPEC_GBA_RIBBON_EARTH = 1 << 10,
    SPEC_GBA_RIBBON_WORLD = 1 << 11,
};
typedef enum spec_gba_ribbon spec_gba_ribbon_t;

// Only pid is stored; the rest is derived on read.
struct spec_gba_personality {
    spec_pid_t pid;
    spec_nature_t nature;
    spec_gender_t gender;
    bool is_shiny;
    uint8_t unown_form;
};
typedef struct spec_gba_personality spec_gba_personality_t;

struct spec_gba_trainer {
    uint8_t name[SPEC_GBA_TRAINER_NAME_SIZE];
    uint16_t id;
    uint16_t secret_id;
    bool is_female;
};
typedef struct spec_gba_trainer spec_gba_trainer_t;

struct spec_gba_origin {
    spec_version_t version;
    uint8_t met_location;
    uint8_t met_level;
    spec_ball_t ball;
};
typedef struct spec_gba_origin spec_gba_origin_t;

struct spec_gba_move {
    uint16_t id;
    uint8_t pp;
    uint8_t pp_ups;
};
typedef struct spec_gba_move spec_gba_move_t;

struct spec_gba_contest {
    uint8_t stats[SPEC_GBA_CONTEST_CATEGORY_COUNT];
    uint8_t sheen;
    spec_gba_contest_rank_t ranks[SPEC_GBA_CONTEST_CATEGORY_COUNT];
};
typedef struct spec_gba_contest spec_gba_contest_t;

struct spec_gba_pokerus {
    uint8_t strain;
    uint8_t days;
};
typedef struct spec_gba_pokerus spec_gba_pokerus_t;

struct spec_gba_status {
    uint8_t sleep_turns;
    bool is_poisoned;
    bool is_burned;
    bool is_frozen;
    bool is_paralyzed;
    bool is_badly_poisoned;
    uint8_t toxic_turns;
};
typedef struct spec_gba_status spec_gba_status_t;

// Zero for boxed Pokémon; filled in when written to the party.
struct spec_gba_party_data {
    spec_gba_status_t status;
    uint8_t level;
    uint8_t mail_id;
    uint16_t current_hp;
    uint16_t stats[SPEC_STAT_COUNT];
};
typedef struct spec_gba_party_data spec_gba_party_data_t;

// Names are game-encoded; species is the game's index, not the National Dex number.
struct spec_gba_pokemon {
    uint16_t species;
    spec_gba_personality_t personality;
    uint8_t nickname[SPEC_GBA_NICKNAME_SIZE];
    spec_language_t language;
    spec_gba_trainer_t trainer;
    spec_gba_origin_t origin;

    uint32_t experience;
    uint8_t friendship;
    uint16_t held_item;
    uint8_t ability_number;
    uint8_t ivs[SPEC_STAT_COUNT];
    // Derived on read.
    spec_iv_method_t iv_method;
    uint8_t evs[SPEC_STAT_COUNT];
    spec_gba_move_t moves[SPEC_GBA_MOVE_COUNT];

    spec_gba_contest_t contest;
    uint16_t ribbons;
    uint8_t markings;
    spec_gba_pokerus_t pokerus;

    bool is_egg;
    bool is_bad_egg;
    bool is_box_rs_blocked;
    bool is_fateful_encounter;

    spec_gba_party_data_t party_data;
};
typedef struct spec_gba_pokemon spec_gba_pokemon_t;

struct spec_gba_play_time {
    uint16_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t frames;
};
typedef struct spec_gba_play_time spec_gba_play_time_t;

struct spec_gba_options {
    spec_gba_button_mode_t button_mode;
    spec_gba_text_speed_t text_speed;
    uint8_t window_frame;
    bool is_stereo;
    bool is_battle_style_set;
    bool is_battle_scene_off;
    bool is_region_map_zoomed;
};
typedef struct spec_gba_options spec_gba_options_t;

// Writing is_caught also marks the species seen.
struct spec_gba_pokedex {
    bool is_obtained;
    bool has_national_dex;
    uint32_t unown_personality;
    uint32_t spinda_personality;
    bool is_seen[SPEC_GBA_POKEDEX_SIZE];
    bool is_caught[SPEC_GBA_POKEDEX_SIZE];
};
typedef struct spec_gba_pokedex spec_gba_pokedex_t;

struct spec_gba_box {
    uint8_t name[SPEC_GBA_BOX_NAME_SIZE];
    uint8_t wallpaper;
    spec_gba_pokemon_t pokemon[SPEC_GBA_BOX_CAPACITY];
};
typedef struct spec_gba_box spec_gba_box_t;

struct spec_gba_daycare_slot {
    spec_gba_pokemon_t pokemon;
    uint32_t steps;
};
typedef struct spec_gba_daycare_slot spec_gba_daycare_slot_t;

// The third slot is FireRed and LeafGreen's Route 5 daycare.
struct spec_gba_daycare {
    spec_gba_daycare_slot_t slots[SPEC_GBA_DAYCARE_CAPACITY];
    bool is_egg_waiting;
    uint32_t egg_personality;
    uint8_t step_counter;
};
typedef struct spec_gba_daycare spec_gba_daycare_t;

typedef spec_item_slot_t spec_gba_item_slot_t;

// Writing condenses pockets; a slot with no item or zero quantity is empty.
struct spec_gba_save {
    spec_game_type_t type;
    spec_language_t language;
    spec_gba_trainer_t trainer;
    spec_gba_play_time_t play_time;
    uint32_t money;
    uint16_t coins;
    uint16_t battle_points; // Emerald only
    bool badges[SPEC_GBA_BADGE_COUNT];
    uint8_t rival_name[SPEC_GBA_TRAINER_NAME_SIZE]; // FireRed and LeafGreen only
    spec_gba_options_t options;
    spec_gba_pokedex_t pokedex;
    uint8_t party_count;
    spec_gba_pokemon_t party[SPEC_GBA_PARTY_CAPACITY];
    uint8_t current_box;
    spec_gba_box_t boxes[SPEC_GBA_BOX_COUNT];
    spec_gba_daycare_t daycare;
    spec_gba_item_slot_t items[SPEC_GBA_POCKET_COUNT][SPEC_GBA_POCKET_MAX_CAPACITY];
};
typedef struct spec_gba_save spec_gba_save_t;

spec_error_t spec_gba_read_save(spec_gba_save_t *save,
                                const uint8_t data[static SPEC_GBA_SAVE_SIZE]);
spec_error_t spec_gba_write_save(const spec_gba_save_t *save,
                                 uint8_t data[static SPEC_GBA_SAVE_SIZE]);

// raw is the encrypted record as the save stores it.
spec_error_t spec_gba_read_pokemon(spec_gba_pokemon_t *pokemon, const uint8_t *raw,
                                   size_t raw_size);
spec_error_t spec_gba_write_pokemon(uint8_t *raw, size_t raw_size,
                                    const spec_gba_pokemon_t *pokemon);
spec_error_t spec_gba_pokemon_calculate_stats(spec_gba_pokemon_t *pokemon);
spec_error_t spec_gba_pokemon_get_name(const spec_gba_pokemon_t *pokemon,
                                       char8_t name[static SPEC_GBA_TEXT_BUFFER_SIZE]);
spec_error_t spec_gba_pokemon_set_name(spec_gba_pokemon_t *pokemon, const char8_t *name);
bool spec_gba_is_safe_to_box(const spec_gba_pokemon_t *pokemon);

uint16_t spec_gba_species_to_national(uint16_t species);
uint16_t spec_gba_species_from_national(uint16_t national_number);

spec_gba_personality_t spec_gba_decode_personality(spec_pid_t pid, uint16_t species,
                                                   const spec_gba_trainer_t *trainer);

const char *spec_gba_item_name(uint16_t item, spec_language_t language);
spec_error_t spec_gba_get_pocket_for_item(spec_gba_pocket_t *pocket, spec_game_type_t type,
                                          uint16_t item);
size_t spec_gba_pocket_capacity(spec_game_type_t type, spec_gba_pocket_t pocket);
size_t spec_gba_pocket_item_count(const spec_gba_save_t *save, spec_gba_pocket_t pocket);

spec_error_t spec_gba_text_to_utf8(char8_t utf8[static SPEC_GBA_TEXT_BUFFER_SIZE],
                                   const uint8_t *text, size_t text_size, spec_language_t language);
spec_error_t spec_gba_text_from_utf8(uint8_t *text, size_t text_size, const char8_t *utf8,
                                     spec_language_t language);

#endif
