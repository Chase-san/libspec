// The Gen 5 Pokédex: caught, the looks seen, the look each entry shows, and Spinda's pattern.

#include "ndsi/ndsi.h"
#include "ndsi/ndsi_internal.h"
#include "spec_internal.h"

// The community's. TODO: Verify against the carts.
constexpr size_t CAUGHT_OFFSET = 0x008;
constexpr size_t SEEN_OFFSET = 0x05C;
constexpr size_t DISPLAYED_OFFSET = 0x1AC;
constexpr size_t FLAGS_SIZE = 0x54;
// Male, female, shiny male and shiny female, in the order of spec_ndsi_pokedex_look_t.
constexpr size_t LOOK_COUNT = 4;

static uint8_t looks_at(const uint8_t *dex, size_t first_look_offset, size_t national_number) {
    uint32_t looks = 0;
    for (unsigned look = 0; look < LOOK_COUNT; ++look) {
        const uint8_t *look_flags = &dex[first_look_offset + look * FLAGS_SIZE];
        looks = spec_set_bits(looks, look, 1, spec_get_array_flag(look_flags, national_number - 1));
    }
    return (uint8_t)looks;
}

// As Gen 4's Pokedex_HasCaughtSpecies, a species counts as caught only when it is also seen.
void spec_ndsi_decode_pokedex(spec_ndsi_pokedex_t *pokedex, const uint8_t *copy,
                              const spec_ndsi_layout_t *layout) {
    const uint8_t *dex = &copy[layout->pokedex_offset];
    // TODO: Decode the National Dex flag and the form and language records.
    for (size_t national_number = 1; national_number < SPEC_NDSI_POKEDEX_SIZE; ++national_number) {
        uint8_t seen_looks = looks_at(dex, SEEN_OFFSET, national_number);
        pokedex->seen_looks[national_number] = seen_looks;
        pokedex->displayed_look[national_number] = looks_at(dex, DISPLAYED_OFFSET, national_number);
        pokedex->is_caught[national_number] =
            seen_looks != 0 && spec_get_array_flag(&dex[CAUGHT_OFFSET], national_number - 1);
    }
    pokedex->spinda_personality = spec_read_u32_le(&dex[layout->spinda_offset]);
}

static void write_looks(uint8_t *dex, size_t first_look_offset, size_t national_number,
                        uint8_t looks) {
    for (unsigned look = 0; look < LOOK_COUNT; ++look) {
        uint8_t *look_flags = &dex[first_look_offset + look * FLAGS_SIZE];
        spec_set_array_flag(look_flags, national_number - 1, spec_get_bits(looks, look, 1) != 0);
    }
}

void spec_ndsi_encode_pokedex(uint8_t *copy, const spec_ndsi_layout_t *layout,
                              const spec_ndsi_pokedex_t *pokedex) {
    uint8_t *dex = &copy[layout->pokedex_offset];
    for (size_t national_number = 1; national_number < SPEC_NDSI_POKEDEX_SIZE; ++national_number) {
        spec_set_array_flag(&dex[CAUGHT_OFFSET], national_number - 1,
                            pokedex->is_caught[national_number]);
        write_looks(dex, SEEN_OFFSET, national_number, pokedex->seen_looks[national_number]);
        write_looks(dex, DISPLAYED_OFFSET, national_number,
                    pokedex->displayed_look[national_number]);
    }
    spec_write_u32_le(&dex[layout->spinda_offset], pokedex->spinda_personality);
}

// The games display exactly one look; tools may leave more, which the games load.
static spec_error_t check_entry(const spec_ndsi_pokedex_t *pokedex, uint16_t national_number) {
    uint8_t seen_looks = pokedex->seen_looks[national_number];
    uint8_t displayed_look = pokedex->displayed_look[national_number];
    bool is_seen = seen_looks != 0;
    if (!spec_fits_in_bits(seen_looks, LOOK_COUNT)) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "seen_looks holds an unknown look");
    }
    if ((displayed_look & ~seen_looks) != 0) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "displayed_look is not a seen look");
    }
    if (is_seen && displayed_look == 0) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "a seen species displays no look");
    }
    if (pokedex->is_caught[national_number] && !is_seen) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "a caught species must be seen");
    }
    return SPEC_OK;
}

spec_error_t spec_ndsi_check_pokedex(const spec_ndsi_pokedex_t *pokedex) {
    for (uint16_t national_number = 1; national_number < SPEC_NDSI_POKEDEX_SIZE;
         ++national_number) {
        if (check_entry(pokedex, national_number) != SPEC_OK) {
            return spec_locate_error(SPEC_ERROR_LOCATION_POKEDEX, national_number, 0);
        }
    }
    return SPEC_OK;
}
