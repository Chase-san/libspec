// The Gen 4 Pokédex: seen and caught, genders, languages, form orders and its upgrades.

#include "nds/nds.h"
#include "nds/nds_internal.h"
#include "nds/tables.h"
#include "spec_internal.h"

constexpr size_t CAUGHT_OFFSET = 0x04;
constexpr size_t SEEN_OFFSET = 0x44;
constexpr size_t FIRST_GENDERS_OFFSET = 0x84;
constexpr size_t SECOND_GENDERS_OFFSET = 0xC4;
constexpr size_t FLAGS_SIZE = 0x40;
constexpr size_t SPINDA_OFFSET = 0x104;
constexpr size_t SHELLOS_OFFSET = 0x108;
constexpr size_t GASTRODON_OFFSET = 0x109;
constexpr size_t BURMY_OFFSET = 0x10A;
constexpr size_t WORMADAM_OFFSET = 0x10B;
constexpr size_t UNOWN_SEEN_OFFSET = 0x10C;
// Deoxys's first two forms sit in the caught flags' last byte, the other two in the seen flags'.
constexpr size_t DEOXYS_FLAGS_BYTE = FLAGS_SIZE - 1;
constexpr size_t PLAYER_NATIONAL_DEX_OFFSET = 0x21;
constexpr unsigned PLAYER_NATIONAL_DEX_BIT = 1;

constexpr uint16_t UNOWN = 201;
constexpr uint16_t PICHU = 172;
constexpr uint16_t DEOXYS = 386;
constexpr uint16_t BURMY = 412;
constexpr uint16_t WORMADAM = 413;
constexpr uint16_t SHELLOS = 422;
constexpr uint16_t GASTRODON = 423;
constexpr uint16_t ROTOM = 479;
constexpr uint16_t GIRATINA = 487;
constexpr uint16_t SHAYMIN = 492;
constexpr uint8_t GENDER_RATIO_GENDERLESS = 255;

constexpr size_t UNOWN_FORM_COUNT = 28;
constexpr size_t DEOXYS_FORM_COUNT = 4;
constexpr size_t ROTOM_FORM_COUNT = 6;
constexpr size_t TWO_FORM_COUNT = 2;
constexpr size_t THREE_FORM_COUNT = 3;
constexpr uint8_t NO_FORM_BYTE = 0xFF;
constexpr unsigned DEOXYS_FORM_BIT_COUNT = 4;
constexpr unsigned ROTOM_FORM_BIT_COUNT = 3;
constexpr unsigned THREE_FORM_BIT_COUNT = 2;
constexpr unsigned POKEDEX_LANGUAGE_BIT_COUNT = 6;

// Diamond and Pearl record languages for these species only (sMeister).
constexpr uint16_t DIAMOND_PEARL_LANGUAGE_SPECIES[] = {
    23, 25, 54, 77, 120, 129, 202, 214, 215, 216, 228, 278, 287, 315,
};
constexpr size_t DIAMOND_PEARL_LANGUAGE_SPECIES_COUNT =
    sizeof DIAMOND_PEARL_LANGUAGE_SPECIES / sizeof DIAMOND_PEARL_LANGUAGE_SPECIES[0];

// A list ends early at the encoding's end value, or holds every form.
static spec_nds_form_order_t form_order_of(const uint8_t *forms, size_t form_count,
                                           uint8_t end_value) {
    spec_nds_form_order_t order = {};
    while (order.count < form_count && forms[order.count] != end_value) {
        order.forms[order.count] = forms[order.count];
        ++order.count;
    }
    return order;
}

static spec_nds_form_order_t decode_packed_forms(uint32_t word, unsigned bit_count,
                                                 size_t form_count, bool is_seen) {
    if (!is_seen) {
        return (spec_nds_form_order_t){};
    }
    uint8_t forms[SPEC_NDS_FORM_ORDER_MAX];
    for (size_t index = 0; index < form_count; ++index) {
        forms[index] = (uint8_t)spec_get_bits(word, (unsigned)index * bit_count, bit_count);
    }
    return form_order_of(forms, form_count, (uint8_t)((1U << bit_count) - 1));
}

