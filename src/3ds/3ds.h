// The Gen 6 and 7 (3DS) API: saves, storage, Pokémon, species, personality, items and text.

#ifndef SPEC_3DS_H
#define SPEC_3DS_H

#include <stddef.h>
#include <stdint.h>

#include "nds/nds.h"
#include "spec.h"

// Omega Ruby and Alpha Sapphire's, the largest; every game type's save has a size of its own.
constexpr size_t SPEC_3DS_SAVE_MAX_SIZE = 0x76000;
constexpr size_t SPEC_3DS_PARTY_CAPACITY = 6;
// Gen 6 has 31 boxes, Gen 7 32.
constexpr size_t SPEC_3DS_BOX_MAX_COUNT = 32;
constexpr size_t SPEC_3DS_BOX_CAPACITY = 30;
constexpr size_t SPEC_3DS_PARTY_RECORD_SIZE = 0x104;
constexpr size_t SPEC_3DS_BOX_RECORD_SIZE = 0xE8;
// Text sizes count UTF-16 units, the terminator's included.
constexpr size_t SPEC_3DS_NAME_SIZE = 13;
constexpr size_t SPEC_3DS_BOX_NAME_SIZE = 17;
constexpr size_t SPEC_3DS_MOVE_COUNT = 4;
// The first, the second and the hidden, which ability numbers 1, 2 and 4 choose.
constexpr size_t SPEC_3DS_SPECIES_ABILITY_COUNT = 3;
constexpr size_t SPEC_3DS_BADGE_COUNT = 8;
// The trainer passport's stamps, which stand in for Gen 7's badges.
constexpr size_t SPEC_3DS_STAMP_COUNT = 15;
constexpr size_t SPEC_3DS_MARKING_COUNT = 6;
constexpr size_t SPEC_3DS_GEO_MEMORY_COUNT = 5;
constexpr size_t SPEC_3DS_DAYCARE_CAPACITY = 2;
// Omega Ruby and Alpha Sapphire have a second daycare, on the Battle Resort.
constexpr size_t SPEC_3DS_DAYCARE_MAX_COUNT = 2;
// Indexed by National Dex number.
constexpr size_t SPEC_3DS_POKEDEX_SIZE = 808;
constexpr size_t SPEC_3DS_POCKET_MAX_CAPACITY = 430;
// Item numbers run below this.
constexpr size_t SPEC_3DS_ITEM_COUNT = 960;
constexpr size_t SPEC_3DS_TEXT_MAX_SIZE = SPEC_3DS_BOX_NAME_SIZE;
// Up to 3 UTF-8 bytes per character, plus NUL.
constexpr size_t SPEC_3DS_TEXT_BUFFER_SIZE = SPEC_3DS_TEXT_MAX_SIZE * 3 + 1;

// The player

struct spec_3ds_trainer {
    uint16_t name[SPEC_3DS_NAME_SIZE];
    uint16_t id;
    uint16_t secret_id;
    bool is_female;
};
typedef struct spec_3ds_trainer spec_3ds_trainer_t;

// The 3DS's own settings, which a Pokémon new to the save takes as its own.
struct spec_3ds_region {
    uint8_t country;
    uint8_t subregion;
    uint8_t console_region;
};
typedef struct spec_3ds_region spec_3ds_region_t;

typedef spec_nds_play_time_t spec_3ds_play_time_t;

typedef spec_nds_text_speed_t spec_3ds_text_speed_t;

// The other bits of the options word are kept as the game wrote them.
struct spec_3ds_options {
    spec_3ds_text_speed_t text_speed;
    bool is_battle_scene_off;
    bool is_battle_style_set;
    uint8_t button_mode;
};
typedef struct spec_3ds_options spec_3ds_options_t;

// Pokémon

// Only pid is stored; is_shiny is derived on read. Nature and gender are stored apart.
struct spec_3ds_personality {
    spec_pid_t pid;
    bool is_shiny;
};
typedef struct spec_3ds_personality spec_3ds_personality_t;

typedef spec_nds_date_t spec_3ds_date_t;

// A met_level of 0 means hatched. encounter_type is Gen 6's, and only Gen 4's Pokémon have one.
struct spec_3ds_origin {
    spec_version_t version;
    spec_ball_t ball;
    uint8_t met_level;
    uint16_t met_location;
    spec_3ds_date_t met_date;
    uint16_t egg_location;
    spec_3ds_date_t egg_date;
    uint8_t encounter_type;
};
typedef struct spec_3ds_origin spec_3ds_origin_t;

