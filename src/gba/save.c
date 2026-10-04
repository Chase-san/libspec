#include <string.h>

#include "gba/gba.h"
#include "gba/gba_internal.h"
#include "spec_internal.h"

constexpr size_t ITEM_SLOT_SIZE = 4;
constexpr size_t ITEM_QUANTITY_OFFSET = 2;
constexpr size_t POKEDEX_FLAGS_SIZE = 0x34;
constexpr size_t POKEDEX_SEEN_COPY_COUNT = 3;
constexpr uint8_t POKEDEX_MODE_NATIONAL = 1;
constexpr uint8_t POKEDEX_ORDER_FIRST = 0;

constexpr size_t SAVE_BLOCK_2 = 0 * SPEC_GBA_SECTION_DATA_SIZE;
constexpr size_t SAVE_BLOCK_1 = 1 * SPEC_GBA_SECTION_DATA_SIZE;
constexpr size_t STORAGE = 5 * SPEC_GBA_SECTION_DATA_SIZE;

struct national_dex_layout {
    size_t magic_offset;
    uint8_t magic;
    size_t var_offset;
    uint16_t var_value;
    uint16_t flag;
    bool does_enabling_pick_national_mode;
};
typedef struct national_dex_layout national_dex_layout_t;

struct layout {
    spec_game_type_t type;
    uint16_t section_sizes[SPEC_GBA_SECTION_COUNT];
    size_t trainer_name_offset;
    size_t trainer_gender_offset;
    size_t trainer_id_offset;
    size_t secret_id_offset;
    size_t play_time_offset;
    bool has_security_key;
    size_t security_key_offset;
    size_t money_offset;
    size_t coins_offset;
    size_t flags_offset;
    uint16_t first_badge_flag;
    uint16_t pokedex_flag;
    size_t pokedex_order_offset;
    size_t pokedex_mode_offset;
    size_t unown_personality_offset;
    size_t spinda_personality_offset;
    size_t pokedex_caught_offset;
    size_t pokedex_seen_offsets[POKEDEX_SEEN_COPY_COUNT];
    national_dex_layout_t national_dex;
    size_t party_count_offset;
    size_t party_offset;
    size_t box_records_offset;
    size_t pocket_offsets[SPEC_GBA_POCKET_COUNT];
};
typedef struct layout layout_t;

struct stored_data {
    uint8_t party[SPEC_GBA_PARTY_CAPACITY][SPEC_GBA_PARTY_RECORD_SIZE];
    uint8_t boxes[SPEC_GBA_BOX_COUNT][SPEC_GBA_BOX_CAPACITY][SPEC_GBA_BOX_RECORD_SIZE];
    uint8_t pockets[SPEC_GBA_POCKET_COUNT][SPEC_GBA_POCKET_MAX_CAPACITY * ITEM_SLOT_SIZE];
};
typedef struct stored_data stored_data_t;

constexpr layout_t RUBY_SAPPHIRE_LAYOUT = {
    .type = SPEC_GAME_TYPE_RUBY_SAPPHIRE,
    .section_sizes = {0x890, 0xF80, 0xF80, 0xF80, 0xC40, 0xF80, 0xF80, 0xF80, 0xF80, 0xF80, 0xF80,
                      0xF80, 0xF80, 0x7D0},
    .trainer_name_offset = SAVE_BLOCK_2 + 0x0,
    .trainer_gender_offset = SAVE_BLOCK_2 + 0x8,
    .trainer_id_offset = SAVE_BLOCK_2 + 0xA,
    .secret_id_offset = SAVE_BLOCK_2 + 0xC,
    .play_time_offset = SAVE_BLOCK_2 + 0xE,
    .has_security_key = false,
    .money_offset = SAVE_BLOCK_1 + 0x490,
    .coins_offset = SAVE_BLOCK_1 + 0x494,
    .flags_offset = SAVE_BLOCK_1 + 0x1220,
    .first_badge_flag = 0x807,
    .pokedex_flag = 0x801,
    .pokedex_order_offset = SAVE_BLOCK_2 + 0x18,
    .pokedex_mode_offset = SAVE_BLOCK_2 + 0x19,
    .unown_personality_offset = SAVE_BLOCK_2 + 0x1C,
    .spinda_personality_offset = SAVE_BLOCK_2 + 0x20,
    .pokedex_caught_offset = SAVE_BLOCK_2 + 0x28,
    .pokedex_seen_offsets = {SAVE_BLOCK_2 + 0x5C, SAVE_BLOCK_1 + 0x938, SAVE_BLOCK_1 + 0x3A8C},
    .national_dex = {SAVE_BLOCK_2 + 0x1A, 0xDA, SAVE_BLOCK_1 + 0x13CC, 0x302, 0x836, true},
    .party_count_offset = SAVE_BLOCK_1 + 0x234,
    .party_offset = SAVE_BLOCK_1 + 0x238,
    .box_records_offset = STORAGE + 0x4,
    .pocket_offsets =
        {
            [SPEC_GBA_POCKET_ITEMS] = SAVE_BLOCK_1 + 0x560,
            [SPEC_GBA_POCKET_KEY_ITEMS] = SAVE_BLOCK_1 + 0x5B0,
            [SPEC_GBA_POCKET_POKE_BALLS] = SAVE_BLOCK_1 + 0x600,
            [SPEC_GBA_POCKET_TMS_HMS] = SAVE_BLOCK_1 + 0x640,
            [SPEC_GBA_POCKET_BERRIES] = SAVE_BLOCK_1 + 0x740,
            [SPEC_GBA_POCKET_PC] = SAVE_BLOCK_1 + 0x498,
        },
};

