// Gen 5 text, UTF-16 with Game Freak's glyphs, to and from UTF-8.

#include "ndsi/ndsi.h"
#include "ndsi/ndsi_internal.h"
#include "spec_internal.h"

constexpr uint16_t END_OF_TEXT = 0xFFFF;
// The games draw their own glyphs over U+2460-U+2487, which Gen 6 and 7 store at U+E081 on.
constexpr char16_t FIRST_GAME_GLYPH = 0x2460;
constexpr char16_t LAST_GAME_GLYPH = 0x2487;
constexpr char32_t FIRST_PRIVATE_USE_GLYPH = 0xE081;
constexpr char32_t LAST_PRIVATE_USE_GLYPH = 0xE0A8;
constexpr char16_t HALF_WIDTH_MALE = 0x246D;
constexpr char16_t HALF_WIDTH_FEMALE = 0x246E;
constexpr char32_t MALE_SIGN = 0x2642;
constexpr char32_t FEMALE_SIGN = 0x2640;
constexpr char32_t FIRST_SURROGATE = 0xD800;
constexpr char32_t LAST_SURROGATE = 0xDFFF;

static bool is_surrogate(char32_t code_point) {
    return code_point >= FIRST_SURROGATE && code_point <= LAST_SURROGATE;
}

static char32_t code_point_of(uint16_t unit) {
    if (unit == HALF_WIDTH_MALE) {
        return MALE_SIGN;
    }
    if (unit == HALF_WIDTH_FEMALE) {
        return FEMALE_SIGN;
    }
    if (unit >= FIRST_GAME_GLYPH && unit <= LAST_GAME_GLYPH) {
        return FIRST_PRIVATE_USE_GLYPH + (unit - FIRST_GAME_GLYPH);
    }
    if (unit == 0 || is_surrogate(unit)) {
        return SPEC_REPLACEMENT_CHARACTER;
    }
    return unit;
}

static bool is_storable(char32_t code_point) {
    return code_point != 0 && code_point < END_OF_TEXT && !is_surrogate(code_point);
}

// Only Japanese text writes the full-width ♂ and ♀.
static uint16_t unit_of(char32_t code_point, spec_language_t language) {
    bool is_international = language != SPEC_LANGUAGE_JAPANESE;
    if (code_point == MALE_SIGN && is_international) {
        return HALF_WIDTH_MALE;
    }
    if (code_point == FEMALE_SIGN && is_international) {
        return HALF_WIDTH_FEMALE;
    }
    if (code_point >= FIRST_PRIVATE_USE_GLYPH && code_point <= LAST_PRIVATE_USE_GLYPH) {
        return (uint16_t)(FIRST_GAME_GLYPH + (code_point - FIRST_PRIVATE_USE_GLYPH));
    }
    return (uint16_t)code_point;
}

spec_error_t spec_ndsi_text_to_utf8(char8_t utf8[static SPEC_NDSI_TEXT_BUFFER_SIZE],
                                    const uint16_t *text, size_t text_size) {
    if (text_size > SPEC_NDSI_TEXT_MAX_SIZE) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                         "text_size is larger than any Gen 5 text field");
    }
    size_t length = 0;
    for (size_t index = 0; index < text_size && text[index] != END_OF_TEXT; ++index) {
        length += spec_utf8_write(&utf8[length], code_point_of(text[index]));
    }
    utf8[length] = '\0';
    return SPEC_OK;
}

// The units after the terminator are kept, as the naming screen leaves them.
spec_error_t spec_ndsi_text_from_utf8(uint16_t *text, size_t text_size, const char8_t *utf8,
                                      spec_language_t language) {
    if (text_size == 0 || text_size > SPEC_NDSI_TEXT_MAX_SIZE) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE,
                         "text_size is not the size of a Gen 5 text field");
    }
    uint16_t encoded[SPEC_NDSI_TEXT_MAX_SIZE];
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
                             "the name has a character Gen 5 cannot store");
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