// Unfilled places keep the all-ones end value the Pokédex starts with.
static uint32_t encode_packed_forms(const spec_nds_form_order_t *order, unsigned bit_count) {
    uint32_t word = UINT32_MAX;
    for (size_t index = 0; index < order->count; ++index) {
        word = spec_set_bits(word, (unsigned)index * bit_count, bit_count, order->forms[index]);
    }
    return word;
}

// Genderless species are stored as male.
static spec_gender_t first_seen_gender_of(size_t national_number, bool is_first_female) {
    if (spec_nds_species_data[national_number].gender_ratio == GENDER_RATIO_GENDERLESS) {
        return SPEC_GENDER_GENDERLESS;
    }
    return is_first_female ? SPEC_GENDER_FEMALE : SPEC_GENDER_MALE;
}

static void decode_flags(spec_nds_pokedex_t *pokedex, const uint8_t *dex) {
    for (size_t national_number = 1; national_number < SPEC_NDS_POKEDEX_SIZE; ++national_number) {
        bool is_seen = spec_get_array_flag(&dex[SEEN_OFFSET], national_number - 1);
        bool is_first_female = spec_get_array_flag(&dex[FIRST_GENDERS_OFFSET], national_number - 1);
        bool is_second_female =
            spec_get_array_flag(&dex[SECOND_GENDERS_OFFSET], national_number - 1);
        pokedex->is_seen[national_number] = is_seen;
        pokedex->is_caught[national_number] =
            is_seen && spec_get_array_flag(&dex[CAUGHT_OFFSET], national_number - 1);
        pokedex->first_seen_gender[national_number] =
            first_seen_gender_of(national_number, is_first_female);
        pokedex->has_seen_both_genders[national_number] = is_second_female != is_first_female;
    }
}

static void decode_languages(uint8_t *languages, const uint8_t *dex,
                             const spec_nds_pokedex_layout_t *pokedex_layout) {
    const uint8_t *stored_languages = &dex[pokedex_layout->languages_offset];
    if (pokedex_layout->records_every_species_language) {
        for (size_t national_number = 1; national_number < SPEC_NDS_POKEDEX_SIZE;
             ++national_number) {
            languages[national_number] = stored_languages[national_number];
        }
        return;
    }
    for (size_t index = 0; index < DIAMOND_PEARL_LANGUAGE_SPECIES_COUNT; ++index) {
        languages[DIAMOND_PEARL_LANGUAGE_SPECIES[index]] = stored_languages[index];
    }
}

// As NumFormsSeen_TwoForms: the first form fills both bits until a second one comes.
static spec_nds_form_order_t decode_two_forms(uint8_t form_byte, bool is_seen) {
    if (!is_seen) {
        return (spec_nds_form_order_t){};
    }
    uint8_t forms[TWO_FORM_COUNT] = {
        (uint8_t)spec_get_bits(form_byte, 0, 1),
        (uint8_t)spec_get_bits(form_byte, 1, 1),
    };
    spec_nds_form_order_t order = {.count = 1, .forms = {forms[0]}};
    if (forms[1] != forms[0]) {
        order.forms[order.count++] = forms[1];
    }
    return order;
}

static spec_nds_form_order_t decode_unown_order(const uint8_t *letters) {
    return form_order_of(letters, UNOWN_FORM_COUNT, NO_FORM_BYTE);
}

static spec_nds_form_order_t decode_deoxys_order(uint8_t caught_byte, uint8_t seen_byte) {
    uint32_t nibbles = (uint32_t)caught_byte | ((uint32_t)seen_byte << 8);
    return decode_packed_forms(nibbles, DEOXYS_FORM_BIT_COUNT, DEOXYS_FORM_COUNT, true);
}