constexpr layout_t EMERALD_LAYOUT = {
    .type = SPEC_GAME_TYPE_EMERALD,
    .section_sizes = {0xF2C, 0xF80, 0xF80, 0xF80, 0xF08, 0xF80, 0xF80, 0xF80, 0xF80, 0xF80, 0xF80,
                      0xF80, 0xF80, 0x7D0},
    .trainer_name_offset = SAVE_BLOCK_2 + 0x0,
    .trainer_gender_offset = SAVE_BLOCK_2 + 0x8,
    .trainer_id_offset = SAVE_BLOCK_2 + 0xA,
    .secret_id_offset = SAVE_BLOCK_2 + 0xC,
    .play_time_offset = SAVE_BLOCK_2 + 0xE,
    .has_security_key = true,
    .security_key_offset = SAVE_BLOCK_2 + 0xAC,
    .money_offset = SAVE_BLOCK_1 + 0x490,
    .coins_offset = SAVE_BLOCK_1 + 0x494,
    .flags_offset = SAVE_BLOCK_1 + 0x1270,
    .first_badge_flag = 0x867,
    .pokedex_flag = 0x861,
    .pokedex_order_offset = SAVE_BLOCK_2 + 0x18,
    .pokedex_mode_offset = SAVE_BLOCK_2 + 0x19,
    .unown_personality_offset = SAVE_BLOCK_2 + 0x1C,
    .spinda_personality_offset = SAVE_BLOCK_2 + 0x20,
    .pokedex_caught_offset = SAVE_BLOCK_2 + 0x28,
    .pokedex_seen_offsets = {SAVE_BLOCK_2 + 0x5C, SAVE_BLOCK_1 + 0x988, SAVE_BLOCK_1 + 0x3B24},
    .national_dex = {SAVE_BLOCK_2 + 0x1A, 0xDA, SAVE_BLOCK_1 + 0x1428, 0x302, 0x896, true},
    .party_count_offset = SAVE_BLOCK_1 + 0x234,
    .party_offset = SAVE_BLOCK_1 + 0x238,
    .box_records_offset = STORAGE + 0x4,
    .pocket_offsets =
        {
            [SPEC_GBA_POCKET_ITEMS] = SAVE_BLOCK_1 + 0x560,
            [SPEC_GBA_POCKET_KEY_ITEMS] = SAVE_BLOCK_1 + 0x5D8,
            [SPEC_GBA_POCKET_POKE_BALLS] = SAVE_BLOCK_1 + 0x650,
            [SPEC_GBA_POCKET_TMS_HMS] = SAVE_BLOCK_1 + 0x690,
            [SPEC_GBA_POCKET_BERRIES] = SAVE_BLOCK_1 + 0x790,
            [SPEC_GBA_POCKET_PC] = SAVE_BLOCK_1 + 0x498,
        },
};

