// Gen 6 and 7 text, UTF-16 with Game Freak's glyphs, to and from UTF-8.

#include "3ds/3ds.h"
#include "3ds/tables.h"
#include "spec_internal.h"

constexpr uint16_t END_OF_TEXT = 0x0000;
// The games' half-width ♂ and ♀.
constexpr uint16_t HALF_WIDTH_MALE = 0xE08E;
constexpr uint16_t HALF_WIDTH_FEMALE = 0xE08F;
constexpr char32_t MALE_SIGN = 0x2642;
constexpr char32_t FEMALE_SIGN = 0x2640;
constexpr char32_t FIRST_SURROGATE = 0xD800;
constexpr char32_t LAST_SURROGATE = 0xDFFF;
constexpr char32_t LAST_UNIT = 0xFFFF;

static bool is_surrogate(char32_t code_point) {
    return code_point >= FIRST_SURROGATE && code_point <= LAST_SURROGATE;
}

static bool is_storable(char32_t code_point) {
    return code_point != END_OF_TEXT && code_point <= LAST_UNIT && !is_surrogate(code_point);
}

// Only Japanese text writes the full-width ♂ and ♀, as in Gen 4 and 5.
static uint16_t unit_of(char32_t code_point, spec_language_t language) {
    bool is_half_width = language != SPEC_LANGUAGE_JAPANESE;
    if (code_point == MALE_SIGN && is_half_width) {
        return HALF_WIDTH_MALE;
    }
    if (code_point == FEMALE_SIGN && is_half_width) {
        return HALF_WIDTH_FEMALE;
    }
    return (uint16_t)code_point;
}

// The units after the terminator are kept, as the naming screen leaves them.
spec_error_t spec_3ds_text_from_utf8(uint16_t *text, size_t text_size, const char8_t *utf8,
                                     spec_language_t language) {
    if (text_size == 0 || text_size > SPEC_3DS_TEXT_MAX_SIZE) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                         "text_size is not the size of a Gen 6 or 7 text field");
    }
    uint16_t encoded[SPEC_3DS_TEXT_MAX_SIZE];
    size_t length = 0;
    size_t position = 0;
    while (utf8[position] != '\0') {
        char32_t code_point = 0;
        size_t utf8_length = spec_utf8_read(&utf8[position], &code_point);
        if (utf8_length == 0) {
            return spec_fail(SPEC_ERROR_INVALID_UTF8, "the name is not valid UTF-8");
        }
        if (!is_storable(code_point)) {
            return spec_fail(SPEC_ERROR_UNENCODABLE_CHARACTER,
                             "the name has a character Gen 6 and 7 cannot store");
        }
        if (length + 1 == text_size) {
            return spec_fail(SPEC_ERROR_NAME_TOO_LONG, "the name is longer than its field");
        }
        encoded[length++] = unit_of(code_point, language);
        position += utf8_length;
    }
    for (size_t index = 0; index < length; ++index) {
        text[index] = encoded[index];
    }
    text[length] = END_OF_TEXT;
    return SPEC_OK;
}

// Gen 7 writes the Chinese species names in glyphs of its own.
static char32_t code_point_of(uint16_t unit) {
    if (unit == HALF_WIDTH_MALE) {
        return MALE_SIGN;
    }
    if (unit == HALF_WIDTH_FEMALE) {
        return FEMALE_SIGN;
    }
    size_t glyph = (size_t)unit - SPEC_3DS_FIRST_CHINESE_GLYPH;
    if (unit >= SPEC_3DS_FIRST_CHINESE_GLYPH && glyph < SPEC_3DS_CHINESE_GLYPH_COUNT
        && spec_3ds_chinese_glyphs[glyph] != 0) {
        return spec_3ds_chinese_glyphs[glyph];
    }
    if (is_surrogate(unit)) {
        return SPEC_REPLACEMENT_CHARACTER;
    }
    return unit;
}

spec_error_t spec_3ds_text_to_utf8(char8_t utf8[static SPEC_3DS_TEXT_BUFFER_SIZE],
                                   const uint16_t *text, size_t text_size) {
    if (text_size > SPEC_3DS_TEXT_MAX_SIZE) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                         "text_size is larger than any Gen 6 or 7 text field");
    }
    size_t length = 0;
    for (size_t index = 0; index < text_size && text[index] != END_OF_TEXT; ++index) {
        length += spec_utf8_write(&utf8[length], code_point_of(text[index]));
    }
    utf8[length] = '\0';
    return SPEC_OK;
}
