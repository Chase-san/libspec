#include <string.h>

#include "gba/gba.h"
#include "gba/tables.h"
#include "spec_internal.h"

constexpr uint8_t END_OF_TEXT = 0xFF;

static const uint16_t *charmap_of(spec_language_t language) {
    switch (language) {
        case SPEC_LANGUAGE_JAPANESE:
            return spec_gba_charmap_japanese;
        case SPEC_LANGUAGE_FRENCH:
            return spec_gba_charmap_french;
        case SPEC_LANGUAGE_GERMAN:
            return spec_gba_charmap_german;
        default:
            return spec_gba_charmap_international;
    }
}

static char32_t code_point_of(const uint16_t *charmap, uint8_t byte) {
    if (byte >= SPEC_GBA_CHARMAP_SIZE || charmap[byte] == 0) {
        return SPEC_REPLACEMENT_CHARACTER;
    }
    return charmap[byte];
}

static bool find_byte(const uint16_t *charmap, char32_t code_point, uint8_t *byte) {
    // Empty charmap entries hold 0.
    if (code_point == 0) {
        return false;
    }
    for (size_t index = 0; index < SPEC_GBA_CHARMAP_SIZE; ++index) {
        if (charmap[index] == code_point) {
            *byte = (uint8_t)index;
            return true;
        }
    }
    return false;
}

spec_error_t spec_gba_text_to_utf8(char8_t utf8[static SPEC_GBA_TEXT_BUFFER_SIZE],
                                   const uint8_t *text, size_t text_size,
                                   spec_language_t language) {
    if (text_size > SPEC_GBA_TEXT_MAX_SIZE) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                         "text_size is larger than any Gen 3 text field");
    }
    const uint16_t *charmap = charmap_of(language);
    size_t length = 0;
    for (size_t index = 0; index < text_size && text[index] != END_OF_TEXT; ++index) {
        length += spec_utf8_write(&utf8[length], code_point_of(charmap, text[index]));
    }
    utf8[length] = '\0';
    return SPEC_OK;
}

spec_error_t spec_gba_text_from_utf8(uint8_t *text, size_t text_size, const char8_t *utf8,
                                     spec_language_t language) {
    if (text_size > SPEC_GBA_TEXT_MAX_SIZE) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                         "text_size is larger than any Gen 3 text field");
    }
    const uint16_t *charmap = charmap_of(language);
    uint8_t encoded[SPEC_GBA_TEXT_MAX_SIZE];
    size_t length = 0;
    size_t position = 0;
    while (utf8[position] != '\0') {
        char32_t code_point = 0;
        size_t utf8_length = spec_utf8_read(&utf8[position], &code_point);
        if (utf8_length == 0) {
            return spec_fail(SPEC_ERROR_INVALID_UTF8, "the name is not valid UTF-8");
        }
        uint8_t byte = 0;
        if (!find_byte(charmap, code_point, &byte)) {
            return spec_fail(SPEC_ERROR_UNENCODABLE_CHARACTER,
                             "the name has a character this language's charset lacks");
        }
        if (length == text_size) {
            return spec_fail(SPEC_ERROR_NAME_TOO_LONG, "the name is longer than its field");
        }
        encoded[length++] = byte;
        position += utf8_length;
    }
    // A name that fills its field has no terminator.
    memcpy(text, encoded, length);
    memset(&text[length], END_OF_TEXT, text_size - length);
    return SPEC_OK;
}