struct spec_3ds_memory {
    uint8_t intensity;
    uint8_t memory;
    uint8_t feeling;
    uint16_t variable;
};
typedef struct spec_3ds_memory spec_3ds_memory_t;

// How a Pokémon feels about one of its trainers.
struct spec_3ds_bond {
    uint8_t friendship;
    uint8_t affection;
    spec_3ds_memory_t memory;
};
typedef struct spec_3ds_bond spec_3ds_bond_t;

// The last trainer other than the original one to hold the Pokémon. Retail saves hold a
// genderless one, with no name.
struct spec_3ds_latest_trainer {
    uint16_t name[SPEC_3DS_NAME_SIZE];
    spec_gender_t gender;
    spec_3ds_bond_t bond;
};
typedef struct spec_3ds_latest_trainer spec_3ds_latest_trainer_t;

// Gen 6's record of where the Pokémon has been; Gen 7 clears it on arrival.
struct spec_3ds_geo_memory {
    uint8_t subregion;
    uint8_t country;
};
typedef struct spec_3ds_geo_memory spec_3ds_geo_memory_t;

typedef spec_nds_move_t spec_3ds_move_t;
typedef spec_nds_contest_category_t spec_3ds_contest_category_t;
typedef spec_nds_contest_t spec_3ds_contest_t;
typedef spec_nds_pokerus_t spec_3ds_pokerus_t;
typedef spec_nds_status_t spec_3ds_status_t;

// Gen 6's Super Training, which Gen 7 keeps, and Gen 6's training bags.
struct spec_3ds_training {
    uint32_t regimens;
    uint16_t distribution_regimens;
    bool is_secret_unlocked;
    bool is_complete;
    uint8_t bag;
    uint8_t bag_hits;
};
typedef struct spec_3ds_training spec_3ds_training_t;

// Zero for boxed Pokémon; filled in when written to the party. Gen 6 keeps a Furfrou's trim days
// here, Gen 7 Pokémon Refresh's dirt.
struct spec_3ds_party_data {
    spec_3ds_status_t status;
    uint8_t level;
    uint16_t current_hp;
    uint16_t stats[SPEC_STAT_COUNT];
    uint8_t form_days_remaining;
    uint8_t form_days_elapsed;
    uint8_t training_bag_effect;
    uint8_t dirt_type;
    uint8_t dirt_location;
};
typedef struct spec_3ds_party_data spec_3ds_party_data_t;

// Names are UTF-16; species is the National Dex number. generation is the record's format: 6
// for X to Alpha Sapphire, 7 for Sun to Ultra Moon. A marking is 0 or 1 in Gen 6, and 0, 1 for
// blue or 2 for pink in Gen 7. Ribbon n is bit n of the record's ribbon bytes. Gen 7 packs the
// form's days remaining, elapsed and most into form_argument's low three bytes.
struct spec_3ds_pokemon {
    uint8_t generation;
    uint16_t species;
    uint8_t form;
    uint32_t form_argument;
    uint32_t encryption_constant;
    spec_3ds_personality_t personality;
    spec_nature_t nature;
    spec_gender_t gender;
    uint16_t nickname[SPEC_3DS_NAME_SIZE];
    bool is_nicknamed;
    spec_language_t language;
    spec_3ds_trainer_t trainer;
    spec_3ds_bond_t trainer_bond;
    spec_3ds_latest_trainer_t latest_trainer;
    bool is_with_latest_trainer;
    spec_3ds_origin_t origin;
    spec_3ds_region_t region;
    spec_3ds_geo_memory_t geo_memories[SPEC_3DS_GEO_MEMORY_COUNT];

    uint32_t experience;
    uint16_t held_item;
    uint8_t ability;
    uint8_t ability_number; // 1, 2, or 4 for the hidden ability
    uint8_t ivs[SPEC_STAT_COUNT];
    bool hyper_trained[SPEC_STAT_COUNT]; // Gen 7 only
    uint8_t evs[SPEC_STAT_COUNT];
    spec_3ds_move_t moves[SPEC_3DS_MOVE_COUNT];
    uint16_t relearn_moves[SPEC_3DS_MOVE_COUNT];

