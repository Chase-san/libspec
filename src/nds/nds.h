// The Gen 4 (Nintendo DS) API: saves, Pokémon, personality, items and text.

#ifndef SPEC_NDS_H
#define SPEC_NDS_H

#include <stddef.h>
#include <stdint.h>
#include <uchar.h>

#include "spec.h"

constexpr size_t SPEC_NDS_SAVE_SIZE = 0x80000;
constexpr size_t SPEC_NDS_PARTY_CAPACITY = 6;
constexpr size_t SPEC_NDS_BOX_COUNT = 18;
constexpr size_t SPEC_NDS_BOX_CAPACITY = 30;
constexpr size_t SPEC_NDS_PARTY_RECORD_SIZE = 236;
constexpr size_t SPEC_NDS_BOX_RECORD_SIZE = 136;
// Text sizes count 16-bit characters, the terminator's included.
constexpr size_t SPEC_NDS_NICKNAME_SIZE = 11;
constexpr size_t SPEC_NDS_TRAINER_NAME_SIZE = 8;
constexpr size_t SPEC_NDS_BOX_NAME_SIZE = 20;
constexpr size_t SPEC_NDS_DAYCARE_CAPACITY = 2;
constexpr size_t SPEC_NDS_MOVE_COUNT = 4;
// Sinnoh's or Johto's, then HeartGold and SoulSilver's Kanto badges.
constexpr size_t SPEC_NDS_BADGE_COUNT = 16;
constexpr size_t SPEC_NDS_MAIL_ICON_COUNT = 3;
constexpr size_t SPEC_NDS_MAIL_SENTENCE_COUNT = 3;
constexpr size_t SPEC_NDS_MAIL_WORD_COUNT = 2;
constexpr size_t SPEC_NDS_SEAL_COUNT = 8;
// Indexed by National Dex number.
constexpr size_t SPEC_NDS_POKEDEX_SIZE = 494;
// Unown's 28 letters, the longest form order.
constexpr size_t SPEC_NDS_FORM_ORDER_MAX = 28;
constexpr size_t SPEC_NDS_POCKET_MAX_CAPACITY = 165;
constexpr size_t SPEC_NDS_TEXT_MAX_SIZE = SPEC_NDS_BOX_NAME_SIZE;
// Up to 3 UTF-8 bytes per character, plus NUL.
constexpr size_t SPEC_NDS_TEXT_BUFFER_SIZE = SPEC_NDS_TEXT_MAX_SIZE * 3 + 1;

enum spec_nds_contest_category {
    SPEC_NDS_CONTEST_COOL,
    SPEC_NDS_CONTEST_BEAUTY,
    SPEC_NDS_CONTEST_CUTE,
    SPEC_NDS_CONTEST_SMART,
    SPEC_NDS_CONTEST_TOUGH,
    SPEC_NDS_CONTEST_CATEGORY_COUNT,
};
typedef enum spec_nds_contest_category spec_nds_contest_category_t;

// In the games' pocket order.
enum spec_nds_pocket {
    SPEC_NDS_POCKET_ITEMS,
    SPEC_NDS_POCKET_MEDICINE,
    SPEC_NDS_POCKET_POKE_BALLS,
    SPEC_NDS_POCKET_TMS_HMS,
    SPEC_NDS_POCKET_BERRIES,
    SPEC_NDS_POCKET_MAIL,
    SPEC_NDS_POCKET_BATTLE_ITEMS,
    SPEC_NDS_POCKET_KEY_ITEMS,
    SPEC_NDS_POCKET_COUNT,
};
typedef enum spec_nds_pocket spec_nds_pocket_t;

enum spec_nds_button_mode : uint8_t {
    SPEC_NDS_BUTTON_MODE_NORMAL,
    SPEC_NDS_BUTTON_MODE_START_IS_X,
    SPEC_NDS_BUTTON_MODE_L_IS_A,
};
typedef enum spec_nds_button_mode spec_nds_button_mode_t;

enum spec_nds_text_speed : uint8_t {
    SPEC_NDS_TEXT_SPEED_SLOW,
    SPEC_NDS_TEXT_SPEED_MEDIUM,
    SPEC_NDS_TEXT_SPEED_FAST,
};
typedef enum spec_nds_text_speed spec_nds_text_speed_t;

