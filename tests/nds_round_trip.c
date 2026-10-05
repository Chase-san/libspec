// Writes a Gen 4 save back unedited and checks it reads the same, then checks refused writes.

#include <stdio.h>
#include <string.h>

#include "nds/nds.h"

static uint8_t original[SPEC_NDS_SAVE_SIZE];
static uint8_t written[SPEC_NDS_SAVE_SIZE];

static bool read_file(uint8_t *data, const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == nullptr) {
        return false;
    }
    size_t size = fread(data, 1, SPEC_NDS_SAVE_SIZE, file);
    fclose(file);
    return size == SPEC_NDS_SAVE_SIZE;
}

static bool is_same_pokemon(const spec_nds_pokemon_t *first, const spec_nds_pokemon_t *second,
                            size_t record_size) {
    uint8_t first_record[SPEC_NDS_PARTY_RECORD_SIZE];
    uint8_t second_record[SPEC_NDS_PARTY_RECORD_SIZE];
    return spec_nds_write_pokemon(first_record, record_size, first) == SPEC_OK
           && spec_nds_write_pokemon(second_record, record_size, second) == SPEC_OK
           && memcmp(first_record, second_record, record_size) == 0;
}

static bool is_same_trainer(const spec_nds_trainer_t *first, const spec_nds_trainer_t *second) {
    return memcmp(first->name, second->name, sizeof first->name) == 0 && first->id == second->id
           && first->secret_id == second->secret_id && first->is_female == second->is_female;
}

static bool is_same_player(const spec_nds_save_t *first, const spec_nds_save_t *second) {
    return first->type == second->type && first->language == second->language
           && is_same_trainer(&first->trainer, &second->trainer)
           && first->play_time.hours == second->play_time.hours
           && first->play_time.minutes == second->play_time.minutes
           && first->play_time.seconds == second->play_time.seconds && first->money == second->money
           && first->coins == second->coins && first->battle_points == second->battle_points
           && memcmp(first->badges, second->badges, sizeof first->badges) == 0
           && memcmp(first->rival_name, second->rival_name, sizeof first->rival_name) == 0
           && first->options.button_mode == second->options.button_mode
           && first->options.text_speed == second->options.text_speed
           && first->options.window_frame == second->options.window_frame
           && first->options.is_stereo == second->options.is_stereo
           && first->options.is_battle_style_set == second->options.is_battle_style_set
           && first->options.is_battle_scene_off == second->options.is_battle_scene_off;
}

static bool is_same_pokedex(const spec_nds_pokedex_t *first, const spec_nds_pokedex_t *second) {
    return first->is_obtained == second->is_obtained
           && first->has_national_dex == second->has_national_dex
           && first->can_view_forms == second->can_view_forms
           && first->can_view_languages == second->can_view_languages
           && first->spinda_personality == second->spinda_personality
           && memcmp(first->is_seen, second->is_seen, sizeof first->is_seen) == 0
           && memcmp(first->is_caught, second->is_caught, sizeof first->is_caught) == 0
           && memcmp(first->first_seen_gender, second->first_seen_gender,
                     sizeof first->first_seen_gender)
                  == 0
           && memcmp(first->has_seen_both_genders, second->has_seen_both_genders,
                     sizeof first->has_seen_both_genders)
                  == 0
           && memcmp(first->languages, second->languages, sizeof first->languages) == 0
           && memcmp(&first->forms, &second->forms, sizeof first->forms) == 0;
}

static bool is_same_storage(const spec_nds_save_t *first, const spec_nds_save_t *second) {
    if (first->party_count != second->party_count || first->current_box != second->current_box) {
        return false;
    }
    for (size_t index = 0; index < SPEC_NDS_PARTY_CAPACITY; ++index) {
        if (!is_same_pokemon(&first->party[index], &second->party[index],
                             SPEC_NDS_PARTY_RECORD_SIZE)) {
            return false;
        }
    }
    for (size_t box = 0; box < SPEC_NDS_BOX_COUNT; ++box) {
        const spec_nds_box_t *first_box = &first->boxes[box];
        const spec_nds_box_t *second_box = &second->boxes[box];
        if (memcmp(first_box->name, second_box->name, sizeof first_box->name) != 0
            || first_box->wallpaper != second_box->wallpaper) {
            return false;
        }
        for (size_t index = 0; index < SPEC_NDS_BOX_CAPACITY; ++index) {
            if (!is_same_pokemon(&first_box->pokemon[index], &second_box->pokemon[index],
                                 SPEC_NDS_BOX_RECORD_SIZE)) {
                return false;
            }
        }
    }
    for (size_t index = 0; index < SPEC_NDS_DAYCARE_CAPACITY; ++index) {
        const spec_nds_daycare_slot_t *first_slot = &first->daycare.slots[index];
        const spec_nds_daycare_slot_t *second_slot = &second->daycare.slots[index];
        if (!is_same_pokemon(&first_slot->pokemon, &second_slot->pokemon, SPEC_NDS_BOX_RECORD_SIZE)
            || first_slot->steps != second_slot->steps) {
            return false;
        }
    }
    return first->daycare.egg_personality == second->daycare.egg_personality
           && first->daycare.step_counter == second->daycare.step_counter;
}

static bool is_slot_empty(const spec_nds_item_slot_t *item_slot) {
    return item_slot->item == 0 || item_slot->quantity == 0;
}