constexpr layout_t FIRERED_LEAFGREEN_LAYOUT = {
    .type = SPEC_GAME_TYPE_FIRERED_LEAFGREEN,
    .section_sizes = {0xF24, 0xF80, 0xF80, 0xF80, 0xEE8, 0xF80, 0xF80, 0xF80, 0xF80, 0xF80, 0xF80,
                      0xF80, 0xF80, 0x7D0},
    .trainer_name_offset = SAVE_BLOCK_2 + 0x0,
    .trainer_gender_offset = SAVE_BLOCK_2 + 0x8,
    .trainer_id_offset = SAVE_BLOCK_2 + 0xA,
    .secret_id_offset = SAVE_BLOCK_2 + 0xC,
    .play_time_offset = SAVE_BLOCK_2 + 0xE,
    .has_security_key = true,
    .security_key_offset = SAVE_BLOCK_2 + 0xF20,
    .money_offset = SAVE_BLOCK_1 + 0x290,
    .coins_offset = SAVE_BLOCK_1 + 0x294,
    .flags_offset = SAVE_BLOCK_1 + 0xEE0,
    .first_badge_flag = 0x820,
    .pokedex_flag = 0x829,
    .pokedex_order_offset = SAVE_BLOCK_2 + 0x18,
    .pokedex_mode_offset = SAVE_BLOCK_2 + 0x19,
    .unown_personality_offset = SAVE_BLOCK_2 + 0x1C,
    .spinda_personality_offset = SAVE_BLOCK_2 + 0x20,
    .pokedex_caught_offset = SAVE_BLOCK_2 + 0x28,
    .pokedex_seen_offsets = {SAVE_BLOCK_2 + 0x5C, SAVE_BLOCK_1 + 0x5F8, SAVE_BLOCK_1 + 0x3A18},
    .national_dex = {SAVE_BLOCK_2 + 0x1B, 0xB9, SAVE_BLOCK_1 + 0x109C, 0x6258, 0x840, false},
    .party_count_offset = SAVE_BLOCK_1 + 0x34,
    .party_offset = SAVE_BLOCK_1 + 0x38,
    .box_records_offset = STORAGE + 0x4,
    .pocket_offsets =
        {
            [SPEC_GBA_POCKET_ITEMS] = SAVE_BLOCK_1 + 0x310,
            [SPEC_GBA_POCKET_KEY_ITEMS] = SAVE_BLOCK_1 + 0x3B8,
            [SPEC_GBA_POCKET_POKE_BALLS] = SAVE_BLOCK_1 + 0x430,
            [SPEC_GBA_POCKET_TMS_HMS] = SAVE_BLOCK_1 + 0x464,
            [SPEC_GBA_POCKET_BERRIES] = SAVE_BLOCK_1 + 0x54C,
            [SPEC_GBA_POCKET_PC] = SAVE_BLOCK_1 + 0x298,
        },
};

// Smallest first: a save also validates under a larger game's section sizes.
constexpr spec_game_type_t TYPES_BY_SECTION_SIZE[] = {
    SPEC_GAME_TYPE_RUBY_SAPPHIRE,
    SPEC_GAME_TYPE_FIRERED_LEAFGREEN,
    SPEC_GAME_TYPE_EMERALD,
};

static inline const layout_t *get_layout(spec_game_type_t type) {
    switch (type) {
        case SPEC_GAME_TYPE_RUBY_SAPPHIRE:
            return &RUBY_SAPPHIRE_LAYOUT;
        case SPEC_GAME_TYPE_EMERALD:
            return &EMERALD_LAYOUT;
        case SPEC_GAME_TYPE_FIRERED_LEAFGREEN:
            return &FIRERED_LEAFGREEN_LAYOUT;
    }
    return nullptr;
}

static uint8_t read_u8(const uint8_t *data, const spec_gba_slot_t *slot, size_t offset) {
    uint8_t value = 0;
    spec_gba_read_slot_bytes(&value, data, slot, offset, 1);
    return value;
}

static uint16_t read_u16(const uint8_t *data, const spec_gba_slot_t *slot, size_t offset) {
    uint8_t bytes[2];
    spec_gba_read_slot_bytes(bytes, data, slot, offset, sizeof bytes);
    return spec_read_u16_le(bytes);
}

static uint32_t read_u32(const uint8_t *data, const spec_gba_slot_t *slot, size_t offset) {
    uint8_t bytes[4];
    spec_gba_read_slot_bytes(bytes, data, slot, offset, sizeof bytes);
    return spec_read_u32_le(bytes);
}

static void write_u8(uint8_t *data, const spec_gba_slot_t *slot, size_t offset, uint8_t value) {
    spec_gba_write_slot_bytes(data, slot, offset, &value, 1);
}

static void write_u16(uint8_t *data, const spec_gba_slot_t *slot, size_t offset, uint16_t value) {
    uint8_t bytes[2];
    spec_write_u16_le(bytes, value);
    spec_gba_write_slot_bytes(data, slot, offset, bytes, sizeof bytes);
}