enum spec_nds_marking : uint8_t {
    SPEC_NDS_MARKING_CIRCLE = 1 << 0,
    SPEC_NDS_MARKING_TRIANGLE = 1 << 1,
    SPEC_NDS_MARKING_SQUARE = 1 << 2,
    SPEC_NDS_MARKING_HEART = 1 << 3,
    SPEC_NDS_MARKING_STAR = 1 << 4,
    SPEC_NDS_MARKING_DIAMOND = 1 << 5,
};
typedef enum spec_nds_marking spec_nds_marking_t;

enum spec_nds_sinnoh_ribbon : uint32_t {
    SPEC_NDS_SINNOH_RIBBON_CHAMPION = 1U << 0,
    SPEC_NDS_SINNOH_RIBBON_ABILITY = 1U << 1,
    SPEC_NDS_SINNOH_RIBBON_GREAT_ABILITY = 1U << 2,
    SPEC_NDS_SINNOH_RIBBON_DOUBLE_ABILITY = 1U << 3,
    SPEC_NDS_SINNOH_RIBBON_MULTI_ABILITY = 1U << 4,
    SPEC_NDS_SINNOH_RIBBON_PAIR_ABILITY = 1U << 5,
    SPEC_NDS_SINNOH_RIBBON_WORLD_ABILITY = 1U << 6,
    SPEC_NDS_SINNOH_RIBBON_ALERT = 1U << 7,
    SPEC_NDS_SINNOH_RIBBON_SHOCK = 1U << 8,
    SPEC_NDS_SINNOH_RIBBON_DOWNCAST = 1U << 9,
    SPEC_NDS_SINNOH_RIBBON_CARELESS = 1U << 10,
    SPEC_NDS_SINNOH_RIBBON_RELAX = 1U << 11,
    SPEC_NDS_SINNOH_RIBBON_SNOOZE = 1U << 12,
    SPEC_NDS_SINNOH_RIBBON_SMILE = 1U << 13,
    SPEC_NDS_SINNOH_RIBBON_GORGEOUS = 1U << 14,
    SPEC_NDS_SINNOH_RIBBON_ROYAL = 1U << 15,
    SPEC_NDS_SINNOH_RIBBON_GORGEOUS_ROYAL = 1U << 16,
    SPEC_NDS_SINNOH_RIBBON_FOOTPRINT = 1U << 17,
    SPEC_NDS_SINNOH_RIBBON_RECORD = 1U << 18,
    SPEC_NDS_SINNOH_RIBBON_HISTORY = 1U << 19,
    SPEC_NDS_SINNOH_RIBBON_LEGEND = 1U << 20,
    SPEC_NDS_SINNOH_RIBBON_RED = 1U << 21,
    SPEC_NDS_SINNOH_RIBBON_GREEN = 1U << 22,
    SPEC_NDS_SINNOH_RIBBON_BLUE = 1U << 23,
    SPEC_NDS_SINNOH_RIBBON_FESTIVAL = 1U << 24,
    SPEC_NDS_SINNOH_RIBBON_CARNIVAL = 1U << 25,
    SPEC_NDS_SINNOH_RIBBON_CLASSIC = 1U << 26,
    SPEC_NDS_SINNOH_RIBBON_PREMIER = 1U << 27,
};
typedef enum spec_nds_sinnoh_ribbon spec_nds_sinnoh_ribbon_t;