    spec_3ds_contest_t contest;
    uint64_t ribbons;
    uint8_t contest_memory_ribbon_count;
    uint8_t battle_memory_ribbon_count;
    uint8_t markings[SPEC_3DS_MARKING_COUNT];
    spec_3ds_pokerus_t pokerus;
    spec_3ds_training_t training;
    uint8_t fullness;
    uint8_t enjoyment;
    uint8_t resort_event_status; // Gen 7 only

    bool is_egg;
    bool is_bad_egg;
    bool is_fateful_encounter;

    spec_3ds_party_data_t party_data;
};
typedef struct spec_3ds_pokemon spec_3ds_pokemon_t;

// Species

// A species of one type has it twice, and of one ability has it as the first and second, as the
// games store them.
struct spec_3ds_species_data {
    uint8_t base_stats[SPEC_STAT_COUNT];
    spec_type_t types[SPEC_SPECIES_TYPE_COUNT];
    uint16_t abilities[SPEC_3DS_SPECIES_ABILITY_COUNT];
    uint8_t gender_ratio;
    uint8_t egg_cycles;
    uint8_t base_friendship;
    spec_growth_rate_t growth_rate;
};
typedef struct spec_3ds_species_data spec_3ds_species_data_t;

// The Pokédex

enum spec_3ds_pokedex_look : uint8_t {
    SPEC_3DS_POKEDEX_LOOK_MALE = 1 << 0,
    SPEC_3DS_POKEDEX_LOOK_FEMALE = 1 << 1,
    SPEC_3DS_POKEDEX_LOOK_SHINY_MALE = 1 << 2,
    SPEC_3DS_POKEDEX_LOOK_SHINY_FEMALE = 1 << 3,
};
typedef enum spec_3ds_pokedex_look spec_3ds_pokedex_look_t;

// The looks are spec_3ds_pokedex_look_t flags. A species seen displays one of its looks seen;
// retail saves hold exceptions, so neither that nor a caught species being seen is enforced.
struct spec_3ds_pokedex {
    bool is_caught[SPEC_3DS_POKEDEX_SIZE];
    uint8_t seen_looks[SPEC_3DS_POKEDEX_SIZE];
    uint8_t displayed_look[SPEC_3DS_POKEDEX_SIZE];
};
typedef struct spec_3ds_pokedex spec_3ds_pokedex_t;

// Storage

struct spec_3ds_box {
    uint16_t name[SPEC_3DS_BOX_NAME_SIZE];
    uint8_t wallpaper;
    spec_3ds_pokemon_t pokemon[SPEC_3DS_BOX_CAPACITY];
};
typedef struct spec_3ds_box spec_3ds_box_t;

// Only Gen 6's daycares count the experience a Pokémon gains there.
struct spec_3ds_daycare_slot {
    spec_3ds_pokemon_t pokemon;
    uint32_t experience_gained;
};
typedef struct spec_3ds_daycare_slot spec_3ds_daycare_slot_t;

struct spec_3ds_daycare {
    spec_3ds_daycare_slot_t slots[SPEC_3DS_DAYCARE_CAPACITY];
    bool is_egg_waiting;
};
typedef struct spec_3ds_daycare spec_3ds_daycare_t;

// Items

// In the games' bag order; Z-Crystals are Gen 7's, and Rotom Powers Ultra Sun and Ultra Moon's.
enum spec_3ds_pocket {
    SPEC_3DS_POCKET_ITEMS,
    SPEC_3DS_POCKET_KEY_ITEMS,
    SPEC_3DS_POCKET_TMS_HMS,
    SPEC_3DS_POCKET_MEDICINE,
    SPEC_3DS_POCKET_BERRIES,
    SPEC_3DS_POCKET_Z_CRYSTALS,
    SPEC_3DS_POCKET_ROTOM_POWERS,
    SPEC_3DS_POCKET_COUNT,
};
typedef enum spec_3ds_pocket spec_3ds_pocket_t;

// Gen 7 also keeps an item's place in the bag's free space and whether it is new; Gen 6 neither.
struct spec_3ds_item_slot {
    uint16_t item;
    uint16_t quantity;
    uint16_t free_space_index;
    bool is_new;
};
typedef struct spec_3ds_item_slot spec_3ds_item_slot_t;

// The save

