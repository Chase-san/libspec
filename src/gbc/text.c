// The Gen 2 character sets, to and from UTF-8, through the engine Gen 1 shares.

#include "gb/gb_internal.h"
#include "gbc/gbc.h"
#include "gbc/tables.h"
#include "spec_internal.h"

static const spec_gb_character_t *charmap_of(spec_language_t language) {
    switch (language) {
        case SPEC_LANGUAGE_JAPANESE:
            return spec_gbc_charmap_japanese;
        case SPEC_LANGUAGE_ENGLISH:
            return spec_gbc_charmap_english;
        case SPEC_LANGUAGE_FRENCH:
        case SPEC_LANGUAGE_GERMAN:
            return spec_gbc_charmap_french_german;
        case SPEC_LANGUAGE_ITALIAN:
        case SPEC_LANGUAGE_SPANISH:
            return spec_gbc_charmap_italian_spanish;
        default:
            return nullptr;
    }
}

spec_error_t spec_gbc_text_to_utf8(char8_t utf8[static SPEC_GBC_TEXT_BUFFER_SIZE],
                                   const uint8_t *text, size_t text_size,
                                   spec_language_t language) {
    const spec_gb_character_t *charmap = charmap_of(language);
    if (charmap == nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "no Gen 2 game is in that language");
    }
    return spec_gb_decode_text(utf8, text, text_size, charmap);
}

spec_error_t spec_gbc_text_from_utf8(uint8_t *text, size_t text_size, const char8_t *utf8,
                                     spec_language_t language) {
    const spec_gb_character_t *charmap = charmap_of(language);
    if (charmap == nullptr) {
        return spec_fail(SPEC_ERROR_VALUE_OUT_OF_RANGE, "no Gen 2 game is in that language");
    }
    return spec_gb_encode_text(text, text_size, utf8, charmap);
}
