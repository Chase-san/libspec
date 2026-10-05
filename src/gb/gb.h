// The Gen 1 (Game Boy) API: saves, Pokémon, species, items and text.

#ifndef SPEC_GB_H
#define SPEC_GB_H

#include <stddef.h>
#include <stdint.h>
#include <uchar.h>

#include "spec.h"

constexpr size_t SPEC_GB_SAVE_SIZE = 0x8000;
constexpr size_t SPEC_GB_BADGE_COUNT = 8;
// Name sizes count the terminator; Japanese names use the first 6 bytes.
constexpr size_t SPEC_GB_NAME_SIZE = 11;
constexpr size_t SPEC_GB_JAPANESE_NAME_SIZE = 6;
constexpr size_t SPEC_GB_TYPE_COUNT = 2;
constexpr size_t SPEC_GB_MOVE_COUNT = 4;
constexpr size_t SPEC_GB_PARTY_RECORD_SIZE = 44;
constexpr size_t SPEC_GB_BOX_RECORD_SIZE = 33;
// Indexed by National Dex number.
constexpr size_t SPEC_GB_POKEDEX_SIZE = 152;
constexpr size_t SPEC_GB_PARTY_CAPACITY = 6;
// Japanese saves hold 8 boxes of 30, the others 12 of 20.
constexpr size_t SPEC_GB_BOX_COUNT = 12;
constexpr size_t SPEC_GB_BOX_CAPACITY = 30;
constexpr size_t SPEC_GB_POCKET_MAX_CAPACITY = 50;
constexpr size_t SPEC_GB_TEXT_MAX_SIZE = SPEC_GB_NAME_SIZE;
// Up to 4 UTF-8 bytes per byte, a ligature being two characters, plus NUL.
constexpr size_t SPEC_GB_TEXT_BUFFER_SIZE = SPEC_GB_TEXT_MAX_SIZE * 4 + 1;

// The player

struct spec_gb_trainer {
    uint8_t name[SPEC_GB_NAME_SIZE];
    uint16_t id;
};
typedef struct spec_gb_trainer spec_gb_trainer_t;

struct spec_gb_play_time {
    uint16_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t frames;
};
typedef struct spec_gb_play_time spec_gb_play_time_t;

// Pokémon

// The game's own index, not the National Dex number.
typedef uint8_t spec_gb_species_t;

// In the records' order; Special serves both special stats.
enum spec_gb_stat {
    SPEC_GB_STAT_HP,
    SPEC_GB_STAT_ATTACK,
    SPEC_GB_STAT_DEFENSE,
    SPEC_GB_STAT_SPEED,
    SPEC_GB_STAT_SPECIAL,
    SPEC_GB_STAT_COUNT,
};
typedef enum spec_gb_stat spec_gb_stat_t;

struct spec_gb_move {
    uint8_t id;
    uint8_t pp;
    uint8_t pp_ups;
};
typedef struct spec_gb_move spec_gb_move_t;

struct spec_gb_status {
    uint8_t sleep_turns;
    bool is_poisoned;
    bool is_burned;
    bool is_frozen;
    bool is_paralyzed;
};
typedef struct spec_gb_status spec_gb_status_t;

// Zero for boxed Pokémon; filled in when written to the party.
struct spec_gb_party_data {
    uint8_t level;
    uint16_t stats[SPEC_GB_STAT_COUNT];
};
typedef struct spec_gb_party_data spec_gb_party_data_t;

// Names are game-encoded in the save's language.
struct spec_gb_pokemon {
    spec_gb_species_t species;
    uint8_t nickname[SPEC_GB_NAME_SIZE];
    spec_gb_trainer_t trainer;

    uint32_t experience;
    // What boxes show; a party Pokémon keeps the level it last had in a box.
    uint8_t box_level;
    uint16_t current_hp;
    spec_gb_status_t status;
    uint8_t types[SPEC_GB_TYPE_COUNT];
    uint8_t catch_rate;
    // HP's is derived from the others on read and ignored on write.
    uint8_t dvs[SPEC_GB_STAT_COUNT];
    uint16_t stat_experience[SPEC_GB_STAT_COUNT];
    spec_gb_move_t moves[SPEC_GB_MOVE_COUNT];

    spec_gb_party_data_t party_data;
};
typedef struct spec_gb_pokemon spec_gb_pokemon_t;

