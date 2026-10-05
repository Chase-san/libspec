// The Gen 2 (Game Boy Color) API: saves, Pokémon, traits, items and text.

#ifndef SPEC_GBC_H
#define SPEC_GBC_H

#include <stddef.h>
#include <stdint.h>
#include <uchar.h>

#include "gb/gb.h"
#include "spec.h"

constexpr size_t SPEC_GBC_SAVE_SIZE = 0x8000;
constexpr size_t SPEC_GBC_JAPANESE_CRYSTAL_SAVE_SIZE = 0x10000;
constexpr size_t SPEC_GBC_BADGE_COUNT = 8;
// Name sizes count the terminator; Japanese names use the first 6 bytes.
constexpr size_t SPEC_GBC_NAME_SIZE = SPEC_GB_NAME_SIZE;
constexpr size_t SPEC_GBC_BOX_NAME_SIZE = 9;
constexpr size_t SPEC_GBC_MOVE_COUNT = 4;
// Japanese mail's author uses the first 5 bytes and has no nationality.
constexpr size_t SPEC_GBC_MAIL_MESSAGE_SIZE = 33;
constexpr size_t SPEC_GBC_MAIL_AUTHOR_SIZE = 8;
constexpr size_t SPEC_GBC_MAIL_NATIONALITY_SIZE = 2;
constexpr size_t SPEC_GBC_PARTY_RECORD_SIZE = 48;
constexpr size_t SPEC_GBC_BOX_RECORD_SIZE = 32;
// Indexed by National Dex number.
constexpr size_t SPEC_GBC_POKEDEX_SIZE = 252;
constexpr size_t SPEC_GBC_UNOWN_FORM_COUNT = 26;
constexpr size_t SPEC_GBC_PARTY_CAPACITY = 6;
// Japanese saves hold 9 boxes of 30, the others 14 of 20.
constexpr size_t SPEC_GBC_BOX_COUNT = 14;
constexpr size_t SPEC_GBC_BOX_CAPACITY = 30;
constexpr size_t SPEC_GBC_DAYCARE_CAPACITY = 2;
constexpr size_t SPEC_GBC_POCKET_MAX_CAPACITY = 57;
constexpr size_t SPEC_GBC_TEXT_BUFFER_SIZE = SPEC_GB_TEXT_BUFFER_SIZE;

// The player

// Only Crystal records a trainer's gender, in its Pokémon and for its player.
struct spec_gbc_trainer {
    uint8_t name[SPEC_GBC_NAME_SIZE];
    uint16_t id;
    bool is_female;
};
typedef struct spec_gbc_trainer spec_gbc_trainer_t;

typedef spec_gb_play_time_t spec_gbc_play_time_t;

// Pokémon

typedef spec_gb_move_t spec_gbc_move_t;
typedef spec_gb_status_t spec_gbc_status_t;

struct spec_gbc_pokerus {
    uint8_t strain;
    uint8_t days;
};
typedef struct spec_gbc_pokerus spec_gbc_pokerus_t;

// What the DVs decide, derived on read and ignored on write.
struct spec_gbc_traits {
    spec_gender_t gender;
    bool is_shiny;
    uint8_t unown_form;
};
typedef struct spec_gbc_traits spec_gbc_traits_t;

enum spec_gbc_time_of_day : uint8_t {
    SPEC_GBC_TIME_OF_DAY_NONE,
    SPEC_GBC_TIME_OF_DAY_MORNING,
    SPEC_GBC_TIME_OF_DAY_DAY,
    SPEC_GBC_TIME_OF_DAY_NIGHT,
};
typedef enum spec_gbc_time_of_day spec_gbc_time_of_day_t;

// Crystal writes it; Gold and Silver keep what trades bring.
struct spec_gbc_caught {
    spec_gbc_time_of_day_t time_of_day;
    uint8_t level;
    uint8_t location;
};
typedef struct spec_gbc_caught spec_gbc_caught_t;