static void decode_forms(spec_nds_pokedex_forms_t *forms, const bool *is_seen, const uint8_t *dex,
                         const spec_nds_pokedex_layout_t *pokedex_layout) {
    forms->unown = decode_unown_order(&dex[UNOWN_SEEN_OFFSET]);
    forms->deoxys = decode_deoxys_order(dex[CAUGHT_OFFSET + DEOXYS_FLAGS_BYTE],
                                        dex[SEEN_OFFSET + DEOXYS_FLAGS_BYTE]);
    forms->shellos = decode_two_forms(dex[SHELLOS_OFFSET], is_seen[SHELLOS]);
    forms->gastrodon = decode_two_forms(dex[GASTRODON_OFFSET], is_seen[GASTRODON]);
    forms->burmy = decode_packed_forms(dex[BURMY_OFFSET], THREE_FORM_BIT_COUNT, THREE_FORM_COUNT,
                                       is_seen[BURMY]);
    forms->wormadam = decode_packed_forms(dex[WORMADAM_OFFSET], THREE_FORM_BIT_COUNT,
                                          THREE_FORM_COUNT, is_seen[WORMADAM]);
    if (pokedex_layout->has_platinum_forms) {
        forms->rotom = decode_packed_forms(spec_read_u32_le(&dex[pokedex_layout->rotom_offset]),
                                           ROTOM_FORM_BIT_COUNT, ROTOM_FORM_COUNT, is_seen[ROTOM]);
        forms->shaymin = decode_two_forms(dex[pokedex_layout->shaymin_offset], is_seen[SHAYMIN]);
        forms->giratina = decode_two_forms(dex[pokedex_layout->giratina_offset], is_seen[GIRATINA]);
    }
    if (pokedex_layout->has_heartgold_soulsilver_forms) {
        forms->unown_caught = decode_unown_order(&dex[pokedex_layout->unown_caught_offset]);
        forms->pichu = decode_packed_forms(dex[pokedex_layout->pichu_offset], THREE_FORM_BIT_COUNT,
                                           THREE_FORM_COUNT, is_seen[PICHU]);
    }
}

void spec_nds_decode_pokedex(spec_nds_pokedex_t *pokedex, const uint8_t *general,
                             const spec_nds_layout_t *layout) {
    const spec_nds_pokedex_layout_t *pokedex_layout = &layout->pokedex;
    const uint8_t *dex = &general[pokedex_layout->offset];
    pokedex->is_obtained = dex[pokedex_layout->obtained_offset] != 0;
    pokedex->has_national_dex = dex[pokedex_layout->national_dex_offset] != 0;
    pokedex->can_view_forms = dex[pokedex_layout->form_view_offset] != 0;
    pokedex->can_view_languages = dex[pokedex_layout->language_view_offset] != 0;
    pokedex->spinda_personality = spec_read_u32_le(&dex[SPINDA_OFFSET]);
    decode_flags(pokedex, dex);
    decode_languages(pokedex->languages, dex, pokedex_layout);
    decode_forms(&pokedex->forms, pokedex->is_seen, dex, pokedex_layout);
}

// As SetBit_Gender: the second array holds the other gender once both are seen.
static void encode_flags(uint8_t *dex, const spec_nds_pokedex_t *pokedex) {
    uint8_t caught[FLAGS_SIZE] = {};
    uint8_t seen[FLAGS_SIZE] = {};
    uint8_t first_genders[FLAGS_SIZE] = {};
    uint8_t second_genders[FLAGS_SIZE] = {};
    for (size_t national_number = 1; national_number < SPEC_NDS_POKEDEX_SIZE; ++national_number) {
        bool is_caught = pokedex->is_caught[national_number];
        bool is_first_female = pokedex->first_seen_gender[national_number] == SPEC_GENDER_FEMALE;
        bool is_second_female = is_first_female != pokedex->has_seen_both_genders[national_number];
        spec_set_array_flag(caught, national_number - 1, is_caught);
        spec_set_array_flag(seen, national_number - 1,
                            is_caught || pokedex->is_seen[national_number]);
        spec_set_array_flag(first_genders, national_number - 1, is_first_female);
        spec_set_array_flag(second_genders, national_number - 1, is_second_female);
    }
    uint32_t deoxys_nibbles = encode_packed_forms(&pokedex->forms.deoxys, DEOXYS_FORM_BIT_COUNT);
    caught[DEOXYS_FLAGS_BYTE] = (uint8_t)deoxys_nibbles;
    seen[DEOXYS_FLAGS_BYTE] = (uint8_t)(deoxys_nibbles >> 8);
    for (size_t index = 0; index < FLAGS_SIZE; ++index) {
        dex[CAUGHT_OFFSET + index] = caught[index];
        dex[SEEN_OFFSET + index] = seen[index];
        dex[FIRST_GENDERS_OFFSET + index] = first_genders[index];
        dex[SECOND_GENDERS_OFFSET + index] = second_genders[index];
    }
}