// The Pokédex

// Writing is_caught also marks the species seen.
struct spec_gb_pokedex {
    bool is_obtained;
    bool is_seen[SPEC_GB_POKEDEX_SIZE];
    bool is_caught[SPEC_GB_POKEDEX_SIZE];
};
typedef struct spec_gb_pokedex spec_gb_pokedex_t;

// Storage

// A box holds its first count Pokémon.
struct spec_gb_box {
    uint8_t count;
    spec_gb_pokemon_t pokemon[SPEC_GB_BOX_CAPACITY];
};
typedef struct spec_gb_box spec_gb_box_t;

// Items

enum spec_gb_pocket {
    SPEC_GB_POCKET_BAG,
    SPEC_GB_POCKET_PC,
    SPEC_GB_POCKET_COUNT,
};
typedef enum spec_gb_pocket spec_gb_pocket_t;

typedef spec_item_slot_t spec_gb_item_slot_t;

// The save

// The save records neither its game nor its language, so reading takes both. Writing condenses
// pockets; a slot with no item or zero quantity is empty.
struct spec_gb_save {
    spec_game_type_t type;
    spec_language_t language;
    spec_gb_trainer_t trainer;
    uint8_t rival_name[SPEC_GB_NAME_SIZE];
    spec_gb_play_time_t play_time;
    uint32_t money;
    uint16_t coins;
    bool badges[SPEC_GB_BADGE_COUNT];
    uint8_t pikachu_friendship; // Yellow only
    spec_gb_pokedex_t pokedex;
    uint8_t party_count;
    spec_gb_pokemon_t party[SPEC_GB_PARTY_CAPACITY];
    uint8_t current_box;
    spec_gb_box_t boxes[SPEC_GB_BOX_COUNT];
    spec_gb_pokemon_t daycare; // species 0 when empty
    spec_gb_item_slot_t items[SPEC_GB_POCKET_COUNT][SPEC_GB_POCKET_MAX_CAPACITY];
};
typedef struct spec_gb_save spec_gb_save_t;

// Save functions

spec_error_t spec_gb_read_save(spec_gb_save_t *save, const uint8_t data[static SPEC_GB_SAVE_SIZE],
                               spec_game_type_t type, spec_language_t language);
spec_error_t spec_gb_write_save(const spec_gb_save_t *save, uint8_t data[static SPEC_GB_SAVE_SIZE]);

// What writing checks, without writing.
spec_error_t spec_gb_check_save(const spec_gb_save_t *save);

// Pokémon functions: a Pokémon has no language of its own, so its names take the save's.

spec_error_t spec_gb_pokemon_get_name(const spec_gb_pokemon_t *pokemon,
                                      char8_t name[static SPEC_GB_TEXT_BUFFER_SIZE],
                                      spec_language_t language);

spec_error_t spec_gb_pokemon_calculate_stats(spec_gb_pokemon_t *pokemon);
spec_error_t spec_gb_pokemon_remove_nickname(spec_gb_pokemon_t *pokemon, spec_language_t language);
spec_error_t spec_gb_pokemon_set_level(spec_gb_pokemon_t *pokemon, uint8_t level);
spec_error_t spec_gb_pokemon_set_nickname(spec_gb_pokemon_t *pokemon, const char8_t *nickname,
                                          spec_language_t language);

// Species functions

spec_gb_species_t spec_gb_species_from_national(uint16_t national_number);
uint16_t spec_gb_species_to_national(spec_gb_species_t species);

// Item functions

spec_error_t spec_gb_check_item_placement(spec_gb_pocket_t pocket, uint16_t item);
const char *spec_gb_item_name(uint16_t item, spec_language_t language);
size_t spec_gb_pocket_capacity(spec_gb_pocket_t pocket);
size_t spec_gb_pocket_item_count(const spec_gb_save_t *save, spec_gb_pocket_t pocket);

// Text functions

spec_error_t spec_gb_text_from_utf8(uint8_t *text, size_t text_size, const char8_t *utf8,
                                    spec_language_t language);
spec_error_t spec_gb_text_to_utf8(char8_t utf8[static SPEC_GB_TEXT_BUFFER_SIZE],
                                  const uint8_t *text, size_t text_size, spec_language_t language);

#endif