// Text is game-encoded; the message's two lines keep their line break and terminator.
struct spec_gbc_mail {
    uint8_t message[SPEC_GBC_MAIL_MESSAGE_SIZE];
    uint8_t author_name[SPEC_GBC_MAIL_AUTHOR_SIZE];
    uint8_t nationality[SPEC_GBC_MAIL_NATIONALITY_SIZE];
    uint16_t author_id;
    uint8_t species;
    uint8_t type;
};
typedef struct spec_gbc_mail spec_gbc_mail_t;

// Zero for boxed Pokémon; filled in when written to the party. Only a Pokémon holding mail has
// any.
struct spec_gbc_party_data {
    spec_gbc_status_t status;
    uint16_t current_hp;
    uint16_t stats[SPEC_STAT_COUNT];
    spec_gbc_mail_t mail;
};
typedef struct spec_gbc_party_data spec_gbc_party_data_t;

// Names are game-encoded in the save's language; species is the National Dex number, an egg's
// what hatches. dvs and stat_experience take spec_gb_stat_t, the party stats spec_stat_t.
struct spec_gbc_pokemon {
    uint8_t species;
    uint8_t nickname[SPEC_GBC_NAME_SIZE];
    spec_gbc_trainer_t trainer;
    bool is_egg;

    uint32_t experience;
    uint8_t level;
    uint8_t held_item;
    // An egg's counts the cycles left until it hatches.
    uint8_t friendship;
    // HP's is derived from the others on read and ignored on write.
    uint8_t dvs[SPEC_GB_STAT_COUNT];
    spec_gbc_traits_t traits;
    uint16_t stat_experience[SPEC_GB_STAT_COUNT];
    spec_gbc_move_t moves[SPEC_GBC_MOVE_COUNT];
    spec_gbc_pokerus_t pokerus;
    spec_gbc_caught_t caught;

    spec_gbc_party_data_t party_data;
};
typedef struct spec_gbc_pokemon spec_gbc_pokemon_t;

// The Pokédex

// Writing is_caught also marks the species seen. The Unown lists hold forms + 1: those caught,
// in the order first caught, 0 ending it; and the first seen, 0 before any.
struct spec_gbc_pokedex {
    bool is_obtained;
    bool has_unown_dex;
    bool is_seen[SPEC_GBC_POKEDEX_SIZE];
    bool is_caught[SPEC_GBC_POKEDEX_SIZE];
    uint8_t unown_caught_order[SPEC_GBC_UNOWN_FORM_COUNT];
    uint8_t first_unown_seen;
};
typedef struct spec_gbc_pokedex spec_gbc_pokedex_t;

// Storage

// A box holds its first count Pokémon.
struct spec_gbc_box {
    uint8_t name[SPEC_GBC_BOX_NAME_SIZE];
    uint8_t count;
    spec_gbc_pokemon_t pokemon[SPEC_GBC_BOX_CAPACITY];
};
typedef struct spec_gbc_box spec_gbc_box_t;

// The man's parent, then the lady's, species 0 when empty; the egg is the next one, built when
// both arrive. mother_index is the parent the egg's species follows: 0 the man's, 1 the lady's.
struct spec_gbc_daycare {
    spec_gbc_pokemon_t parents[SPEC_GBC_DAYCARE_CAPACITY];
    bool are_parents_compatible;
    bool is_egg_waiting;
    uint8_t steps_to_egg;
    uint8_t mother_index;
    spec_gbc_pokemon_t egg;
};
typedef struct spec_gbc_daycare spec_gbc_daycare_t;

// Items

// In the games' pocket order, then the PC.
enum spec_gbc_pocket {
    SPEC_GBC_POCKET_ITEMS,
    SPEC_GBC_POCKET_BALLS,
    SPEC_GBC_POCKET_KEY_ITEMS,
    SPEC_GBC_POCKET_TMS_HMS,
    SPEC_GBC_POCKET_PC,
    SPEC_GBC_POCKET_COUNT,
};
typedef enum spec_gbc_pocket spec_gbc_pocket_t;