static void encode_languages(uint8_t *dex, const uint8_t *languages,
                             const spec_nds_pokedex_layout_t *pokedex_layout) {
    uint8_t *stored_languages = &dex[pokedex_layout->languages_offset];
    if (pokedex_layout->records_every_species_language) {
        for (size_t national_number = 1; national_number < SPEC_NDS_POKEDEX_SIZE;
             ++national_number) {
            stored_languages[national_number] = languages[national_number];
        }
        return;
    }
    for (size_t index = 0; index < DIAMOND_PEARL_LANGUAGE_SPECIES_COUNT; ++index) {
        stored_languages[index] = languages[DIAMOND_PEARL_LANGUAGE_SPECIES[index]];
    }
}

static uint8_t encode_two_forms(const spec_nds_form_order_t *order) {
    if (order->count == 0) {
        return NO_FORM_BYTE;
    }
    uint8_t second_form = order->count == 2 ? order->forms[1] : order->forms[0];
    uint32_t form_byte = spec_set_bits(NO_FORM_BYTE, 0, 1, order->forms[0]);
    return (uint8_t)spec_set_bits(form_byte, 1, 1, second_form);
}

static void encode_unown_order(uint8_t *letters, const spec_nds_form_order_t *order) {
    for (size_t index = 0; index < UNOWN_FORM_COUNT; ++index) {
        letters[index] = index < order->count ? order->forms[index] : NO_FORM_BYTE;
    }
}

// Deoxys's forms go with the caught and seen flags.
static void encode_forms(uint8_t *dex, const spec_nds_pokedex_forms_t *forms,
                         const spec_nds_pokedex_layout_t *pokedex_layout) {
    encode_unown_order(&dex[UNOWN_SEEN_OFFSET], &forms->unown);
    dex[SHELLOS_OFFSET] = encode_two_forms(&forms->shellos);
    dex[GASTRODON_OFFSET] = encode_two_forms(&forms->gastrodon);
    dex[BURMY_OFFSET] = (uint8_t)encode_packed_forms(&forms->burmy, THREE_FORM_BIT_COUNT);
    dex[WORMADAM_OFFSET] = (uint8_t)encode_packed_forms(&forms->wormadam, THREE_FORM_BIT_COUNT);
    if (pokedex_layout->has_platinum_forms) {
        spec_write_u32_le(&dex[pokedex_layout->rotom_offset],
                          encode_packed_forms(&forms->rotom, ROTOM_FORM_BIT_COUNT));
        dex[pokedex_layout->shaymin_offset] = encode_two_forms(&forms->shaymin);
        dex[pokedex_layout->giratina_offset] = encode_two_forms(&forms->giratina);
    }
    if (pokedex_layout->has_heartgold_soulsilver_forms) {
        encode_unown_order(&dex[pokedex_layout->unown_caught_offset], &forms->unown_caught);
        dex[pokedex_layout->pichu_offset] =
            (uint8_t)encode_packed_forms(&forms->pichu, THREE_FORM_BIT_COUNT);
    }
}

// As the National Dex event, which marks both the Pokédex and the trainer.
void spec_nds_encode_pokedex(uint8_t *general, const spec_nds_layout_t *layout,
                             const spec_nds_pokedex_t *pokedex) {
    const spec_nds_pokedex_layout_t *pokedex_layout = &layout->pokedex;
    uint8_t *dex = &general[pokedex_layout->offset];
    uint8_t *player_national_dex = &general[layout->player_offset + PLAYER_NATIONAL_DEX_OFFSET];
    dex[pokedex_layout->obtained_offset] = pokedex->is_obtained ? 1 : 0;
    dex[pokedex_layout->national_dex_offset] = pokedex->has_national_dex ? 1 : 0;
    *player_national_dex = (uint8_t)spec_set_bits(*player_national_dex, PLAYER_NATIONAL_DEX_BIT, 1,
                                                  pokedex->has_national_dex);
    dex[pokedex_layout->form_view_offset] = pokedex->can_view_forms ? 1 : 0;
    dex[pokedex_layout->language_view_offset] = pokedex->can_view_languages ? 1 : 0;
    spec_write_u32_le(&dex[SPINDA_OFFSET], pokedex->spinda_personality);
    encode_flags(dex, pokedex);
    encode_languages(dex, pokedex->languages, pokedex_layout);
    encode_forms(dex, &pokedex->forms, pokedex_layout);
}