// The contest ribbons are one per rank won, as Pal Park converts a Gen 3 rank.
enum spec_nds_hoenn_ribbon : uint32_t {
    SPEC_NDS_HOENN_RIBBON_COOL = 1U << 0,
    SPEC_NDS_HOENN_RIBBON_COOL_SUPER = 1U << 1,
    SPEC_NDS_HOENN_RIBBON_COOL_HYPER = 1U << 2,
    SPEC_NDS_HOENN_RIBBON_COOL_MASTER = 1U << 3,
    SPEC_NDS_HOENN_RIBBON_BEAUTY = 1U << 4,
    SPEC_NDS_HOENN_RIBBON_BEAUTY_SUPER = 1U << 5,
    SPEC_NDS_HOENN_RIBBON_BEAUTY_HYPER = 1U << 6,
    SPEC_NDS_HOENN_RIBBON_BEAUTY_MASTER = 1U << 7,
    SPEC_NDS_HOENN_RIBBON_CUTE = 1U << 8,
    SPEC_NDS_HOENN_RIBBON_CUTE_SUPER = 1U << 9,
    SPEC_NDS_HOENN_RIBBON_CUTE_HYPER = 1U << 10,
    SPEC_NDS_HOENN_RIBBON_CUTE_MASTER = 1U << 11,
    SPEC_NDS_HOENN_RIBBON_SMART = 1U << 12,
    SPEC_NDS_HOENN_RIBBON_SMART_SUPER = 1U << 13,
    SPEC_NDS_HOENN_RIBBON_SMART_HYPER = 1U << 14,
    SPEC_NDS_HOENN_RIBBON_SMART_MASTER = 1U << 15,
    SPEC_NDS_HOENN_RIBBON_TOUGH = 1U << 16,
    SPEC_NDS_HOENN_RIBBON_TOUGH_SUPER = 1U << 17,
    SPEC_NDS_HOENN_RIBBON_TOUGH_HYPER = 1U << 18,
    SPEC_NDS_HOENN_RIBBON_TOUGH_MASTER = 1U << 19,
    SPEC_NDS_HOENN_RIBBON_CHAMPION = 1U << 20,
    SPEC_NDS_HOENN_RIBBON_WINNING = 1U << 21,
    SPEC_NDS_HOENN_RIBBON_VICTORY = 1U << 22,
    SPEC_NDS_HOENN_RIBBON_ARTIST = 1U << 23,
    SPEC_NDS_HOENN_RIBBON_EFFORT = 1U << 24,
    SPEC_NDS_HOENN_RIBBON_MARINE = 1U << 25,
    SPEC_NDS_HOENN_RIBBON_LAND = 1U << 26,
    SPEC_NDS_HOENN_RIBBON_SKY = 1U << 27,
    SPEC_NDS_HOENN_RIBBON_COUNTRY = 1U << 28,
    SPEC_NDS_HOENN_RIBBON_NATIONAL = 1U << 29,
    SPEC_NDS_HOENN_RIBBON_EARTH = 1U << 30,
    SPEC_NDS_HOENN_RIBBON_WORLD = 1U << 31,
};
typedef enum spec_nds_hoenn_ribbon spec_nds_hoenn_ribbon_t;

enum spec_nds_super_contest_ribbon : uint32_t {
    SPEC_NDS_SUPER_CONTEST_RIBBON_COOL = 1U << 0,
    SPEC_NDS_SUPER_CONTEST_RIBBON_COOL_GREAT = 1U << 1,
    SPEC_NDS_SUPER_CONTEST_RIBBON_COOL_ULTRA = 1U << 2,
    SPEC_NDS_SUPER_CONTEST_RIBBON_COOL_MASTER = 1U << 3,
    SPEC_NDS_SUPER_CONTEST_RIBBON_BEAUTY = 1U << 4,
    SPEC_NDS_SUPER_CONTEST_RIBBON_BEAUTY_GREAT = 1U << 5,
    SPEC_NDS_SUPER_CONTEST_RIBBON_BEAUTY_ULTRA = 1U << 6,
    SPEC_NDS_SUPER_CONTEST_RIBBON_BEAUTY_MASTER = 1U << 7,
    SPEC_NDS_SUPER_CONTEST_RIBBON_CUTE = 1U << 8,
    SPEC_NDS_SUPER_CONTEST_RIBBON_CUTE_GREAT = 1U << 9,
    SPEC_NDS_SUPER_CONTEST_RIBBON_CUTE_ULTRA = 1U << 10,
    SPEC_NDS_SUPER_CONTEST_RIBBON_CUTE_MASTER = 1U << 11,
    SPEC_NDS_SUPER_CONTEST_RIBBON_SMART = 1U << 12,
    SPEC_NDS_SUPER_CONTEST_RIBBON_SMART_GREAT = 1U << 13,
    SPEC_NDS_SUPER_CONTEST_RIBBON_SMART_ULTRA = 1U << 14,
    SPEC_NDS_SUPER_CONTEST_RIBBON_SMART_MASTER = 1U << 15,
    SPEC_NDS_SUPER_CONTEST_RIBBON_TOUGH = 1U << 16,
    SPEC_NDS_SUPER_CONTEST_RIBBON_TOUGH_GREAT = 1U << 17,
    SPEC_NDS_SUPER_CONTEST_RIBBON_TOUGH_ULTRA = 1U << 18,
    SPEC_NDS_SUPER_CONTEST_RIBBON_TOUGH_MASTER = 1U << 19,
};
typedef enum spec_nds_super_contest_ribbon spec_nds_super_contest_ribbon_t;

