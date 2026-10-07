// The Gen 3 flash: two slots of fourteen checksummed sectors, and slot-relative access.

#include <string.h>

#include "gba/gba_internal.h"
#include "spec_internal.h"

constexpr size_t SECTOR_SIZE = 0x1000; // pret/pokeemerald SECTOR_SIZE
constexpr size_t SLOT_COUNT = 2;       // pret/pokeemerald NUM_SAVE_SLOTS
constexpr uint16_t ALL_SECTIONS = (1 << SPEC_GBA_SECTION_COUNT) - 1;

constexpr size_t SECTION_ID_OFFSET = 0xFF4;       // pret/pokeemerald SaveSector id
constexpr size_t CHECKSUM_OFFSET = 0xFF6;         // pret/pokeemerald SaveSector checksum
constexpr size_t SIGNATURE_OFFSET = 0xFF8;        // pret/pokeemerald SaveSector signature
constexpr size_t COUNTER_OFFSET = 0xFFC;          // pret/pokeemerald SaveSector counter
constexpr uint32_t SECTOR_SIGNATURE = 0x08012025; // pret/pokeemerald SECTOR_SIGNATURE

struct piece {
    size_t data_offset;
    size_t size;
};
typedef struct piece piece_t;

static size_t sector_offset_of(const spec_gba_save_slot_t *slot, size_t section_id) {
    return (slot->first_sector + slot->position_of_section[section_id]) * SECTOR_SIZE;
}

static uint16_t sector_checksum_of(const uint8_t *sector, size_t section_size) {
    uint32_t sum = 0;
    for (size_t offset = 0; offset < section_size; offset += 4) {
        sum += spec_read_u32_le(&sector[offset]);
    }
    return (uint16_t)((sum >> 16) + sum);
}

static piece_t piece_at(const spec_gba_save_slot_t *slot, size_t offset, size_t size_left) {
    size_t section_id = offset / SPEC_GBA_SECTION_DATA_SIZE;
    size_t offset_in_section = offset % SPEC_GBA_SECTION_DATA_SIZE;
    size_t room_in_section = SPEC_GBA_SECTION_DATA_SIZE - offset_in_section;
    return (piece_t){
        .data_offset = sector_offset_of(slot, section_id) + offset_in_section,
        .size = size_left < room_in_section ? size_left : room_in_section,
    };
}

void spec_gba_read_slot_bytes(uint8_t *bytes, const uint8_t *data, const spec_gba_save_slot_t *slot,
                              size_t offset, size_t size) {
    for (size_t copied = 0; copied < size;) {
        piece_t piece = piece_at(slot, offset + copied, size - copied);
        memcpy(&bytes[copied], &data[piece.data_offset], piece.size);
        copied += piece.size;
    }
}

bool spec_gba_read_slot_flag(const uint8_t *data, const spec_gba_save_slot_t *slot,
                             size_t flags_offset, uint16_t flag) {
    uint8_t flag_byte = spec_gba_read_slot_u8(data, slot, flags_offset + flag / 8);
    return spec_get_bits(flag_byte, flag % 8, 1) != 0;
}

uint16_t spec_gba_read_slot_u16(const uint8_t *data, const spec_gba_save_slot_t *slot,
                                size_t offset) {
    uint8_t bytes[2];
    spec_gba_read_slot_bytes(bytes, data, slot, offset, sizeof bytes);
    return spec_read_u16_le(bytes);
}

uint32_t spec_gba_read_slot_u32(const uint8_t *data, const spec_gba_save_slot_t *slot,
                                size_t offset) {
    uint8_t bytes[4];
    spec_gba_read_slot_bytes(bytes, data, slot, offset, sizeof bytes);
    return spec_read_u32_le(bytes);
}

uint8_t spec_gba_read_slot_u8(const uint8_t *data, const spec_gba_save_slot_t *slot,
                              size_t offset) {
    uint8_t value = 0;
    spec_gba_read_slot_bytes(&value, data, slot, offset, 1);
    return value;
}

void spec_gba_write_slot_bytes(uint8_t *data, const spec_gba_save_slot_t *slot, size_t offset,
                               const uint8_t *bytes, size_t size) {
    for (size_t copied = 0; copied < size;) {
        piece_t piece = piece_at(slot, offset + copied, size - copied);
        memcpy(&data[piece.data_offset], &bytes[copied], piece.size);
        copied += piece.size;
    }
}

void spec_gba_write_slot_flag(uint8_t *data, const spec_gba_save_slot_t *slot, size_t flags_offset,
                              uint16_t flag, bool is_set) {
    size_t flag_byte_offset = flags_offset + flag / 8;
    uint8_t flag_byte = spec_gba_read_slot_u8(data, slot, flag_byte_offset);
    spec_gba_write_slot_u8(data, slot, flag_byte_offset,
                           (uint8_t)spec_set_bits(flag_byte, flag % 8, 1, is_set));
}

void spec_gba_write_slot_u16(uint8_t *data, const spec_gba_save_slot_t *slot, size_t offset,
                             uint16_t value) {
    uint8_t bytes[2];
    spec_write_u16_le(bytes, value);
    spec_gba_write_slot_bytes(data, slot, offset, bytes, sizeof bytes);
}

