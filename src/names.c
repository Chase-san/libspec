// Names by number and language: species, forms, moves, abilities, items, natures and types.

#include "spec.h"
#include "spec_internal.h"
#include "spec_tables.h"

constexpr uint16_t ARCEUS = 493;
// Gen 4 numbered Arceus's forms by its own type order, which held ??? (pret TYPE_MYSTERY).
constexpr uint8_t GEN4_ARCEUS_MYSTERY_FORM = 9;

static bool is_name_language(spec_language_t language) {
    return language < SPEC_NAME_LANGUAGE_COUNT;
}

static bool has_species_name(uint16_t national_number, spec_language_t language) {
    return national_number < SPEC_SPECIES_NAME_COUNT && is_name_language(language);
}

const char8_t *spec_game_boy_species_name(uint16_t national_number, spec_language_t language) {
    if (!has_species_name(national_number, language)) {
        return nullptr;
    }
    return (const char8_t *)spec_game_boy_species_names[language][national_number];
}

const char8_t *spec_gen5_species_name(uint16_t national_number, spec_language_t language) {
    if (!has_species_name(national_number, language)) {
        return nullptr;
    }
    return (const char8_t *)spec_gen5_species_names[language][national_number];
}

const char *spec_item_name(uint16_t item, spec_language_t language) {
    if (item >= SPEC_ITEM_NAME_COUNT || !is_name_language(language)) {
        return nullptr;
    }
    return spec_item_names[language][item];
}

const char8_t *spec_upper_case_species_name(uint16_t national_number, spec_language_t language) {
    if (!has_species_name(national_number, language)) {
        return nullptr;
    }
    return (const char8_t *)spec_upper_case_species_names[language][national_number];
}

const char *spec_ability_name(uint16_t ability, spec_language_t language) {
    if (ability >= SPEC_ABILITY_NAME_COUNT || !is_name_language(language)) {
        return nullptr;
    }
    return spec_ability_names[language][ability];
}

static bool is_gen4(spec_game_type_t type) {
    return type == SPEC_GAME_TYPE_DIAMOND_PEARL || type == SPEC_GAME_TYPE_PLATINUM
           || type == SPEC_GAME_TYPE_HEARTGOLD_SOULSILVER;
}

// Game types count up generation by generation, so the latest row begun by the type is in force.
static const spec_form_names_t *form_names_in(spec_game_type_t type, uint16_t national_number,
                                              uint8_t form) {
    const spec_form_names_t *latest = nullptr;
    for (size_t index = 0; index < spec_form_names_count; ++index) {
        const spec_form_names_t *form_names = &spec_form_names[index];
        bool applies = form_names->national_number == national_number && form_names->form == form
                       && form_names->from_game <= type;
        if (applies && (latest == nullptr || form_names->from_game >= latest->from_game)) {
            latest = form_names;
        }
    }
    return latest;
}

const char *spec_form_name(spec_game_type_t type, uint16_t national_number, uint8_t form,
                           spec_language_t language) {
    if (!is_name_language(language)) {
        return nullptr;
    }
    if (is_gen4(type) && national_number == ARCEUS && form >= GEN4_ARCEUS_MYSTERY_FORM) {
        if (form == GEN4_ARCEUS_MYSTERY_FORM) {
            return nullptr;
        }
        --form;
    }
    const spec_form_names_t *form_names = form_names_in(type, national_number, form);
    return form_names == nullptr ? nullptr : form_names->names[language];
}

const char *spec_move_name(uint16_t move, spec_language_t language) {
    if (move >= SPEC_MOVE_NAME_COUNT || !is_name_language(language)) {
        return nullptr;
    }
    return spec_move_names[language][move];
}

const char *spec_nature_name(spec_nature_t nature, spec_language_t language) {
    if (nature >= SPEC_NATURE_COUNT || !is_name_language(language)) {
        return nullptr;
    }
    return spec_nature_names[language][nature];
}

const char *spec_species_name(uint16_t national_number, spec_language_t language) {
    if (!has_species_name(national_number, language)) {
        return nullptr;
    }
    return spec_species_names[language][national_number];
}

const char *spec_type_name(spec_type_t type, spec_language_t language) {
    if (type >= SPEC_TYPE_COUNT || !is_name_language(language)) {
        return nullptr;
    }
    return spec_type_names[language][type];
}
