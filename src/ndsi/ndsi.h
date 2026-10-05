// The Gen 5 (Nintendo DSi) API: saves, Pokémon, personality, items and text.

#ifndef SPEC_NDSI_H
#define SPEC_NDSI_H

#include <stddef.h>
#include <stdint.h>
#include <uchar.h>

#include "nds/nds.h"
#include "spec.h"

constexpr size_t SPEC_NDSI_SAVE_SIZE = 0x80000;
constexpr size_t SPEC_NDSI_PARTY_CAPACITY = 6;
constexpr size_t SPEC_NDSI_BOX_COUNT = 24;
constexpr size_t SPEC_NDSI_BOX_CAPACITY = 30;
constexpr size_t SPEC_NDSI_PARTY_RECORD_SIZE = 220;
constexpr size_t SPEC_NDSI_BOX_RECORD_SIZE = 136;
// Text sizes count UTF-16 units, the terminator's included.
constexpr size_t SPEC_NDSI_NICKNAME_SIZE = 11;
constexpr size_t SPEC_NDSI_TRAINER_NAME_SIZE = 8;
constexpr size_t SPEC_NDSI_BOX_NAME_SIZE = 20;
constexpr size_t SPEC_NDSI_DAYCARE_CAPACITY = 2;
constexpr size_t SPEC_NDSI_MOVE_COUNT = 4;
constexpr size_t SPEC_NDSI_BADGE_COUNT = 8;
constexpr size_t SPEC_NDSI_MAIL_ICON_COUNT = 4;
constexpr size_t SPEC_NDSI_MAIL_SENTENCE_COUNT = 3;
// Indexed by National Dex number.
constexpr size_t SPEC_NDSI_POKEDEX_SIZE = 650;
constexpr size_t SPEC_NDSI_POCKET_MAX_CAPACITY = 310;
constexpr size_t SPEC_NDSI_TEXT_MAX_SIZE = SPEC_NDSI_BOX_NAME_SIZE;
// Up to 3 UTF-8 bytes per character, plus NUL.
constexpr size_t SPEC_NDSI_TEXT_BUFFER_SIZE = SPEC_NDSI_TEXT_MAX_SIZE * 3 + 1;

typedef spec_nds_contest_category_t spec_ndsi_contest_category_t;

// In the games' pocket order; Poké Balls go in the Items pocket.
enum spec_ndsi_pocket {
    SPEC_NDSI_POCKET_ITEMS,
    SPEC_NDSI_POCKET_MEDICINE,
    SPEC_NDSI_POCKET_TMS_HMS,
    SPEC_NDSI_POCKET_BERRIES,
    SPEC_NDSI_POCKET_KEY_ITEMS,
    SPEC_NDSI_POCKET_COUNT,
};
typedef enum spec_ndsi_pocket spec_ndsi_pocket_t;

typedef spec_nds_marking_t spec_ndsi_marking_t;
typedef spec_nds_sinnoh_ribbon_t spec_ndsi_sinnoh_ribbon_t;
typedef spec_nds_hoenn_ribbon_t spec_ndsi_hoenn_ribbon_t;
typedef spec_nds_super_contest_ribbon_t spec_ndsi_super_contest_ribbon_t;

enum spec_ndsi_pokedex_look : uint8_t {
    SPEC_NDSI_POKEDEX_LOOK_MALE = 1 << 0,
    SPEC_NDSI_POKEDEX_LOOK_FEMALE = 1 << 1,
    SPEC_NDSI_POKEDEX_LOOK_SHINY_MALE = 1 << 2,
    SPEC_NDSI_POKEDEX_LOOK_SHINY_FEMALE = 1 << 3,
};
typedef enum spec_ndsi_pokedex_look spec_ndsi_pokedex_look_t;

// nature is the pid's, which Pokémon from Gen 3 and 4 keep; Gen 5 stores its own.
typedef spec_nds_personality_t spec_ndsi_personality_t;

