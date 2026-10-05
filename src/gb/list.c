// Pokémon lists, as the Game Boy games store the party and the boxes.

#include "gb/gb_internal.h"

constexpr size_t COUNT_SIZE = 1;

size_t spec_gb_list_nickname_offset(const spec_gb_list_shape_t *shape, size_t index) {
    return spec_gb_list_trainer_name_offset(shape, shape->capacity) + index * shape->name_size;
}

// Species bytes, then their terminator.
static size_t species_bytes_size(const spec_gb_list_shape_t *shape) {
    return shape->capacity + 1;
}

size_t spec_gb_list_record_offset(const spec_gb_list_shape_t *shape, size_t index) {
    return COUNT_SIZE + species_bytes_size(shape) + index * shape->record_size;
}

size_t spec_gb_list_size(const spec_gb_list_shape_t *shape) {
    return spec_gb_list_nickname_offset(shape, shape->capacity);
}

size_t spec_gb_list_species_offset(const spec_gb_list_shape_t *shape, size_t index) {
    (void)shape;
    return COUNT_SIZE + index;
}

size_t spec_gb_list_trainer_name_offset(const spec_gb_list_shape_t *shape, size_t index) {
    return spec_gb_list_record_offset(shape, shape->capacity) + index * shape->name_size;
}
