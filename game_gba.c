//Generation 3
//The GBA Games

//Ruby/Sapphire/Emerald
//Fire Red/Leaf Green

#include <string.h>
#include "types.h"
#include "game_gba.h"
#include "checksum.h"

//http://bulbapedia.bulbagarden.net/wiki/Save_data_structure_in_Generation_III

static const uint16_t gba_to_codepage[] = {
	0x0020, 0x3042, 0x3044, 0x3046, 0x3048, 0x304a, 0x304b, 0x304d,
	0x304f, 0x3051, 0x3053, 0x3055, 0x3057, 0x3059, 0x305b, 0x305d,
	0x305f, 0x3061, 0x3064, 0x3066, 0x3068, 0x306a, 0x306b, 0x306c,
	0x306d, 0x306e, 0x306f, 0x3072, 0x3075, 0x3078, 0x307b, 0x307e,
	0x307f, 0x3080, 0x3081, 0x3082, 0x3084, 0x3086, 0x3088, 0x3089,
	0x308a, 0x308b, 0x308c, 0x308d, 0x308f, 0x3092, 0x3093, 0x3041,
	0x3043, 0x3045, 0x3047, 0x3049, 0x3083, 0x3085, 0x3087, 0x304c,
	0x304e, 0x3050, 0x3052, 0x3054, 0x3056, 0x3058, 0x305a, 0x305c,
	0x305e, 0x3060, 0x3062, 0x3065, 0x3067, 0x3069, 0x3070, 0x3073,
	0x3076, 0x3079, 0x307c, 0x3071, 0x3074, 0x3077, 0x307a, 0x307d,
	0xffff, 0x30a2, 0x30a4, 0x30a6, 0x30a8, 0x30aa, 0x30ab, 0x30ad,
	0x30af, 0x30b1, 0x30b3, 0x30b5, 0x30b7, 0x30b9, 0x30bb, 0x30bd,
	0x30bf, 0x30c1, 0x30c4, 0x30c6, 0x30c8, 0x30ca, 0x30cb, 0x30cc,
	0x30cd, 0x30ce, 0x30cf, 0x30d2, 0x30d5, 0x30d8, 0x30db, 0x30de,
	0x30df, 0x30e0, 0x30e1, 0x30e2, 0x30e4, 0x30e6, 0x30e8, 0x30e9,
	0x30ea, 0x30eb, 0x30ec, 0x30ed, 0x30ef, 0x30f2, 0x30f3, 0x30a1,
	0x30a3, 0x30a5, 0x30a7, 0x30a9, 0x30e3, 0x30e5, 0x30e7, 0x30ac,
	0x30ae, 0x30b0, 0x30b2, 0x30b4, 0x30b6, 0x30b8, 0x30ba, 0x30bc,
	0x30be, 0x30c0, 0x30c2, 0x30c5, 0x30c7, 0x30c9, 0x30d0, 0x30d3,
	0x30d6, 0x30d9, 0x30dc, 0x30d1, 0x30d4, 0x30d7, 0x30da, 0x30dd,
	0xffff, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036,
	0x0037, 0x0038, 0x0039, 0x0021, 0x003f, 0x002e, 0x002d, 0xffff,
	0x2026, 0x201c, 0x201d, 0x2018, 0x2019, 0x2642, 0x2640, 0xffff,
	0x002c, 0xffff, 0x002f, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045,
	0x0046, 0x0047, 0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d,
	0x004e, 0x004f, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055,
	0x0056, 0x0057, 0x0058, 0x0059, 0x005a, 0x0061, 0x0062, 0x0063,
	0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b,
	0x006c, 0x006d, 0x006e, 0x006f, 0x0070, 0x0071, 0x0072, 0x0073,
	0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007a, 0xffff,
	0xffff, 0x00c4, 0x00d6, 0x00dc, 0x00e4, 0x00f6, 0x00fc, 0xffff,
	0xffff, 0xffff, 0xffff, 0x2192, 0xffff, 0xffff, 0x000a, 0x0000
};

enum {
	GBA_SAVE_SECTION = 0xE000,
	GBA_SAVE_BLOCK_COUNT = 14,
	GBA_BLOCK_LENGTH = 0x1000,
	GBA_BLOCK_DATA_LENGTH = 0xF80,
	GBA_BLOCK_FOOTER_LENGTH = 0xC,
	GBA_BLOCK_FOOTER_MARK = 0x08012025,
	GBA_UNPACKED_LENGTH = GBA_BLOCK_DATA_LENGTH * GBA_SAVE_BLOCK_COUNT,
	GBA_CODEPAGE_SIZE = 0x100
};

void gba_text_to_ucs2(char16_t *dst, char8_t *src, size_t size) {
	for(int i = 0; i < size; ++i) {
		dst[i] = gba_to_codepage[src[i]];
	}
}

void ucs2_to_gba_text(char8_t *dst, char16_t *src, size_t size) {
	for(int i = 0; i < size; ++i) {
		dst[i] = 0xAC; //question mark
		for(int j = 0; j < GBA_CODEPAGE_SIZE; ++j) {
			if(gba_to_codepage[j] == src[i]) {
				dst[i] = gba_to_codepage[j];
				break;
			}
		}
	}
}


gba_savetype_t gba_detect_save_type(uint8_t *ptr, size_t size) {
	return GBA_TYPE_UNKNOWN;
}

#pragma pack(push, 1)
//12 byte footer for every data block
typedef struct {
	uint16_t section_id;
	uint16_t checksum;
	uint32_t mark; // 25 20 01 08
	uint32_t save_index; //counts the number of times saved as well
} gba_footer_t;
#pragma pack(pop)