// Writing condenses a pocket, so only the filled slots and their order count.
static bool is_same_pocket(const spec_nds_item_slot_t *first, const spec_nds_item_slot_t *second) {
    size_t first_index = 0;
    size_t second_index = 0;
    while (true) {
        while (first_index < SPEC_NDS_POCKET_MAX_CAPACITY && is_slot_empty(&first[first_index])) {
            ++first_index;
        }
        while (second_index < SPEC_NDS_POCKET_MAX_CAPACITY
               && is_slot_empty(&second[second_index])) {
            ++second_index;
        }
        if (first_index == SPEC_NDS_POCKET_MAX_CAPACITY
            || second_index == SPEC_NDS_POCKET_MAX_CAPACITY) {
            return first_index == second_index;
        }
        if (first[first_index].item != second[second_index].item
            || first[first_index].quantity != second[second_index].quantity) {
            return false;
        }
        ++first_index;
        ++second_index;
    }
}

static bool is_same_bag(const spec_nds_save_t *first, const spec_nds_save_t *second) {
    for (size_t pocket = 0; pocket < SPEC_NDS_POCKET_COUNT; ++pocket) {
        if (!is_same_pocket(first->items[pocket], second->items[pocket])) {
            return false;
        }
    }
    return true;
}

static size_t count_differences(const uint8_t *first, const uint8_t *second, size_t size) {
    size_t difference_count = 0;
    for (size_t index = 0; index < size; ++index) {
        difference_count += first[index] != second[index];
    }
    return difference_count;
}

// An unedited write reads back as it was read, field for field.
static bool check_unedited_write(const spec_nds_save_t *save) {
    memcpy(written, original, sizeof written);
    if (spec_nds_write_save(save, written) != SPEC_OK) {
        printf("write_save: %s\n", spec_last_error().message);
        return false;
    }
    static spec_nds_save_t reread;
    if (spec_nds_read_save(&reread, written) != SPEC_OK) {
        printf("reread: %s\n", spec_last_error().message);
        return false;
    }
    bool is_player_kept = is_same_player(save, &reread);
    bool is_pokedex_kept = is_same_pokedex(&save->pokedex, &reread.pokedex);
    bool is_storage_kept = is_same_storage(save, &reread);
    bool are_items_kept = is_same_bag(save, &reread);
    if (!is_player_kept || !is_pokedex_kept || !is_storage_kept || !are_items_kept) {
        printf("player kept %d, pokedex kept %d, storage kept %d, items kept %d\n", is_player_kept,
               is_pokedex_kept, is_storage_kept, are_items_kept);
        return false;
    }
    printf("%zu bytes written\n", count_differences(original, written, sizeof original));
    return true;
}

static bool is_failed_write_reported(const spec_nds_save_t *broken, spec_error_t expected_error,
                                     spec_error_location_t location, uint32_t index0,
                                     uint32_t index1) {
    memcpy(written, original, sizeof written);
    spec_error_t error = spec_nds_write_save(broken, written);
    spec_error_data_t detail = spec_last_error();
    bool is_reported = error == expected_error && detail.error == error
                       && detail.location == location && detail.index0 == index0
                       && detail.index1 == index1;
    bool is_unchanged = memcmp(original, written, sizeof written) == 0;
    if (!is_reported || !is_unchanged) {
        printf("failed write: %s, reported %d, unchanged %d\n", detail.message, is_reported,
               is_unchanged);
        return false;
    }
    return true;
}

static bool check_failed_writes(const spec_nds_save_t *save) {
    static spec_nds_save_t broken;
    broken = *save;
    broken.boxes[3].pokemon[12].origin.met_level = 200;
    if (!is_failed_write_reported(&broken, SPEC_ERROR_VALUE_OUT_OF_RANGE, SPEC_ERROR_LOCATION_BOX,
                                  3, 12)) {
        return false;
    }
    broken = *save;
    broken.items[SPEC_NDS_POCKET_KEY_ITEMS][0] = (spec_nds_item_slot_t){.item = 4, .quantity = 1};
    if (!is_failed_write_reported(&broken, SPEC_ERROR_WRONG_POCKET, SPEC_ERROR_LOCATION_ITEMS,
                                  SPEC_NDS_POCKET_KEY_ITEMS, 0)) {
        return false;
    }
    broken = *save;
    broken.pokedex.forms.shellos = (spec_nds_form_order_t){.count = 1, .forms = {2}};
    return is_failed_write_reported(&broken, SPEC_ERROR_VALUE_OUT_OF_RANGE,
                                    SPEC_ERROR_LOCATION_POKEDEX, 422, 0);
}

int main(int argument_count, char **arguments) {
    if (argument_count != 2 || !read_file(original, arguments[1])) {
        printf("usage: nds_round_trip SAVE (at least %zu bytes)\n", SPEC_NDS_SAVE_SIZE);
        return 1;
    }
    static spec_nds_save_t save;
    if (spec_nds_read_save(&save, original) != SPEC_OK) {
        printf("read_save: %s\n", spec_last_error().message);
        return 1;
    }
    if (!check_unedited_write(&save) || !check_failed_writes(&save)) {
        return 1;
    }
    printf("type %d, language %d, party %d: ok\n", save.type, save.language, save.party_count);
    return 0;
}