static void write_u32(uint8_t *data, const spec_gba_slot_t *slot, size_t offset, uint32_t value) {
    uint8_t bytes[4];
    spec_write_u32_le(bytes, value);
    spec_gba_write_slot_bytes(data, slot, offset, bytes, sizeof bytes);
}

static bool read_flag(const uint8_t *data, const spec_gba_slot_t *slot, const layout_t *layout,
                      uint16_t flag) {
    uint8_t flag_byte = read_u8(data, slot, layout->flags_offset + flag / 8);
    return spec_get_bits(flag_byte, flag % 8, 1) != 0;
}

static void write_flag(uint8_t *data, const spec_gba_slot_t *slot, const layout_t *layout,
                       uint16_t flag, bool is_set) {
    size_t flag_byte_offset = layout->flags_offset + flag / 8;
    uint8_t flag_byte = read_u8(data, slot, flag_byte_offset);
    write_u8(data, slot, flag_byte_offset, (uint8_t)spec_set_bits(flag_byte, flag % 8, 1, is_set));
}

static uint32_t security_key_of(const uint8_t *data, const spec_gba_slot_t *slot,
                                const layout_t *layout) {
    if (!layout->has_security_key) {
        return 0;
    }
    return read_u32(data, slot, layout->security_key_offset);
}

static uint16_t quantity_key_of(uint32_t security_key, spec_gba_pocket_t pocket) {
    return pocket == SPEC_GBA_POCKET_PC ? 0 : (uint16_t)security_key;
}

static void gather_stored_data(stored_data_t *stored, const uint8_t *data,
                               const spec_gba_slot_t *slot, const layout_t *layout) {
    spec_gba_read_slot_bytes(&stored->party[0][0], data, slot, layout->party_offset,
                             sizeof stored->party);
    spec_gba_read_slot_bytes(&stored->boxes[0][0][0], data, slot, layout->box_records_offset,
                             sizeof stored->boxes);
    for (size_t pocket = 0; pocket < SPEC_GBA_POCKET_COUNT; ++pocket) {
        spec_gba_read_slot_bytes(
            stored->pockets[pocket], data, slot, layout->pocket_offsets[pocket],
            spec_gba_pocket_capacity(layout->type, (spec_gba_pocket_t)pocket) * ITEM_SLOT_SIZE);
    }
}

static void scatter_stored_data(uint8_t *data, const spec_gba_slot_t *slot, const layout_t *layout,
                                const stored_data_t *stored) {
    spec_gba_write_slot_bytes(data, slot, layout->party_offset, &stored->party[0][0],
                              sizeof stored->party);
    spec_gba_write_slot_bytes(data, slot, layout->box_records_offset, &stored->boxes[0][0][0],
                              sizeof stored->boxes);
    for (size_t pocket = 0; pocket < SPEC_GBA_POCKET_COUNT; ++pocket) {
        spec_gba_write_slot_bytes(
            data, slot, layout->pocket_offsets[pocket], stored->pockets[pocket],
            spec_gba_pocket_capacity(layout->type, (spec_gba_pocket_t)pocket) * ITEM_SLOT_SIZE);
    }
}

static void decode_trainer(spec_gba_trainer_t *trainer, const uint8_t *data,
                           const spec_gba_slot_t *slot, const layout_t *layout) {
    spec_gba_read_slot_bytes(trainer->name, data, slot, layout->trainer_name_offset,
                             SPEC_GBA_TRAINER_NAME_SIZE);
    trainer->is_female = read_u8(data, slot, layout->trainer_gender_offset) != 0;
    trainer->id = read_u16(data, slot, layout->trainer_id_offset);
    trainer->secret_id = read_u16(data, slot, layout->secret_id_offset);
}

// The name's eighth byte, its terminator, is left as the game wrote it.
static void encode_trainer(uint8_t *data, const spec_gba_slot_t *slot, const layout_t *layout,
                           const spec_gba_trainer_t *trainer) {
    spec_gba_write_slot_bytes(data, slot, layout->trainer_name_offset, trainer->name,
                              SPEC_GBA_TRAINER_NAME_SIZE);
    write_u8(data, slot, layout->trainer_gender_offset, trainer->is_female ? 1 : 0);
    write_u16(data, slot, layout->trainer_id_offset, trainer->id);
    write_u16(data, slot, layout->secret_id_offset, trainer->secret_id);
}