enum spec_nds_shiny_leaf : uint8_t {
    SPEC_NDS_SHINY_LEAF_A = 1 << 0,
    SPEC_NDS_SHINY_LEAF_B = 1 << 1,
    SPEC_NDS_SHINY_LEAF_C = 1 << 2,
    SPEC_NDS_SHINY_LEAF_D = 1 << 3,
    SPEC_NDS_SHINY_LEAF_E = 1 << 4,
    SPEC_NDS_SHINY_LEAF_CROWN = 1 << 5,
};
typedef enum spec_nds_shiny_leaf spec_nds_shiny_leaf_t;

// The Pokédex records no Korean.
enum spec_nds_pokedex_language : uint8_t {
    SPEC_NDS_POKEDEX_LANGUAGE_JAPANESE = 1 << 0,
    SPEC_NDS_POKEDEX_LANGUAGE_ENGLISH = 1 << 1,
    SPEC_NDS_POKEDEX_LANGUAGE_FRENCH = 1 << 2,
    SPEC_NDS_POKEDEX_LANGUAGE_GERMAN = 1 << 3,
    SPEC_NDS_POKEDEX_LANGUAGE_ITALIAN = 1 << 4,
    SPEC_NDS_POKEDEX_LANGUAGE_SPANISH = 1 << 5,
};
typedef enum spec_nds_pokedex_language spec_nds_pokedex_language_t;

// Only pid is stored; the rest is derived on read.
struct spec_nds_personality {
    spec_pid_t pid;
    spec_nature_t nature;
    spec_gender_t gender;
    bool is_shiny;
};
typedef struct spec_nds_personality spec_nds_personality_t;

struct spec_nds_trainer {
    uint16_t name[SPEC_NDS_TRAINER_NAME_SIZE];
    uint16_t id;
    uint16_t secret_id;
    bool is_female;
};
typedef struct spec_nds_trainer spec_nds_trainer_t;

struct spec_nds_date {
    uint8_t year; // since 2000
    uint8_t month;
    uint8_t day;
};
typedef struct spec_nds_date spec_nds_date_t;

// A met_level of 0 means hatched.
struct spec_nds_origin {
    spec_version_t version;
    spec_ball_t ball;
    uint8_t met_level;
    uint16_t met_location;
    spec_nds_date_t met_date;
    uint16_t egg_location;
    spec_nds_date_t egg_date;
    uint8_t encounter_type;
};
typedef struct spec_nds_origin spec_nds_origin_t;

struct spec_nds_move {
    uint16_t id;
    uint8_t pp;
    uint8_t pp_ups;
};
typedef struct spec_nds_move spec_nds_move_t;

struct spec_nds_contest {
    uint8_t stats[SPEC_NDS_CONTEST_CATEGORY_COUNT];
    uint8_t sheen;
};
typedef struct spec_nds_contest spec_nds_contest_t;

struct spec_nds_pokerus {
    uint8_t strain;
    uint8_t days;
};
typedef struct spec_nds_pokerus spec_nds_pokerus_t;

struct spec_nds_status {
    uint8_t sleep_turns;
    bool is_poisoned;
    bool is_burned;
    bool is_frozen;
    bool is_paralyzed;
    bool is_badly_poisoned;
    uint8_t toxic_turns;
};
typedef struct spec_nds_status spec_nds_status_t;

// The form is Platinum's and later; Diamond and Pearl leave it 0.
struct spec_nds_mail_icon {
    uint16_t sprite;
    uint8_t palette;
    uint8_t form;
};
typedef struct spec_nds_mail_icon spec_nds_mail_icon_t;

struct spec_nds_mail_sentence {
    uint16_t type;
    uint16_t id;
    uint16_t words[SPEC_NDS_MAIL_WORD_COUNT];
};
typedef struct spec_nds_mail_sentence spec_nds_mail_sentence_t;