// The same shape as Gen 4's, but the name is UTF-16.
struct spec_ndsi_trainer {
    uint16_t name[SPEC_NDSI_TRAINER_NAME_SIZE];
    uint16_t id;
    uint16_t secret_id;
    bool is_female;
};
typedef struct spec_ndsi_trainer spec_ndsi_trainer_t;

typedef spec_nds_date_t spec_ndsi_date_t;
typedef spec_nds_origin_t spec_ndsi_origin_t;
typedef spec_nds_move_t spec_ndsi_move_t;
typedef spec_nds_contest_t spec_ndsi_contest_t;
typedef spec_nds_pokerus_t spec_ndsi_pokerus_t;
typedef spec_nds_status_t spec_ndsi_status_t;
typedef spec_nds_mail_sentence_t spec_ndsi_mail_sentence_t;

// A type of 0xFF is no mail. The icon words are kept as stored; Gen 5's use of them is unknown.
struct spec_ndsi_mail {
    spec_ndsi_trainer_t author;
    spec_language_t language;
    spec_version_t version;
    uint8_t type;
    uint16_t icons[SPEC_NDSI_MAIL_ICON_COUNT];
    spec_ndsi_mail_sentence_t sentences[SPEC_NDSI_MAIL_SENTENCE_COUNT];
};
typedef struct spec_ndsi_mail spec_ndsi_mail_t;

// Zero for boxed Pokémon; filled in when written to the party.
struct spec_ndsi_party_data {
    spec_ndsi_status_t status;
    uint8_t level;
    uint16_t current_hp;
    uint16_t stats[SPEC_STAT_COUNT];
    spec_ndsi_mail_t mail;
};
typedef struct spec_ndsi_party_data spec_ndsi_party_data_t;

// Names are UTF-16; species is the National Dex number.
struct spec_ndsi_pokemon {
    uint16_t species;
    uint8_t form;
    spec_ndsi_personality_t personality;
    uint16_t nickname[SPEC_NDSI_NICKNAME_SIZE];
    bool is_nicknamed;
    spec_language_t language;
    spec_ndsi_trainer_t trainer;
    spec_ndsi_origin_t origin;

    uint32_t experience;
    uint8_t friendship;
    uint16_t held_item;
    uint8_t ability;
    bool has_hidden_ability;
    spec_nature_t nature;
    uint8_t ivs[SPEC_STAT_COUNT];
    // Derived on read, for Pokémon from Gen 3 and 4 only.
    spec_iv_method_t iv_method;
    uint8_t evs[SPEC_STAT_COUNT];
    spec_ndsi_move_t moves[SPEC_NDSI_MOVE_COUNT];

    spec_ndsi_contest_t contest;
    uint32_t sinnoh_ribbons;
    uint32_t hoenn_ribbons;
    uint32_t super_contest_ribbons;
    uint8_t markings;
    spec_ndsi_pokerus_t pokerus;
    uint8_t pokestar_fame; // Black 2 and White 2 only

    bool is_egg;
    bool is_bad_egg;
    bool is_fateful_encounter;
    bool is_n_pokemon;

    spec_ndsi_party_data_t party_data;
};
typedef struct spec_ndsi_pokemon spec_ndsi_pokemon_t;

typedef spec_nds_play_time_t spec_ndsi_play_time_t;

// The looks are spec_ndsi_pokedex_look_t flags. A seen species displays one of its seen looks;
// a caught species must be seen.
struct spec_ndsi_pokedex {
    uint32_t spinda_personality;
    bool is_caught[SPEC_NDSI_POKEDEX_SIZE];
    uint8_t seen_looks[SPEC_NDSI_POKEDEX_SIZE];
    uint8_t displayed_look[SPEC_NDSI_POKEDEX_SIZE];
};
typedef struct spec_ndsi_pokedex spec_ndsi_pokedex_t;

struct spec_ndsi_box {
    uint16_t name[SPEC_NDSI_BOX_NAME_SIZE];
    uint8_t wallpaper;
    spec_ndsi_pokemon_t pokemon[SPEC_NDSI_BOX_CAPACITY];
};
typedef struct spec_ndsi_box spec_ndsi_box_t;