static void decode_play_time(spec_gba_play_time_t *play_time, const uint8_t *data,
                             const spec_gba_slot_t *slot, const layout_t *layout) {
    play_time->hours = read_u16(data, slot, layout->play_time_offset);
    play_time->minutes = read_u8(data, slot, layout->play_time_offset + 2);
    play_time->seconds = read_u8(data, slot, layout->play_time_offset + 3);
    play_time->frames = read_u8(data, slot, layout->play_time_offset + 4);
}

static void encode_play_time(uint8_t *data, const spec_gba_slot_t *slot, const layout_t *layout,
                             const spec_gba_play_time_t *play_time) {
    write_u16(data, slot, layout->play_time_offset, play_time->hours);
    write_u8(data, slot, layout->play_time_offset + 2, play_time->minutes);
    write_u8(data, slot, layout->play_time_offset + 3, play_time->seconds);
    write_u8(data, slot, layout->play_time_offset + 4, play_time->frames);
}

static void decode_wallet(spec_gba_save_t *save, const uint8_t *data, const spec_gba_slot_t *slot,
                          const layout_t *layout) {
    uint32_t security_key = security_key_of(data, slot, layout);
    save->money = read_u32(data, slot, layout->money_offset) ^ security_key;
    save->coins = (uint16_t)(read_u16(data, slot, layout->coins_offset) ^ security_key);
}

static void encode_wallet(uint8_t *data, const spec_gba_slot_t *slot, const layout_t *layout,
                          const spec_gba_save_t *save) {
    uint32_t security_key = security_key_of(data, slot, layout);
    write_u32(data, slot, layout->money_offset, save->money ^ security_key);
    write_u16(data, slot, layout->coins_offset, (uint16_t)(save->coins ^ security_key));
}

static void decode_badges(bool badges[static SPEC_GBA_BADGE_COUNT], const uint8_t *data,
                          const spec_gba_slot_t *slot, const layout_t *layout) {
    for (size_t badge = 0; badge < SPEC_GBA_BADGE_COUNT; ++badge) {
        badges[badge] = read_flag(data, slot, layout, (uint16_t)(layout->first_badge_flag + badge));
    }
}

static void encode_badges(uint8_t *data, const spec_gba_slot_t *slot, const layout_t *layout,
                          const bool badges[static SPEC_GBA_BADGE_COUNT]) {
    for (size_t badge = 0; badge < SPEC_GBA_BADGE_COUNT; ++badge) {
        write_flag(data, slot, layout, (uint16_t)(layout->first_badge_flag + badge), badges[badge]);
    }
}

static bool is_national_dex_enabled(const uint8_t *data, const spec_gba_slot_t *slot,
                                    const layout_t *layout) {
    const national_dex_layout_t *national_dex = &layout->national_dex;
    return read_u8(data, slot, national_dex->magic_offset) == national_dex->magic
           && read_u16(data, slot, national_dex->var_offset) == national_dex->var_value
           && read_flag(data, slot, layout, national_dex->flag);
}

// As EnableNationalPokedex and DisableNationalPokedex.
static void set_national_dex(uint8_t *data, const spec_gba_slot_t *slot, const layout_t *layout,
                             bool is_enabled) {
    const national_dex_layout_t *national_dex = &layout->national_dex;
    write_u8(data, slot, national_dex->magic_offset, is_enabled ? national_dex->magic : 0);
    write_u16(data, slot, national_dex->var_offset, is_enabled ? national_dex->var_value : 0);
    write_flag(data, slot, layout, national_dex->flag, is_enabled);
    if (is_enabled && national_dex->does_enabling_pick_national_mode) {
        write_u8(data, slot, layout->pokedex_mode_offset, POKEDEX_MODE_NATIONAL);
        write_u8(data, slot, layout->pokedex_order_offset, POKEDEX_ORDER_FIRST);
    }
}

static bool is_pokedex_bit_set(const uint8_t *flags, size_t national_number) {
    return spec_get_bits(flags[(national_number - 1) / 8], (national_number - 1) % 8, 1) != 0;
}

static void set_pokedex_bit(uint8_t *flags, size_t national_number) {
    flags[(national_number - 1) / 8] |= (uint8_t)(1 << ((national_number - 1) % 8));
}

