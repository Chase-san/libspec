#include "spec.h"
#include "spec_internal.h"

static thread_local spec_error_data_t last_error;

const char *spec_error_string(spec_error_t error) {
    switch (error) {
        case SPEC_OK:
            return "ok";
        case SPEC_ERROR_INVALID_SAVE:
            return "invalid save";
        case SPEC_ERROR_VALUE_OUT_OF_RANGE:
            return "value out of range";
        case SPEC_ERROR_INVALID_UTF8:
            return "invalid UTF-8";
        case SPEC_ERROR_UNENCODABLE_CHARACTER:
            return "character the game cannot store";
        case SPEC_ERROR_NAME_TOO_LONG:
            return "name too long";
        case SPEC_ERROR_INVALID_ITEM:
            return "item not in this game";
        case SPEC_ERROR_WRONG_POCKET:
            return "item in the wrong pocket";
    }
    return "unknown error";
}

spec_error_data_t spec_last_error(void) {
    return last_error;
}

spec_error_t spec_fail(spec_error_t error, const char *message) {
    last_error = (spec_error_data_t){
        .message = message,
        .error = error,
        .location = SPEC_ERROR_LOCATION_NONE,
    };
    return error;
}

spec_error_t spec_locate_error(spec_error_location_t location, uint32_t index0, uint32_t index1) {
    last_error.location = location;
    last_error.index0 = index0;
    last_error.index1 = index1;
    return last_error.error;
}
