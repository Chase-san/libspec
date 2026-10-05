// Item pockets as every console's bag stores them: empty slots, counts and condensing.

#include <string.h>

#include "spec.h"
#include "spec_internal.h"

size_t spec_count_filled_slots(const spec_item_slot_t *item_slots, size_t slot_count) {
    size_t filled_slot_count = 0;
    for (size_t index = 0; index < slot_count; ++index) {
        if (!spec_is_item_slot_empty(&item_slots[index])) {
            ++filled_slot_count;
        }
    }
    return filled_slot_count;
}

// The games clear the item when its quantity reaches zero.
bool spec_is_item_slot_empty(const spec_item_slot_t *item_slot) {
    return item_slot->item == 0 || item_slot->quantity == 0;
}

// As the games condense a pocket: the filled slots keep their order, and empty ones follow.
void spec_condense_pocket(spec_item_slot_t *condensed, const spec_item_slot_t *item_slots,
                          size_t slot_count) {
    memset(condensed, 0, slot_count * sizeof *condensed);
    size_t filled_slot_count = 0;
    for (size_t index = 0; index < slot_count; ++index) {
        if (!spec_is_item_slot_empty(&item_slots[index])) {
            condensed[filled_slot_count++] = item_slots[index];
        }
    }
}