// As GetSetPokedexFlag: seen needs all three copies.
static void decode_pokedex(spec_gba_pokedex_t *pokedex, const uint8_t *data,
                           const spec_gba_slot_t *slot, const layout_t *layout) {
    uint8_t caught[POKEDEX_FLAGS_SIZE];
    uint8_t seen[POKEDEX_SEEN_COPY_COUNT][POKEDEX_FLAGS_SIZE];
    spec_gba_read_slot_bytes(caught, data, slot, layout->pokedex_caught_offset, sizeof caught);
    for (size_t copy = 0; copy < POKEDEX_SEEN_COPY_COUNT; ++copy) {
        spec_gba_read_slot_bytes(seen[copy], data, slot, layout->pokedex_seen_offsets[copy],
                                 sizeof seen[copy]);
    }
    for (size_t national_number = 1; national_number < SPEC_GBA_POKEDEX_SIZE; ++national_number) {
        bool is_seen = is_pokedex_bit_set(seen[0], national_number)
                       && is_pokedex_bit_set(seen[1], national_number)
                       && is_pokedex_bit_set(seen[2], national_number);
        pokedex->is_seen[national_number] = is_seen;
        pokedex->is_caught[national_number] =
            is_seen && is_pokedex_bit_set(caught, national_number);
    }
    pokedex->is_obtained = read_flag(data, slot, layout, layout->pokedex_flag);
    pokedex->has_national_dex = is_national_dex_enabled(data, slot, layout);
    pokedex->unown_personality = read_u32(data, slot, layout->unown_personality_offset);
    pokedex->spinda_personality = read_u32(data, slot, layout->spinda_personality_offset);
}

// Enabling resets the Pokédex mode, so only a change is written.
static void encode_pokedex(uint8_t *data, const spec_gba_slot_t *slot, const layout_t *layout,
                           const spec_gba_pokedex_t *pokedex) {
    uint8_t caught[POKEDEX_FLAGS_SIZE] = {};
    uint8_t seen[POKEDEX_FLAGS_SIZE] = {};
    for (size_t national_number = 1; national_number < SPEC_GBA_POKEDEX_SIZE; ++national_number) {
        if (pokedex->is_caught[national_number]) {
            set_pokedex_bit(caught, national_number);
        }
        if (pokedex->is_seen[national_number] || pokedex->is_caught[national_number]) {
            set_pokedex_bit(seen, national_number);
        }
    }
    spec_gba_write_slot_bytes(data, slot, layout->pokedex_caught_offset, caught, sizeof caught);
    for (size_t copy = 0; copy < POKEDEX_SEEN_COPY_COUNT; ++copy) {
        spec_gba_write_slot_bytes(data, slot, layout->pokedex_seen_offsets[copy], seen,
                                  sizeof seen);
    }
    write_flag(data, slot, layout, layout->pokedex_flag, pokedex->is_obtained);
    if (pokedex->has_national_dex != is_national_dex_enabled(data, slot, layout)) {
        set_national_dex(data, slot, layout, pokedex->has_national_dex);
    }
    write_u32(data, slot, layout->unown_personality_offset, pokedex->unown_personality);
    write_u32(data, slot, layout->spinda_personality_offset, pokedex->spinda_personality);
}

static void decode_all_pokemon(spec_gba_save_t *save, const stored_data_t *stored) {
    for (size_t index = 0; index < SPEC_GBA_PARTY_CAPACITY; ++index) {
        spec_gba_decode_pokemon(&save->party[index], stored->party[index],
                                SPEC_GBA_PARTY_RECORD_SIZE);
        spec_gba_fill_party_data(&save->party[index]);
    }
    for (size_t box = 0; box < SPEC_GBA_BOX_COUNT; ++box) {
        for (size_t slot = 0; slot < SPEC_GBA_BOX_CAPACITY; ++slot) {
            spec_gba_decode_pokemon(&save->boxes[box][slot], stored->boxes[box][slot],
                                    SPEC_GBA_BOX_RECORD_SIZE);
        }
    }
}

// Encoded before the save changes, so a failure changes nothing.
static spec_error_t encode_all_pokemon(stored_data_t *stored, const spec_gba_save_t *save) {
    for (size_t index = 0; index < SPEC_GBA_PARTY_CAPACITY; ++index) {
        spec_gba_pokemon_t pokemon = save->party[index];
        spec_gba_fill_party_data(&pokemon);
        if (spec_gba_encode_pokemon(stored->party[index], SPEC_GBA_PARTY_RECORD_SIZE, &pokemon)
            != SPEC_OK) {
            return spec_locate_error(SPEC_ERROR_LOCATION_PARTY, (uint32_t)index, 0);
        }
    }
    for (size_t box = 0; box < SPEC_GBA_BOX_COUNT; ++box) {
        for (size_t slot = 0; slot < SPEC_GBA_BOX_CAPACITY; ++slot) {
            if (spec_gba_encode_pokemon(stored->boxes[box][slot], SPEC_GBA_BOX_RECORD_SIZE,
                                        &save->boxes[box][slot])
                != SPEC_OK) {
                return spec_locate_error(SPEC_ERROR_LOCATION_BOX, (uint32_t)box, (uint32_t)slot);
            }
        }
    }
    return SPEC_OK;
}