typedef struct {
	uint8_t order[GBA_SAVE_BLOCK_COUNT];
	uint32_t save_index;
} gba_internal_save_t;

static inline gba_footer_t *get_block_footer(const uint8_t *ptr) {
	return (gba_footer_t *)(ptr + GBA_BLOCK_LENGTH - GBA_BLOCK_FOOTER_LENGTH);
}

static inline uint16_t get_block_checksum(const uint8_t *ptr) {
	return gba_block_checksum(ptr, GBA_BLOCK_DATA_LENGTH);
}

size_t gba_get_save_offset(const uint8_t *ptr) {
	gba_footer_t *a = get_block_footer(ptr);
	gba_footer_t *b = get_block_footer(ptr + GBA_SAVE_SECTION);
	//TODO check that the mark is correct for the backup save, block 3
	//as otherwise we only have one save!
	if(a->save_index > b->save_index) {
		return 0;
	}
	return GBA_SAVE_SECTION; //second save
}

size_t gba_get_backup_offset(const uint8_t *ptr) {
	gba_footer_t *a = get_block_footer(ptr);
	gba_footer_t *b = get_block_footer(ptr + GBA_SAVE_SECTION);
	//TODO check that the mark is correct for the backup save, block 3
	//as otherwise we only have one save!
	if(a->save_index > b->save_index) {
		return GBA_SAVE_SECTION;
	}
	return 0;
}

/**
 * Unpacks the save at the pointer to a gba_save_t
 * @param ptr pointer to the data
 * @return the save
 */
gba_save_t *gba_read_save_internal(const uint8_t *ptr) {
	gba_save_t *save = malloc(sizeof(gba_save_t));
	gba_internal_save_t *internal = save->internal = malloc(sizeof(gba_internal_save_t));
	save->unpacked = malloc(GBA_UNPACKED_LENGTH);
	internal->save_index = get_block_footer(ptr)->save_index;
	memset(save->unpacked, 0, GBA_UNPACKED_LENGTH); //not sure if it is 0 or 0xFF
	for(size_t i = 0; i < GBA_SAVE_BLOCK_COUNT; ++i) {
		const uint8_t *block_ptr = ptr + i * GBA_BLOCK_LENGTH;
		//get footer
		gba_footer_t *footer = get_block_footer(block_ptr);
		internal->order[i] = footer->section_id;
		//get ptr to unpack too
		uint8_t *unpack_ptr = save->unpacked + footer->section_id * GBA_BLOCK_DATA_LENGTH;
		memcpy(unpack_ptr, block_ptr, GBA_BLOCK_DATA_LENGTH);
	}
	return save;
}

/**
 * Reads the main save from the given save pointer.
 * @param ptr the pointer to read from
 * @return the main save for this gba game
 */
gba_save_t *gba_read_main_save(const uint8_t *ptr) {
	return gba_read_save_internal(ptr + gba_get_save_offset(ptr));
}

/**
 * Reads the backup save from the given save pointer.
 * @param ptr the pointer to read from
 * @return the backup save for this gba game
 */
gba_save_t *gba_read_backup_save(const uint8_t *ptr) {
	return gba_read_save_internal(ptr + gba_get_backup_offset(ptr));
}

/**
 * Frees the save data made for the user.
 * @param save the save to free
 */
void gba_free_save(gba_save_t *save) {
	free(save->unpacked);
	free((gba_internal_save_t *)save->internal);
	free(save);
}

/**
 * Saves the save to the given pointer.
 * @param ptr place to save the data
 * @param save
 */
void gba_write_save_internal(uint8_t *ptr, const gba_save_t *save) {
	//wipe whatever is there now
	memset(ptr, 0, GBA_SAVE_SECTION);
	gba_internal_save_t *internal = save->internal;
	for(size_t i = 0; i < GBA_SAVE_BLOCK_COUNT; ++i) {
		//write data
		uint8_t *dest_ptr = ptr + i * GBA_BLOCK_LENGTH;
		uint8_t *src_ptr = save->unpacked + internal->order[i] * GBA_BLOCK_DATA_LENGTH;
		memcpy(dest_ptr, src_ptr, GBA_BLOCK_DATA_LENGTH);
		//write footer
		gba_footer_t *footer = get_block_footer(dest_ptr);
		footer->section_id = internal->order[i];
		footer->mark = GBA_BLOCK_FOOTER_MARK;
		footer->save_index = internal->save_index;
		//calculate checksum
		footer->checksum = get_block_checksum(dest_ptr);
	}
}

void gba_write_main_save(uint8_t *dst, const gba_save_t *save) {
	gba_write_save_internal(dst + gba_get_save_offset(dst), save);
}

void gba_write_backup_save(uint8_t *dst, const gba_save_t *save) {
	gba_write_save_internal(dst + gba_get_backup_offset(dst), save);
}

/**
 * Writes the save to the dst similar to how the game would do it.
 * @param dst data file thing to point to
 * @param save save to save to data
 */
void gba_save_game(uint8_t *dst, gba_save_t *save) {
	gba_internal_save_t *internal = save->internal;
	internal->save_index += 1;
	for(int i = 0; i < GBA_SAVE_BLOCK_COUNT; ++i) {
		internal->order[i] += 1;
		if(internal->order[i] >= GBA_SAVE_BLOCK_COUNT) {
			internal->order[i] = 0;
		}
	}
	gba_write_save_internal(dst + gba_get_backup_offset(dst), save);
}

/**
 * Creates a data block you should load your file into.
 * @return pointer to the data block
 */
uint8_t *gba_create_data() {
	uint8_t *data = malloc(GBA_SAVE_SIZE);
	memset(data, 0x0, GBA_SAVE_SIZE);
	return data;
}