typedef spec_item_slot_t spec_gbc_item_slot_t;

// The save

// The save records neither its game nor its language, so reading takes both. Writing condenses
// pockets; a slot with no item or zero quantity is empty, and a key item's quantity is 1.
struct spec_gbc_save {
    spec_game_type_t type;
    spec_language_t language;
    spec_gbc_trainer_t trainer;
    uint8_t rival_name[SPEC_GBC_NAME_SIZE];
    spec_gbc_play_time_t play_time;
    uint32_t money;
    uint32_t moms_money;
    uint16_t coins;
    bool johto_badges[SPEC_GBC_BADGE_COUNT];
    bool kanto_badges[SPEC_GBC_BADGE_COUNT];
    spec_gbc_pokedex_t pokedex;
    uint8_t party_count;
    spec_gbc_pokemon_t party[SPEC_GBC_PARTY_CAPACITY];
    uint8_t current_box;
    spec_gbc_box_t boxes[SPEC_GBC_BOX_COUNT];
    spec_gbc_daycare_t daycare;
    spec_gbc_item_slot_t items[SPEC_GBC_POCKET_COUNT][SPEC_GBC_POCKET_MAX_CAPACITY];
};
typedef struct spec_gbc_save spec_gbc_save_t;

// Save functions: data_size is SPEC_GBC_SAVE_SIZE, or SPEC_GBC_JAPANESE_CRYSTAL_SAVE_SIZE for
// Japanese Crystal.

spec_error_t spec_gbc_read_save(spec_gbc_save_t *save, const uint8_t *data, size_t data_size,
                                spec_game_type_t type, spec_language_t language);
spec_error_t spec_gbc_write_save(const spec_gbc_save_t *save, uint8_t *data, size_t data_size);

// What writing checks, without writing.
spec_error_t spec_gbc_check_save(const spec_gbc_save_t *save);

// Pokémon functions: a Pokémon has no language of its own, so its names take the save's.

spec_error_t spec_gbc_pokemon_get_name(const spec_gbc_pokemon_t *pokemon,
                                       char8_t name[static SPEC_GBC_TEXT_BUFFER_SIZE],
                                       spec_language_t language);
bool spec_gbc_pokemon_is_safe_to_box(const spec_gbc_pokemon_t *pokemon);

spec_error_t spec_gbc_pokemon_calculate_stats(spec_gbc_pokemon_t *pokemon);
spec_error_t spec_gbc_pokemon_remove_nickname(spec_gbc_pokemon_t *pokemon,
                                              spec_language_t language);
spec_error_t spec_gbc_pokemon_set_level(spec_gbc_pokemon_t *pokemon, uint8_t level);
spec_error_t spec_gbc_pokemon_set_nickname(spec_gbc_pokemon_t *pokemon, const char8_t *nickname,
                                           spec_language_t language);

// Trait functions

spec_gbc_traits_t spec_gbc_decode_traits(const uint8_t dvs[static SPEC_GB_STAT_COUNT],
                                         uint8_t species);

// Item functions

spec_error_t spec_gbc_check_item_placement(spec_game_type_t type, spec_gbc_pocket_t pocket,
                                           uint16_t item);
spec_error_t spec_gbc_get_pocket_for_item(spec_gbc_pocket_t *pocket, spec_game_type_t type,
                                          uint16_t item);
const char *spec_gbc_item_name(uint16_t item, spec_language_t language);
size_t spec_gbc_pocket_capacity(spec_gbc_pocket_t pocket);
size_t spec_gbc_pocket_item_count(const spec_gbc_save_t *save, spec_gbc_pocket_t pocket);

// Text functions

spec_error_t spec_gbc_text_from_utf8(uint8_t *text, size_t text_size, const char8_t *utf8,
                                     spec_language_t language);
spec_error_t spec_gbc_text_to_utf8(char8_t utf8[static SPEC_GBC_TEXT_BUFFER_SIZE],
                                   const uint8_t *text, size_t text_size, spec_language_t language);

#endif