static spec_error_t check_form_order(const spec_nds_form_order_t *order, uint16_t national_number,
                                     size_t form_count, bool does_game_record) {
    size_t game_form_count = does_game_record ? form_count : 0;
    if (order->count > game_form_count) {
        (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                        "the form order is longer than this game records");
        return spec_locate_error(SPEC_ERROR_LOCATION_POKEDEX, national_number, 0);
    }
    for (size_t index = 0; index < order->count; ++index) {
        if (order->forms[index] >= form_count) {
            (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "the form order has an unknown form");
            return spec_locate_error(SPEC_ERROR_LOCATION_POKEDEX, national_number, 0);
        }
    }
    return SPEC_OK;
}

static spec_error_t check_forms(const spec_nds_pokedex_forms_t *forms,
                                const spec_nds_pokedex_layout_t *pokedex_layout) {
    bool has_platinum_forms = pokedex_layout->has_platinum_forms;
    bool has_heartgold_soulsilver_forms = pokedex_layout->has_heartgold_soulsilver_forms;
    struct {
        const spec_nds_form_order_t *order;
        uint16_t national_number;
        size_t form_count;
        bool does_game_record;
    } const checks[] = {
        {&forms->unown, UNOWN, UNOWN_FORM_COUNT, true},
        {&forms->unown_caught, UNOWN, UNOWN_FORM_COUNT, has_heartgold_soulsilver_forms},
        {&forms->deoxys, DEOXYS, DEOXYS_FORM_COUNT, true},
        {&forms->shellos, SHELLOS, TWO_FORM_COUNT, true},
        {&forms->gastrodon, GASTRODON, TWO_FORM_COUNT, true},
        {&forms->burmy, BURMY, THREE_FORM_COUNT, true},
        {&forms->wormadam, WORMADAM, THREE_FORM_COUNT, true},
        {&forms->rotom, ROTOM, ROTOM_FORM_COUNT, has_platinum_forms},
        {&forms->shaymin, SHAYMIN, TWO_FORM_COUNT, has_platinum_forms},
        {&forms->giratina, GIRATINA, TWO_FORM_COUNT, has_platinum_forms},
        {&forms->pichu, PICHU, THREE_FORM_COUNT, has_heartgold_soulsilver_forms},
    };
    for (size_t index = 0; index < sizeof checks / sizeof checks[0]; ++index) {
        spec_error_t error =
            check_form_order(checks[index].order, checks[index].national_number,
                             checks[index].form_count, checks[index].does_game_record);
        if (error != SPEC_OK) {
            return error;
        }
    }
    return SPEC_OK;
}

static bool does_diamond_pearl_record_language(uint16_t national_number) {
    for (size_t index = 0; index < DIAMOND_PEARL_LANGUAGE_SPECIES_COUNT; ++index) {
        if (DIAMOND_PEARL_LANGUAGE_SPECIES[index] == national_number) {
            return true;
        }
    }
    return false;
}

static spec_error_t check_languages(const uint8_t *languages,
                                    const spec_nds_pokedex_layout_t *pokedex_layout) {
    for (uint16_t national_number = 1; national_number < SPEC_NDS_POKEDEX_SIZE; ++national_number) {
        uint8_t language_flags = languages[national_number];
        bool does_game_record = pokedex_layout->records_every_species_language
                                || does_diamond_pearl_record_language(national_number);
        if (!spec_fits_in_bits(language_flags, POKEDEX_LANGUAGE_BIT_COUNT)
            || (!does_game_record && language_flags != 0)) {
            (void)spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                            "the Pokédex records no such languages for this species");
            return spec_locate_error(SPEC_ERROR_LOCATION_POKEDEX, national_number, 0);
        }
    }
    return SPEC_OK;
}

spec_error_t spec_nds_check_pokedex(const spec_nds_pokedex_t *pokedex,
                                    const spec_nds_layout_t *layout) {
    spec_error_t error = check_languages(pokedex->languages, &layout->pokedex);
    if (error != SPEC_OK) {
        return error;
    }
    return check_forms(&pokedex->forms, &layout->pokedex);
}