// Gen 6 writing condenses pockets, a slot with no item or zero quantity being empty; Gen 7 keeps
// every slot where it is. A game lacking a field must leave it zero.
struct spec_3ds_save {
    spec_game_type_t type;
    spec_version_t version;
    spec_language_t language;
    spec_3ds_trainer_t trainer;
    spec_3ds_region_t region;
    spec_3ds_play_time_t play_time;
    uint32_t money;
    uint32_t battle_points;
    bool badges[SPEC_3DS_BADGE_COUNT];
    bool stamps[SPEC_3DS_STAMP_COUNT];
    spec_3ds_options_t options;
    spec_3ds_pokedex_t pokedex;
    uint8_t party_count;
    spec_3ds_pokemon_t party[SPEC_3DS_PARTY_CAPACITY];
    uint8_t current_box;
    spec_3ds_box_t boxes[SPEC_3DS_BOX_MAX_COUNT];
    spec_3ds_daycare_t daycares[SPEC_3DS_DAYCARE_MAX_COUNT];
    spec_3ds_item_slot_t items[SPEC_3DS_POCKET_COUNT][SPEC_3DS_POCKET_MAX_CAPACITY];
};
typedef struct spec_3ds_save spec_3ds_save_t;

// Save functions

// data is the plaintext main file a save manager or an emulator keeps.
spec_error_t spec_3ds_read_save(spec_3ds_save_t *save, const uint8_t *data, size_t data_size);
// Gen 7 saves are signed again, as the games sign them.
spec_error_t spec_3ds_write_save(const spec_3ds_save_t *save, uint8_t *data, size_t data_size);

// What writing checks, without writing.
spec_error_t spec_3ds_check_save(const spec_3ds_save_t *save);
// 0 for a type that is not a 3DS game's.
size_t spec_3ds_save_size(spec_game_type_t type);

// Storage functions: 0 for a type that is not a 3DS game's.

size_t spec_3ds_box_count(spec_game_type_t type);
size_t spec_3ds_daycare_count(spec_game_type_t type);

// Pokémon functions

// raw is the encrypted record as the save stores it; generation is its format, 6 or 7.
spec_error_t spec_3ds_read_pokemon(spec_3ds_pokemon_t *pokemon, const uint8_t *raw, size_t raw_size,
                                   uint8_t generation);
spec_error_t spec_3ds_write_pokemon(uint8_t *raw, size_t raw_size,
                                    const spec_3ds_pokemon_t *pokemon);

// The level its experience gives, which boxed records do not store.
uint8_t spec_3ds_pokemon_get_level(const spec_3ds_pokemon_t *pokemon);
spec_error_t spec_3ds_pokemon_get_name(const spec_3ds_pokemon_t *pokemon,
                                       char8_t name[static SPEC_3DS_TEXT_BUFFER_SIZE]);

spec_error_t spec_3ds_pokemon_calculate_stats(spec_3ds_pokemon_t *pokemon);
spec_error_t spec_3ds_pokemon_remove_nickname(spec_3ds_pokemon_t *pokemon);
spec_error_t spec_3ds_pokemon_set_level(spec_3ds_pokemon_t *pokemon, uint8_t level);
spec_error_t spec_3ds_pokemon_set_nickname(spec_3ds_pokemon_t *pokemon, const char8_t *nickname,
                                           spec_naming_t naming);

// Species functions

// nullptr for a species or game type that is no 3DS game's. A form with no data of its own has its
// species', as do the forms a game lacks.
const spec_3ds_species_data_t *spec_3ds_get_species_data(spec_game_type_t type, uint16_t species,
                                                         uint8_t form);

// Personality functions

spec_3ds_personality_t spec_3ds_decode_personality(spec_pid_t pid,
                                                   const spec_3ds_trainer_t *trainer);

// Item functions

spec_error_t spec_3ds_check_item_placement(spec_game_type_t type, spec_3ds_pocket_t pocket,
                                           uint16_t item);
spec_error_t spec_3ds_get_pocket_for_item(spec_3ds_pocket_t *pocket, spec_game_type_t type,
                                          uint16_t item);
const char *spec_3ds_item_name(uint16_t item, spec_language_t language);
size_t spec_3ds_pocket_capacity(spec_game_type_t type, spec_3ds_pocket_t pocket);
size_t spec_3ds_pocket_item_count(const spec_3ds_save_t *save, spec_3ds_pocket_t pocket);

// Text functions

spec_error_t spec_3ds_text_from_utf8(uint16_t *text, size_t text_size, const char8_t *utf8,
                                     spec_language_t language);
spec_error_t spec_3ds_text_to_utf8(char8_t utf8[static SPEC_3DS_TEXT_BUFFER_SIZE],
                                   const uint16_t *text, size_t text_size);

#endif