struct spec_ndsi_daycare_slot {
    spec_ndsi_pokemon_t pokemon;
    uint32_t steps;
};
typedef struct spec_ndsi_daycare_slot spec_ndsi_daycare_slot_t;

struct spec_ndsi_daycare {
    spec_ndsi_daycare_slot_t slots[SPEC_NDSI_DAYCARE_CAPACITY];
    bool is_egg_waiting;
};
typedef struct spec_ndsi_daycare spec_ndsi_daycare_t;

typedef spec_item_slot_t spec_ndsi_item_slot_t;

// Writing condenses pockets; a slot with no item or zero quantity is empty.
struct spec_ndsi_save {
    spec_game_type_t type;
    spec_language_t language;
    spec_ndsi_trainer_t trainer;
    spec_ndsi_play_time_t play_time;
    uint32_t money;
    bool badges[SPEC_NDSI_BADGE_COUNT];
    uint16_t rival_name[SPEC_NDSI_TRAINER_NAME_SIZE]; // Black 2 and White 2 only
    spec_ndsi_pokedex_t pokedex;
    uint8_t party_count;
    spec_ndsi_pokemon_t party[SPEC_NDSI_PARTY_CAPACITY];
    uint8_t current_box;
    spec_ndsi_box_t boxes[SPEC_NDSI_BOX_COUNT];
    spec_ndsi_daycare_t daycare;
    spec_ndsi_item_slot_t items[SPEC_NDSI_POCKET_COUNT][SPEC_NDSI_POCKET_MAX_CAPACITY];
};
typedef struct spec_ndsi_save spec_ndsi_save_t;

spec_error_t spec_ndsi_read_save(spec_ndsi_save_t *save,
                                 const uint8_t data[static SPEC_NDSI_SAVE_SIZE]);
spec_error_t spec_ndsi_write_save(const spec_ndsi_save_t *save,
                                  uint8_t data[static SPEC_NDSI_SAVE_SIZE]);

// raw is the encrypted record as the save stores it.
spec_error_t spec_ndsi_read_pokemon(spec_ndsi_pokemon_t *pokemon, const uint8_t *raw,
                                    size_t raw_size);
spec_error_t spec_ndsi_write_pokemon(uint8_t *raw, size_t raw_size,
                                     const spec_ndsi_pokemon_t *pokemon);
spec_error_t spec_ndsi_pokemon_calculate_stats(spec_ndsi_pokemon_t *pokemon);
spec_error_t spec_ndsi_pokemon_get_name(const spec_ndsi_pokemon_t *pokemon,
                                        char8_t name[static SPEC_NDSI_TEXT_BUFFER_SIZE]);
spec_error_t spec_ndsi_pokemon_set_nickname(spec_ndsi_pokemon_t *pokemon, const char8_t *nickname,
                                            spec_naming_t naming);
spec_error_t spec_ndsi_pokemon_remove_nickname(spec_ndsi_pokemon_t *pokemon);
bool spec_ndsi_is_safe_to_box(const spec_ndsi_pokemon_t *pokemon);

spec_ndsi_personality_t spec_ndsi_decode_personality(spec_pid_t pid, uint16_t species,
                                                     const spec_ndsi_trainer_t *trainer);

const char *spec_ndsi_item_name(uint16_t item, spec_language_t language);
spec_error_t spec_ndsi_get_pocket_for_item(spec_ndsi_pocket_t *pocket, spec_game_type_t type,
                                           uint16_t item);
size_t spec_ndsi_pocket_capacity(spec_game_type_t type, spec_ndsi_pocket_t pocket);
size_t spec_ndsi_pocket_item_count(const spec_ndsi_save_t *save, spec_ndsi_pocket_t pocket);

spec_error_t spec_ndsi_text_to_utf8(char8_t utf8[static SPEC_NDSI_TEXT_BUFFER_SIZE],
                                    const uint16_t *text, size_t text_size);
spec_error_t spec_ndsi_text_from_utf8(uint16_t *text, size_t text_size, const char8_t *utf8,
                                      spec_language_t language);

#endif
