// The Gen 1 character sets, to and from UTF-8, and the text engine Gen 2 shares.

#include <string.h>

#include "gb/gb.h"
#include "gb/gb_internal.h"
#include "gb/tables.h"
#include "spec_internal.h"

constexpr char32_t FIRST_HIRAGANA = 0x3041;
constexpr char32_t LAST_HIRAGANA = 0x3096;
constexpr char32_t FIRST_KATAKANA = 0x30A1;
constexpr char32_t LAST_KATAKANA = 0x30FA;

static const spec_gb_character_t *charmap_of(spec_language_t language) {
    switch (language) {
        case SPEC_LANGUAGE_JAPANESE:
            return spec_gb_charmap_japanese;
        case SPEC_LANGUAGE_ENGLISH:
            return spec_gb_charmap_english;
        case SPEC_LANGUAGE_FRENCH:
        case SPEC_LANGUAGE_GERMAN:
            return spec_gb_charmap_french_german;
        case SPEC_LANGUAGE_ITALIAN:
        case SPEC_LANGUAGE_SPANISH:
            return spec_gb_charmap_italian_spanish;
        default:
            return nullptr;
    }
}

static bool is_hiragana(char32_t code_point) {
    return code_point >= FIRST_HIRAGANA && code_point <= LAST_HIRAGANA;
}

static bool is_kana(char32_t code_point) {
    return is_hiragana(code_point) || (code_point >= FIRST_KATAKANA && code_point <= LAST_KATAKANA);
}

// A tile both kana share reads as the name's first other kana does.
static bool reads_as_hiragana(const uint8_t *text, size_t text_size,
                              const spec_gb_character_t *charmap) {
    for (size_t index = 0; index < text_size && text[index] != SPEC_GB_END_OF_TEXT; ++index) {
        const spec_gb_character_t *character = &charmap[text[index]];
        if (character->hiragana == 0 && is_kana(character->code_points[0])) {
            return is_hiragana(character->code_points[0]);
        }
    }
    return false;
}

// A ligature byte decodes to two characters; control codes decode to U+FFFD.
spec_error_t spec_gb_decode_text(char8_t *utf8, const uint8_t *text, size_t text_size,
                                 const spec_gb_character_t *charmap) {
    if (text_size > SPEC_GB_TEXT_MAX_SIZE) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                         "text_size is larger than any Game Boy name field");
    }
    bool is_hiragana_name = reads_as_hiragana(text, text_size, charmap);
    size_t length = 0;
    for (size_t index = 0; index < text_size && text[index] != SPEC_GB_END_OF_TEXT; ++index) {
        const spec_gb_character_t *character = &charmap[text[index]];
        char32_t first = character->code_points[0];
        if (first == 0) {
            length += spec_utf8_write(&utf8[length], SPEC_REPLACEMENT_CHARACTER);
            continue;
        }
        if (is_hiragana_name && character->hiragana != 0) {
            first = character->hiragana;
        }
        length += spec_utf8_write(&utf8[length], first);
        if (character->code_points[1] != 0) {
            length += spec_utf8_write(&utf8[length], character->code_points[1]);
        }
    }
    utf8[length] = '\0';
    return SPEC_OK;
}

static bool find_ligature(const spec_gb_character_t *charmap, char32_t first, char32_t second,
                          uint8_t *byte) {
    for (size_t index = 0; index < SPEC_GB_CHARMAP_SIZE; ++index) {
        const spec_gb_character_t *character = &charmap[index];
        if (!character->is_read_only && character->code_points[1] != 0
            && character->code_points[0] == first && character->code_points[1] == second) {
            *byte = (uint8_t)index;
            return true;
        }
    }
    return false;
}

static bool find_character(const spec_gb_character_t *charmap, char32_t code_point, uint8_t *byte) {
    // Empty charmap entries hold 0.
    if (code_point == 0) {
        return false;
    }
    for (size_t index = 0; index < SPEC_GB_CHARMAP_SIZE; ++index) {
        const spec_gb_character_t *character = &charmap[index];
        bool is_match =
            character->code_points[0] == code_point || character->hiragana == code_point;
        if (!character->is_read_only && character->code_points[1] == 0 && is_match) {
            *byte = (uint8_t)index;
            return true;
        }
    }
    return false;
}

// Ligatures match first; the name is padded with terminators, as the games pad species names.
spec_error_t spec_gb_encode_text(uint8_t *text, size_t text_size, const char8_t *utf8,
                                 const spec_gb_character_t *charmap) {
    if (text_size == 0 || text_size > SPEC_GB_ITEM_NAME_SIZE) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                         "text_size is longer than any Game Boy text");
    }
    uint8_t encoded[SPEC_GB_ITEM_NAME_SIZE];
    size_t length = 0;
    size_t position = 0;
    while (utf8[position] != '\0') {
        char32_t first = 0;
        size_t first_length = spec_utf8_read(&utf8[position], &first);
        if (first_length == 0) {
            return spec_fail(SPEC_ERROR_INVALID_UTF8, "the name is not valid UTF-8");
        }
        char32_t second = 0;
        size_t second_length = utf8[position + first_length] == '\0'
                                   ? 0
                                   : spec_utf8_read(&utf8[position + first_length], &second);
        uint8_t byte = 0;
        if (second_length != 0 && find_ligature(charmap, first, second, &byte)) {
            position += first_length + second_length;
        } else if (find_character(charmap, first, &byte)) {
            position += first_length;
        } else {
            return spec_fail(SPEC_ERROR_UNENCODABLE_CHARACTER,
                             "the name has a character this language's charset lacks");
        }
        if (length + 1 == text_size) {
            return spec_fail(SPEC_ERROR_NAME_TOO_LONG, "the name is longer than its field");
        }
        encoded[length++] = byte;
    }
    memcpy(text, encoded, length);
    memset(&text[length], SPEC_GB_END_OF_TEXT, text_size - length);
    return SPEC_OK;
}

size_t spec_gb_name_size(spec_language_t language) {
    return language == SPEC_LANGUAGE_JAPANESE ? SPEC_GB_JAPANESE_NAME_SIZE : SPEC_GB_NAME_SIZE;
}

spec_error_t spec_gb_text_from_utf8(uint8_t *text, size_t text_size, const char8_t *utf8,
                                    spec_language_t language) {
    const spec_gb_character_t *charmap = charmap_of(language);
    if (charmap == nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "no Gen 1 game is in that language");
    }
    return spec_gb_encode_text(text, text_size, utf8, charmap);
}

spec_error_t spec_gb_text_to_utf8(char8_t utf8[static SPEC_GB_TEXT_BUFFER_SIZE],
                                  const uint8_t *text, size_t text_size, spec_language_t language) {
    const spec_gb_character_t *charmap = charmap_of(language);
    if (charmap == nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "no Gen 1 game is in that language");
    }
    return spec_gb_decode_text(utf8, text, text_size, charmap);
}
