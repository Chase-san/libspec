// The Gen 6 and 7 Pokédex: caught, the looks seen and the look each entry shows.

#include <string.h>

#include "3ds/3ds.h"
#include "3ds/3ds_internal.h"
#include "spec_internal.h"

constexpr size_t GEN6_CAUGHT_OFFSET = 0x008;
constexpr size_t GEN6_SEEN_OFFSET = 0x068;
constexpr size_t GEN6_DISPLAYED_OFFSET = 0x1E8;
constexpr size_t GEN6_LOOK_FLAGS_SIZE = 0x60;
// X and Y mark apart the species caught through Pokémon Bank or Poké Transporter.
constexpr size_t BANK_CAUGHT_OFFSET = 0x64C;
constexpr size_t GEN7_CAUGHT_OFFSET = 0x088;
constexpr size_t GEN7_SEEN_OFFSET = 0x0F0;
constexpr size_t GEN7_DISPLAYED_OFFSET = 0x320;
// Gen 7's look flags go on to the forms after the species.
constexpr size_t GEN7_LOOK_FLAGS_SIZE = 0x8C;
// Male, female, shiny male and shiny female, in the order of spec_3ds_pokedex_look_t.
constexpr size_t LOOK_COUNT = 4;
constexpr uint16_t GEN6_SPECIES_COUNT = 721;
constexpr uint16_t GEN7_SPECIES_COUNT = 807;

// Where one generation's Pokédex keeps its flags.
struct pokedex_offsets {
    size_t caught;
    size_t seen;
    size_t displayed;
    size_t look_flags_size;
    uint16_t species_count;
};
typedef struct pokedex_offsets pokedex_offsets_t;

constexpr pokedex_offsets_t GEN6_POKEDEX = {
    .caught = GEN6_CAUGHT_OFFSET,
    .seen = GEN6_SEEN_OFFSET,
    .displayed = GEN6_DISPLAYED_OFFSET,
    .look_flags_size = GEN6_LOOK_FLAGS_SIZE,
    .species_count = GEN6_SPECIES_COUNT,
};
constexpr pokedex_offsets_t GEN7_POKEDEX = {
    .caught = GEN7_CAUGHT_OFFSET,
    .seen = GEN7_SEEN_OFFSET,
    .displayed = GEN7_DISPLAYED_OFFSET,
    .look_flags_size = GEN7_LOOK_FLAGS_SIZE,
    .species_count = GEN7_SPECIES_COUNT,
};

static const pokedex_offsets_t *pokedex_offsets_of(const spec_3ds_layout_t *layout) {
    return layout->is_gen7 ? &GEN7_POKEDEX : &GEN6_POKEDEX;
}

static bool is_caught_at(const uint8_t *dex, const spec_3ds_layout_t *layout,
                         size_t national_number) {
    bool is_caught =
        spec_get_array_flag(&dex[pokedex_offsets_of(layout)->caught], national_number - 1);
    bool is_bank_caught = layout->has_bank_caught_flags
                          && spec_get_array_flag(&dex[BANK_CAUGHT_OFFSET], national_number - 1);
    return is_caught || is_bank_caught;
}

static uint8_t looks_at(const uint8_t *dex, size_t first_look_offset, size_t look_flags_size,
                        size_t national_number) {
    uint32_t looks = 0;
    for (unsigned look = 0; look < LOOK_COUNT; ++look) {
        const uint8_t *look_flags = &dex[first_look_offset + look * look_flags_size];
        looks = spec_set_bits(looks, look, 1, spec_get_array_flag(look_flags, national_number - 1));
    }
    return (uint8_t)looks;
}