void spec_gba_write_slot_u32(uint8_t *data, const spec_gba_save_slot_t *slot, size_t offset,
                             uint32_t value) {
    uint8_t bytes[4];
    spec_write_u32_le(bytes, value);
    spec_gba_write_slot_bytes(data, slot, offset, bytes, sizeof bytes);
}

void spec_gba_write_slot_u8(uint8_t *data, const spec_gba_save_slot_t *slot, size_t offset,
                            uint8_t value) {
    spec_gba_write_slot_bytes(data, slot, offset, &value, 1);
}

static bool is_sector_valid(const uint8_t *sector, const uint16_t *section_sizes) {
    uint16_t section_id = spec_read_u16_le(&sector[SECTION_ID_OFFSET]);
    bool is_signed = spec_read_u32_le(&sector[SIGNATURE_OFFSET]) == SECTOR_SIGNATURE;
    if (!is_signed || section_id >= SPEC_GBA_SECTION_COUNT) {
        return false;
    }
    uint16_t checksum = sector_checksum_of(sector, section_sizes[section_id]);
    return spec_read_u16_le(&sector[CHECKSUM_OFFSET]) == checksum;
}

// True when all fourteen sections are present and valid.
static bool read_slot(spec_gba_save_slot_t *slot, const uint8_t *data, size_t slot_index,
                      const uint16_t *section_sizes) {
    uint16_t found_sections = 0;
    slot->first_sector = slot_index * SPEC_GBA_SECTION_COUNT;
    for (size_t position = 0; position < SPEC_GBA_SECTION_COUNT; ++position) {
        const uint8_t *sector = &data[(slot->first_sector + position) * SECTOR_SIZE];
        if (!is_sector_valid(sector, section_sizes)) {
            continue;
        }
        uint16_t section_id = spec_read_u16_le(&sector[SECTION_ID_OFFSET]);
        found_sections |= (uint16_t)(1 << section_id);
        slot->position_of_section[section_id] = (uint8_t)position;
        slot->counter = spec_read_u32_le(&sector[COUNTER_OFFSET]);
    }
    return found_sections == ALL_SECTIONS;
}

static uint32_t newer_counter(uint32_t first_counter, uint32_t second_counter) {
    bool has_wrapped = (first_counter == UINT32_MAX && second_counter == 0)
                       || (first_counter == 0 && second_counter == UINT32_MAX);
    if (has_wrapped) {
        return 0;
    }
    return first_counter > second_counter ? first_counter : second_counter;
}

// As GetSaveValidStatus: the newer counter wins, and slot counter % 2 is loaded.
bool spec_gba_find_active_slot(spec_gba_save_slot_t *active, const uint8_t *data,
                               const uint16_t section_sizes[static SPEC_GBA_SECTION_COUNT]) {
    spec_gba_save_slot_t slots[SLOT_COUNT] = {};
    bool is_whole[SLOT_COUNT] = {};
    for (size_t slot_index = 0; slot_index < SLOT_COUNT; ++slot_index) {
        is_whole[slot_index] = read_slot(&slots[slot_index], data, slot_index, section_sizes);
    }
    uint32_t counter = 0;
    if (is_whole[0] && is_whole[1]) {
        counter = newer_counter(slots[0].counter, slots[1].counter);
    } else if (is_whole[0]) {
        counter = slots[0].counter;
    } else if (is_whole[1]) {
        counter = slots[1].counter;
    } else {
        return false;
    }
    size_t active_index = counter % SLOT_COUNT;
    if (!is_whole[active_index]) {
        return false;
    }
    *active = slots[active_index];
    active->counter = counter;
    return true;
}

// As the game saves: the other slot, rotated one sector, counter + 1.
spec_gba_save_slot_t spec_gba_copy_to_next_slot(uint8_t *data, const spec_gba_save_slot_t *active) {
    spec_gba_save_slot_t next = {
        .first_sector = ((active->counter + 1) % SLOT_COUNT) * SPEC_GBA_SECTION_COUNT,
        .counter = active->counter + 1,
    };
    size_t rotation = active->position_of_section[0];
    for (size_t section_id = 0; section_id < SPEC_GBA_SECTION_COUNT; ++section_id) {
        next.position_of_section[section_id] =
            (uint8_t)((section_id + rotation + 1) % SPEC_GBA_SECTION_COUNT);
        memcpy(&data[sector_offset_of(&next, section_id)],
               &data[sector_offset_of(active, section_id)], SECTOR_SIZE);
    }
    return next;
}

void spec_gba_stamp_slot(uint8_t *data, const spec_gba_save_slot_t *slot,
                         const uint16_t section_sizes[static SPEC_GBA_SECTION_COUNT]) {
    for (size_t section_id = 0; section_id < SPEC_GBA_SECTION_COUNT; ++section_id) {
        uint8_t *sector = &data[sector_offset_of(slot, section_id)];
        spec_write_u32_le(&sector[COUNTER_OFFSET], slot->counter);
        spec_write_u16_le(&sector[CHECKSUM_OFFSET],
                          sector_checksum_of(sector, section_sizes[section_id]));
    }
}