// A type of 0xFF is no mail.
struct spec_nds_mail {
    spec_nds_trainer_t author;
    spec_language_t language;
    spec_version_t version;
    uint8_t type;
    spec_nds_mail_icon_t icons[SPEC_NDS_MAIL_ICON_COUNT];
    spec_nds_mail_sentence_t sentences[SPEC_NDS_MAIL_SENTENCE_COUNT];
};
typedef struct spec_nds_mail spec_nds_mail_t;

struct spec_nds_seal {
    uint8_t type;
    uint8_t x;
    uint8_t y;
};
typedef struct spec_nds_seal spec_nds_seal_t;

// Zero for boxed Pokémon; filled in when written to the party.
struct spec_nds_party_data {
    spec_nds_status_t status;
    uint8_t level;
    uint8_t ball_capsule;
    uint16_t current_hp;
    uint16_t stats[SPEC_STAT_COUNT];
    spec_nds_mail_t mail;
    spec_nds_seal_t seals[SPEC_NDS_SEAL_COUNT];
};
typedef struct spec_nds_party_data spec_nds_party_data_t;

// Names are game-encoded; species is the National Dex number.
struct spec_nds_pokemon {
    uint16_t species;
    uint8_t form;
    spec_nds_personality_t personality;
    uint16_t nickname[SPEC_NDS_NICKNAME_SIZE];
    bool is_nicknamed;
    spec_language_t language;
    spec_nds_trainer_t trainer;
    spec_nds_origin_t origin;

    uint32_t experience;
    uint8_t friendship;
    uint16_t held_item;
    uint8_t ability;
    uint8_t ivs[SPEC_STAT_COUNT];
    // Derived on read.
    spec_iv_method_t iv_method;
    uint8_t evs[SPEC_STAT_COUNT];
    spec_nds_move_t moves[SPEC_NDS_MOVE_COUNT];

    spec_nds_contest_t contest;
    uint32_t sinnoh_ribbons;
    uint32_t hoenn_ribbons;
    uint32_t super_contest_ribbons;
    uint8_t markings;
    spec_nds_pokerus_t pokerus;
    uint8_t shiny_leaves; // HeartGold and SoulSilver only
    int8_t walking_mood;  // HeartGold and SoulSilver only

    bool is_egg;
    bool is_bad_egg;
    bool is_fateful_encounter;

    spec_nds_party_data_t party_data;
};
typedef struct spec_nds_pokemon spec_nds_pokemon_t;

struct spec_nds_play_time {
    uint16_t hours;
    uint8_t minutes;
    uint8_t seconds;
};
typedef struct spec_nds_play_time spec_nds_play_time_t;

struct spec_nds_options {
    spec_nds_button_mode_t button_mode;
    spec_nds_text_speed_t text_speed;
    uint8_t window_frame;
    bool is_stereo;
    bool is_battle_style_set;
    bool is_battle_scene_off;
};
typedef struct spec_nds_options spec_nds_options_t;

struct spec_nds_form_order {
    uint8_t count;
    uint8_t forms[SPEC_NDS_FORM_ORDER_MAX];
};
typedef struct spec_nds_form_order spec_nds_form_order_t;

// Forms in the order the Pokédex first saw them.
struct spec_nds_pokedex_forms {
    spec_nds_form_order_t unown;
    spec_nds_form_order_t unown_caught; // HeartGold and SoulSilver only
    spec_nds_form_order_t deoxys;
    spec_nds_form_order_t shellos;
    spec_nds_form_order_t gastrodon;
    spec_nds_form_order_t burmy;
    spec_nds_form_order_t wormadam;
    spec_nds_form_order_t rotom;    // not Diamond and Pearl
    spec_nds_form_order_t shaymin;  // not Diamond and Pearl
    spec_nds_form_order_t giratina; // not Diamond and Pearl
    spec_nds_form_order_t pichu;    // HeartGold and SoulSilver only
};
typedef struct spec_nds_pokedex_forms spec_nds_pokedex_forms_t;

// Writing is_caught also marks the species seen. languages holds spec_nds_pokedex_language_t
// flags; Diamond and Pearl record them for 14 species only.
struct spec_nds_pokedex {
    bool is_obtained;
    bool has_national_dex;
    bool can_view_forms;
    bool can_view_languages;
    uint32_t spinda_personality;
    bool is_seen[SPEC_NDS_POKEDEX_SIZE];
    bool is_caught[SPEC_NDS_POKEDEX_SIZE];
    spec_gender_t first_seen_gender[SPEC_NDS_POKEDEX_SIZE];
    bool has_seen_both_genders[SPEC_NDS_POKEDEX_SIZE];
    uint8_t languages[SPEC_NDS_POKEDEX_SIZE];
    spec_nds_pokedex_forms_t forms;
};
typedef struct spec_nds_pokedex spec_nds_pokedex_t;