// TODO: Decode the National Dex flag and the form and language records.
void spec_3ds_decode_pokedex(spec_3ds_pokedex_t *pokedex, const uint8_t *data,
                             const spec_3ds_layout_t *layout) {
    const uint8_t *dex = &data[layout->pokedex_offset];
    const pokedex_offsets_t *offsets = pokedex_offsets_of(layout);
    // A compound literal would build the whole Pokédex on the stack in unoptimized builds.
    memset(pokedex, 0, sizeof *pokedex);
    for (size_t national_number = 1; national_number <= offsets->species_count; ++national_number) {
        uint8_t seen_looks =
            looks_at(dex, offsets->seen, offsets->look_flags_size, national_number);
        pokedex->seen_looks[national_number] = seen_looks;
        pokedex->displayed_look[national_number] =
            looks_at(dex, offsets->displayed, offsets->look_flags_size, national_number);
        pokedex->is_caught[national_number] = is_caught_at(dex, layout, national_number);
    }
}

static void write_looks(uint8_t *dex, size_t first_look_offset, size_t look_flags_size,
                        size_t national_number, uint8_t looks) {
    for (unsigned look = 0; look < LOOK_COUNT; ++look) {
        uint8_t *look_flags = &dex[first_look_offset + look * look_flags_size];
        spec_set_array_flag(look_flags, national_number - 1, spec_get_bits(looks, look, 1) != 0);
    }
}

// A species caught through Pokémon Bank keeps that mark; one no longer caught loses both.
static void write_caught(uint8_t *dex, const spec_3ds_layout_t *layout, size_t national_number,
                         bool is_caught) {
    size_t caught_offset = pokedex_offsets_of(layout)->caught;
    if (!is_caught && layout->has_bank_caught_flags) {
        spec_set_array_flag(&dex[BANK_CAUGHT_OFFSET], national_number - 1, false);
    }
    if (is_caught_at(dex, layout, national_number) != is_caught) {
        spec_set_array_flag(&dex[caught_offset], national_number - 1, is_caught);
    }
}

void spec_3ds_encode_pokedex(uint8_t *data, const spec_3ds_layout_t *layout,
                             const spec_3ds_pokedex_t *pokedex) {
    uint8_t *dex = &data[layout->pokedex_offset];
    const pokedex_offsets_t *offsets = pokedex_offsets_of(layout);
    for (size_t national_number = 1; national_number <= offsets->species_count; ++national_number) {
        write_caught(dex, layout, national_number, pokedex->is_caught[national_number]);
        write_looks(dex, offsets->seen, offsets->look_flags_size, national_number,
                    pokedex->seen_looks[national_number]);
        write_looks(dex, offsets->displayed, offsets->look_flags_size, national_number,
                    pokedex->displayed_look[national_number]);
    }
}

static bool is_entry_empty(const spec_3ds_pokedex_t *pokedex, uint16_t national_number) {
    return !pokedex->is_caught[national_number] && pokedex->seen_looks[national_number] == 0
           && pokedex->displayed_look[national_number] == 0;
}

// Retail saves hold looks displayed but unseen and species caught but unseen, so any mix is kept.
spec_error_t spec_3ds_check_pokedex(const spec_3ds_pokedex_t *pokedex,
                                    const spec_3ds_layout_t *layout) {
    uint16_t species_count = pokedex_offsets_of(layout)->species_count;
    for (uint16_t national_number = 1; national_number < SPEC_3DS_POKEDEX_SIZE; ++national_number) {
        bool does_fit = spec_fits_in_bits(pokedex->seen_looks[national_number], LOOK_COUNT)
                        && spec_fits_in_bits(pokedex->displayed_look[national_number], LOOK_COUNT);
        if (!does_fit) {
            (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "a look is not one of the four");
            return spec_locate_error(SPEC_ERROR_LOCATION_POKEDEX, national_number, 0);
        }
        if (national_number > species_count && !is_entry_empty(pokedex, national_number)) {
            (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "this game lacks the species");
            return spec_locate_error(SPEC_ERROR_LOCATION_POKEDEX, national_number, 0);
        }
    }
    return SPEC_OK;
}
