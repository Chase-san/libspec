// The Gen 4 character set, to and from UTF-8.

#include "nds/nds.h"
#include "nds/nds_internal.h"
#include "nds/tables.h"
#include "spec_internal.h"

constexpr uint16_t END_OF_TEXT = 0xFFFF;
constexpr uint16_t FULL_WIDTH_MALE = 0x00EE;
constexpr uint16_t FULL_WIDTH_FEMALE = 0x00EF;
constexpr uint16_t HALF_WIDTH_MALE = 0x01BB;
constexpr uint16_t HALF_WIDTH_FEMALE = 0x01BC;

// TODO: Check the codes of the first 15 Korean syllables on a Korean cart.
static char32_t code_point_of(uint16_t code) {
    if (code >= SPEC_NDS_CHARMAP_SIZE || spec_nds_charmap[code] == 0) {
        return SPEC_REPLACEMENT_CHARACTER;
    }
    return spec_nds_charmap[code];
}

// Scanning from code 0 finds the full-width ♂ and ♀ first.
static bool find_code(char32_t code_point, uint16_t *code) {
    // Empty charmap entries hold 0.
    if (code_point == 0) {
        return false;
    }
    for (size_t index = 0; index < SPEC_NDS_CHARMAP_SIZE; ++index) {
        if (spec_nds_charmap[index] == code_point) {
            *code = (uint16_t)index;
            return true;
        }
    }
    return false;
}

// Only Japanese text writes the full-width ♂ and ♀.
static uint16_t code_for_language(uint16_t code, spec_language_t language) {
    if (language == SPEC_LANGUAGE_JAPANESE) {
        return code;
    }
    if (code == FULL_WIDTH_MALE) {
        return HALF_WIDTH_MALE;
    }
    if (code == FULL_WIDTH_FEMALE) {
        return HALF_WIDTH_FEMALE;
    }
    return code;
}

spec_error_t spec_nds_text_to_utf8(char8_t utf8[static SPEC_NDS_TEXT_BUFFER_SIZE],
                                   const uint16_t *text, size_t text_size) {
    if (text_size > SPEC_NDS_TEXT_MAX_SIZE) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                         "text_size is larger than any Gen 4 text field");
    }
    size_t length = 0;
    for (size_t index = 0; index < text_size && text[index] != END_OF_TEXT; ++index) {
        length += spec_utf8_write(&utf8[length], code_point_of(text[index]));
    }
    utf8[length] = '\0';
    return SPEC_OK;
}

// The units after the terminator are kept, as the naming screen leaves them.
spec_error_t spec_nds_text_from_utf8(uint16_t *text, size_t text_size, const char8_t *utf8,
                                     spec_language_t language) {
    if (text_size == 0 || text_size > SPEC_NDS_TEXT_MAX_SIZE) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                         "text_size is not the size of a Gen 4 text field");
    }
    uint16_t encoded[SPEC_NDS_TEXT_MAX_SIZE];
    size_t length = 0;
    size_t position = 0;
    while (utf8[position] != '\0') {
        char32_t code_point = 0;
        size_t utf8_length = spec_utf8_read(&utf8[position], &code_point);
        if (utf8_length == 0) {
            return spec_fail(SPEC_ERROR_INVALID_UTF8, "the name is not valid UTF-8");
        }
        uint16_t code = 0;
        if (!find_code(code_point, &code)) {
            return spec_fail(SPEC_ERROR_UNENCODABLE_CHARACTER,
                             "the name has a character the Gen 4 charset lacks");
        }
        if (length + 1 == text_size) {
            return spec_fail(SPEC_ERROR_NAME_TOO_LONG, "the name is longer than its field");
        }
        encoded[length++] = code_for_language(code, language);
        position += utf8_length;
    }
    for (size_t index = 0; index < length; ++index) {
        text[index] = encoded[index];
    }
    text[length] = END_OF_TEXT;
    return SPEC_OK;
}

void spec_nds_read_text(uint16_t *text, const uint8_t *bytes, size_t text_size) {
    for (size_t index = 0; index < text_size; ++index) {
        text[index] = spec_read_u16_le(&bytes[index * 2]);
    }
}

void spec_nds_write_text(uint8_t *bytes, const uint16_t *text, size_t text_size) {
    for (size_t index = 0; index < text_size; ++index) {
        spec_write_u16_le(&bytes[index * 2], text[index]);
    }
}
