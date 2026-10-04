#include "spec_internal.h"

constexpr char32_t LAST_CODE_POINT = 0x10FFFF;
constexpr char32_t FIRST_SURROGATE = 0xD800;
constexpr char32_t LAST_SURROGATE = 0xDFFF;

static size_t length_of_lead_byte(char8_t lead_byte) {
    if (lead_byte < 0x80) {
        return 1;
    }
    if ((lead_byte & 0xE0) == 0xC0) {
        return 2;
    }
    if ((lead_byte & 0xF0) == 0xE0) {
        return 3;
    }
    if ((lead_byte & 0xF8) == 0xF0) {
        return 4;
    }
    return 0;
}

static size_t length_of_code_point(char32_t code_point) {
    if (code_point < 0x80) {
        return 1;
    }
    if (code_point < 0x800) {
        return 2;
    }
    if (code_point < 0x10000) {
        return 3;
    }
    return 4;
}

static bool is_continuation_byte(char8_t byte) {
    return (byte & 0xC0) == 0x80;
}

static bool is_surrogate(char32_t code_point) {
    return code_point >= FIRST_SURROGATE && code_point <= LAST_SURROGATE;
}

size_t spec_utf8_read(const char8_t *utf8, char32_t *code_point) {
    static const char8_t LEAD_BYTE_PAYLOAD_MASKS[] = {0, 0x7F, 0x1F, 0x0F, 0x07};
    size_t length = length_of_lead_byte(utf8[0]);
    if (length == 0) {
        return 0;
    }
    char32_t assembled = utf8[0] & LEAD_BYTE_PAYLOAD_MASKS[length];
    for (size_t index = 1; index < length; ++index) {
        // NUL is not a continuation byte, so truncated input stops here.
        if (!is_continuation_byte(utf8[index])) {
            return 0;
        }
        assembled = (assembled << 6) | (utf8[index] & 0x3F);
    }
    bool is_overlong = length_of_code_point(assembled) != length;
    if (is_overlong || is_surrogate(assembled) || assembled > LAST_CODE_POINT) {
        return 0;
    }
    *code_point = assembled;
    return length;
}

size_t spec_utf8_write(char8_t *utf8, char32_t code_point) {
    static const char8_t LEAD_BYTE_TAGS[] = {0, 0x00, 0xC0, 0xE0, 0xF0};
    size_t length = length_of_code_point(code_point);
    char32_t remaining_bits = code_point;
    for (size_t index = length - 1; index > 0; --index) {
        utf8[index] = (char8_t)(0x80 | (remaining_bits & 0x3F));
        remaining_bits >>= 6;
    }
    utf8[0] = (char8_t)(LEAD_BYTE_TAGS[length] | remaining_bits);
    return length;
}