struct spec_nds_box {
    uint16_t name[SPEC_NDS_BOX_NAME_SIZE];
    uint8_t wallpaper;
    spec_nds_pokemon_t pokemon[SPEC_NDS_BOX_CAPACITY];
};
typedef struct spec_nds_box spec_nds_box_t;

struct spec_nds_daycare_slot {
    spec_nds_pokemon_t pokemon;
    uint32_t steps;
};
typedef struct spec_nds_daycare_slot spec_nds_daycare_slot_t;

struct spec_nds_daycare {
    spec_nds_daycare_slot_t slots[SPEC_NDS_DAYCARE_CAPACITY];
    uint32_t egg_personality;
    uint8_t step_counter;
};
typedef struct spec_nds_daycare spec_nds_daycare_t;

typedef spec_item_slot_t spec_nds_item_slot_t;

// Writing condenses pockets; a slot with no item or zero quantity is empty.
struct spec_nds_save {
    spec_game_type_t type;
    spec_language_t language;
    spec_nds_trainer_t trainer;
    spec_nds_play_time_t play_time;
    uint32_t money;
    uint16_t coins;
    uint16_t battle_points;
    bool badges[SPEC_NDS_BADGE_COUNT];
    uint16_t rival_name[SPEC_NDS_TRAINER_NAME_SIZE];
    spec_nds_options_t options;
    spec_nds_pokedex_t pokedex;
    uint8_t party_count;
    spec_nds_pokemon_t party[SPEC_NDS_PARTY_CAPACITY];
    uint8_t current_box;
    spec_nds_box_t boxes[SPEC_NDS_BOX_COUNT];
    spec_nds_daycare_t daycare;
    spec_nds_item_slot_t items[SPEC_NDS_POCKET_COUNT][SPEC_NDS_POCKET_MAX_CAPACITY];
};
typedef struct spec_nds_save spec_nds_save_t;

spec_error_t spec_nds_read_save(spec_nds_save_t *save,
                                const uint8_t data[static SPEC_NDS_SAVE_SIZE]);
spec_error_t spec_nds_write_save(const spec_nds_save_t *save,
                                 uint8_t data[static SPEC_NDS_SAVE_SIZE]);

// raw is the encrypted record as the save stores it.
spec_error_t spec_nds_read_pokemon(spec_nds_pokemon_t *pokemon, const uint8_t *raw,
                                   size_t raw_size);
spec_error_t spec_nds_write_pokemon(uint8_t *raw, size_t raw_size,
                                    const spec_nds_pokemon_t *pokemon);
spec_error_t spec_nds_pokemon_calculate_stats(spec_nds_pokemon_t *pokemon);
spec_error_t spec_nds_pokemon_get_name(const spec_nds_pokemon_t *pokemon,
                                       char8_t name[static SPEC_NDS_TEXT_BUFFER_SIZE]);
spec_error_t spec_nds_pokemon_set_name(spec_nds_pokemon_t *pokemon, const char8_t *name);
bool spec_nds_is_safe_to_box(const spec_nds_pokemon_t *pokemon);

spec_nds_personality_t spec_nds_decode_personality(spec_pid_t pid, uint16_t species,
                                                   const spec_nds_trainer_t *trainer);

const char *spec_nds_item_name(uint16_t item, spec_language_t language);
spec_error_t spec_nds_get_pocket_for_item(spec_nds_pocket_t *pocket, spec_game_type_t type,
                                          uint16_t item);
size_t spec_nds_pocket_capacity(spec_game_type_t type, spec_nds_pocket_t pocket);
size_t spec_nds_pocket_item_count(const spec_nds_save_t *save, spec_nds_pocket_t pocket);

spec_error_t spec_nds_text_to_utf8(char8_t utf8[static SPEC_NDS_TEXT_BUFFER_SIZE],
                                   const uint16_t *text, size_t text_size);
spec_error_t spec_nds_text_from_utf8(uint16_t *text, size_t text_size, const char8_t *utf8,
                                     spec_language_t language);

#endif