static void decode_all_items(spec_gba_save_t *save, const stored_data_t *stored,
                             const layout_t *layout, uint32_t security_key) {
    for (size_t pocket = 0; pocket < SPEC_GBA_POCKET_COUNT; ++pocket) {
        uint16_t quantity_key = quantity_key_of(security_key, (spec_gba_pocket_t)pocket);
        size_t capacity = spec_gba_pocket_capacity(layout->type, (spec_gba_pocket_t)pocket);
        for (size_t index = 0; index < capacity; ++index) {
            const uint8_t *stored_slot = &stored->pockets[pocket][index * ITEM_SLOT_SIZE];
            spec_gba_item_slot_t *item_slot = &save->items[pocket][index];
            item_slot->item = spec_read_u16_le(stored_slot);
            item_slot->quantity =
                (uint16_t)(spec_read_u16_le(&stored_slot[ITEM_QUANTITY_OFFSET]) ^ quantity_key);
        }
    }
}

static void write_stored_item_slot(uint8_t *stored_slot, const spec_gba_item_slot_t *item_slot,
                                   uint16_t quantity_key) {
    spec_write_u16_le(stored_slot, item_slot->item);
    spec_write_u16_le(&stored_slot[ITEM_QUANTITY_OFFSET],
                      (uint16_t)(item_slot->quantity ^ quantity_key));
}

static spec_error_t check_item_fits(const layout_t *layout, spec_gba_pocket_t pocket,
                                    size_t filled_slot_count, uint16_t item) {
    if (filled_slot_count == spec_gba_pocket_capacity(layout->type, pocket)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                         "the pocket holds more items than this game allows");
    }
    return spec_gba_check_item_placement(layout->type, pocket, item);
}

// As the game condenses a pocket.
static spec_error_t encode_pocket(uint8_t *stored_pocket, const spec_gba_item_slot_t *item_slots,
                                  spec_gba_pocket_t pocket, const layout_t *layout,
                                  uint16_t quantity_key) {
    size_t filled_slot_count = 0;
    for (size_t index = 0; index < SPEC_GBA_POCKET_MAX_CAPACITY; ++index) {
        if (spec_gba_is_item_slot_empty(&item_slots[index])) {
            continue;
        }
        if (check_item_fits(layout, pocket, filled_slot_count, item_slots[index].item) != SPEC_OK) {
            return spec_locate_error(SPEC_ERROR_LOCATION_ITEMS, pocket, (uint32_t)index);
        }
        write_stored_item_slot(&stored_pocket[filled_slot_count * ITEM_SLOT_SIZE],
                               &item_slots[index], quantity_key);
        ++filled_slot_count;
    }
    constexpr spec_gba_item_slot_t EMPTY_SLOT = {};
    size_t capacity = spec_gba_pocket_capacity(layout->type, pocket);
    for (size_t index = filled_slot_count; index < capacity; ++index) {
        write_stored_item_slot(&stored_pocket[index * ITEM_SLOT_SIZE], &EMPTY_SLOT, quantity_key);
    }
    return SPEC_OK;
}

static spec_error_t encode_all_items(stored_data_t *stored, const spec_gba_save_t *save,
                                     const layout_t *layout, uint32_t security_key) {
    for (size_t pocket = 0; pocket < SPEC_GBA_POCKET_COUNT; ++pocket) {
        uint16_t quantity_key = quantity_key_of(security_key, (spec_gba_pocket_t)pocket);
        spec_error_t error = encode_pocket(stored->pockets[pocket], save->items[pocket],
                                           (spec_gba_pocket_t)pocket, layout, quantity_key);
        if (error != SPEC_OK) {
            return error;
        }
    }
    return SPEC_OK;
}

static bool is_gen3_language(spec_language_t language) {
    return (language >= SPEC_LANGUAGE_JAPANESE && language <= SPEC_LANGUAGE_GERMAN)
           || language == SPEC_LANGUAGE_SPANISH;
}

static void count_language_vote(size_t votes[], const spec_gba_pokemon_t *pokemon,
                                const spec_gba_trainer_t *player) {
    bool is_players_own =
        pokemon->trainer.id == player->id && pokemon->trainer.secret_id == player->secret_id;
    bool is_hatched_species = spec_gba_species_to_national(pokemon->species) != 0
                              && !pokemon->is_egg && !pokemon->is_bad_egg;
    if (is_players_own && is_hatched_species && is_gen3_language(pokemon->language)) {
        votes[pokemon->language]++;
    }
}

// The save has no language, so the player's own Pokémon vote.
static spec_language_t detect_language(const spec_gba_save_t *save) {
    size_t votes[SPEC_LANGUAGE_SPANISH + 1] = {};
    for (size_t index = 0; index < save->party_count && index < SPEC_GBA_PARTY_CAPACITY; ++index) {
        count_language_vote(votes, &save->party[index], &save->trainer);
    }
    for (size_t box = 0; box < SPEC_GBA_BOX_COUNT; ++box) {
        for (size_t slot = 0; slot < SPEC_GBA_BOX_CAPACITY; ++slot) {
            count_language_vote(votes, &save->boxes[box][slot], &save->trainer);
        }
    }
    spec_language_t leader = SPEC_LANGUAGE_UNKNOWN;
    bool is_tied = false;
    for (size_t language = 1; language <= SPEC_LANGUAGE_SPANISH; ++language) {
        if (votes[language] > votes[leader]) {
            leader = (spec_language_t)language;
            is_tied = false;
        } else if (votes[language] != 0 && votes[language] == votes[leader]) {
            is_tied = true;
        }
    }
    return is_tied ? SPEC_LANGUAGE_UNKNOWN : leader;
}

spec_error_t spec_gba_read_save(spec_gba_save_t *save,
                                const uint8_t data[static SPEC_GBA_SAVE_SIZE]) {
    size_t type_count = sizeof TYPES_BY_SECTION_SIZE / sizeof TYPES_BY_SECTION_SIZE[0];
    for (size_t index = 0; index < type_count; ++index) {
        const layout_t *layout = get_layout(TYPES_BY_SECTION_SIZE[index]);
        spec_gba_slot_t active;
        if (!spec_gba_find_active_slot(&active, data, layout->section_sizes)) {
            continue;
        }
        stored_data_t stored;
        gather_stored_data(&stored, data, &active, layout);
        *save = (spec_gba_save_t){.type = layout->type};
        decode_trainer(&save->trainer, data, &active, layout);
        decode_play_time(&save->play_time, data, &active, layout);
        decode_wallet(save, data, &active, layout);
        decode_badges(save->badges, data, &active, layout);
        decode_pokedex(&save->pokedex, data, &active, layout);
        save->party_count = read_u8(data, &active, layout->party_count_offset);
        decode_all_pokemon(save, &stored);
        decode_all_items(save, &stored, layout, security_key_of(data, &active, layout));
        save->language = detect_language(save);
        return SPEC_OK;
    }
    return spec_fail(SPEC_ERROR_INVALID_SAVE, "no save slot is valid for any Gen 3 game");
}

spec_error_t spec_gba_write_save(const spec_gba_save_t *save,
                                 uint8_t data[static SPEC_GBA_SAVE_SIZE]) {
    const layout_t *layout = get_layout(save->type);
    if (layout == nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "type is not a GBA game type");
    }
    spec_gba_slot_t active;
    if (!spec_gba_find_active_slot(&active, data, layout->section_sizes)) {
        return spec_fail(SPEC_ERROR_INVALID_SAVE, "no save slot is valid for the save's type");
    }
    stored_data_t stored;
    spec_error_t error = encode_all_pokemon(&stored, save);
    if (error != SPEC_OK) {
        return error;
    }
    error = encode_all_items(&stored, save, layout, security_key_of(data, &active, layout));
    if (error != SPEC_OK) {
        return error;
    }
    spec_gba_slot_t next = spec_gba_copy_to_next_slot(data, &active);
    encode_trainer(data, &next, layout, &save->trainer);
    encode_play_time(data, &next, layout, &save->play_time);
    encode_wallet(data, &next, layout, save);
    encode_badges(data, &next, layout, save->badges);
    encode_pokedex(data, &next, layout, &save->pokedex);
    write_u8(data, &next, layout->party_count_offset, save->party_count);
    scatter_stored_data(data, &next, layout, &stored);
    spec_gba_stamp_slot(data, &next, layout->section_sizes);
    return SPEC_OK;
}
