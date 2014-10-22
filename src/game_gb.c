//Generation 1&2
//The GB Games

//JPGreen (gen 0)
//Red/Blue/Yellow
//Gold/Silver/Crystal

//Unlike later games, the GB games have no encryption or checksums
//Which makes working with them relatively easy

#include "checksum.h"
#include "types.h"
#include "game_gb.h"
#include <stdint.h>
#include <string.h>

//TODO find a better table, we need both english and japanese for this
/**
 * The PK and MN symbols use lower case pi and mu respectively.
 */
static const uint16_t GB_TO_CODEPAGE[] = {
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0020,
	0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048,
	0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0050,
	0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058,
	0x0059, 0x005a, 0x0028, 0x0029, 0x003a, 0x003b, 0x005b, 0x005d,
	0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068,
	0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f, 0x0070,
	0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078,
	0x0079, 0x007a, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0x03c0, 0x03bc, 0x002d, 0xffff, 0xffff, 0x003f, 0x0021,
	0x002e, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	0xffff, 0x00d7, 0xffff, 0x002f, 0x002c, 0xffff, 0x0030, 0x0031,
	0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039,
};

enum {
	GB_RBY_PROTECTED_START = 0x2598,
	GB_RBY_PROTECTED_LENGTH = 0xf8b,
	GB_RBY_CHECKSUM = 0x3523,

	GB_GS_PROTECTED_START = 0x2009,
	GB_GS_PROTECTED_LENGTH = 0xd5f,
	GB_GS_CHECKSUM = 0x2d69,

	//gs secondary chunk is split into 5 sections
	GB_GS_PROTECTED2_0_START = 0x15c7,
	GB_GS_PROTECTED2_0_LENGTH = 0x226,
	GB_GS_PROTECTED2_1_START = 0x3d96,
	GB_GS_PROTECTED2_1_LENGTH = 0x1aa,
	GB_GS_PROTECTED2_2_START = 0x0c6B,
	GB_GS_PROTECTED2_2_LENGTH = 0x47d,
	GB_GS_PROTECTED2_3_START = 0x7e39,
	GB_GS_PROTECTED2_3_LENGTH = 0x34,
	GB_GS_PROTECTED2_4_START = 0x10e8,
	GB_GS_PROTECTED2_4_LENGTH = 0x4df,
	GB_GS_CHECKSUM2 = 0x7E6D,

	GB_C_PROTECTED_START = 0x2009,
	GB_C_PROTECTED_LENGTH = 0xb7a,
	GB_C_CHECKSUM = 0x2d0d,

	GB_C_PROTECTED2_START = 0x1209,
	GB_C_CHECKSUM2 = 0x1F0D,

	GB_CODEPAGE_SIZE = 0x100
};

void gb_text_to_ucs2(char16_t *dst, char8_t *src, size_t size) {
	for(int i = 0; i < size; ++i) {
		dst[i] = GB_TO_CODEPAGE[src[i]];
	}
}

void ucs2_to_gb_text(char8_t *dst, char16_t *src, size_t size) {
	uint8_t ended = 0;
	for(int i = 0; i < size; ++i) {
		//go through the table and find the matching symbol for src[i]
		if(ended) {
			dst[i] = 0x50;
		}
		dst[i] = 0xE6; //question mark by default
		for(int j = 0; j < GB_CODEPAGE_SIZE; ++j) {
			if(GB_TO_CODEPAGE[j] == src[i]) {
				dst[i] = GB_TO_CODEPAGE[j];
				if(!src[i]) {
					ended = 1;
				}
				break;
			}
		}
	}
}

//complex enough to need it's own function
uint16_t gb_gs_secondary_checksum(const uint8_t *ptr) {
	uint16_t sum = gb_gsc_checksum(ptr + GB_GS_PROTECTED2_0_START, GB_GS_PROTECTED2_0_LENGTH);
	sum += gb_gsc_checksum(ptr + GB_GS_PROTECTED2_1_START, GB_GS_PROTECTED2_1_LENGTH);
	sum += gb_gsc_checksum(ptr + GB_GS_PROTECTED2_2_START, GB_GS_PROTECTED2_2_LENGTH);
	sum += gb_gsc_checksum(ptr + GB_GS_PROTECTED2_3_START, GB_GS_PROTECTED2_3_LENGTH);
	sum += gb_gsc_checksum(ptr + GB_GS_PROTECTED2_4_START, GB_GS_PROTECTED2_4_LENGTH);
	return sum;
}

gb_savetype_t gb_detect_type(const uint8_t *ptr) {
	//we have to use checksums to detect type...
	if(gb_rby_checksum(ptr + GB_RBY_PROTECTED_START, GB_RBY_PROTECTED_LENGTH)
			== ptr[GB_RBY_CHECKSUM]) {
		//if it isn't we can't load the save anyway, right?
		return GB_TYPE_RBY;
	}
	if(gb_gsc_checksum(ptr + GB_GS_PROTECTED_START, GB_GS_PROTECTED_LENGTH)
			== *(uint16_t *)&ptr[GB_GS_CHECKSUM]) {
		return GB_TYPE_GS;
	}
	if(gb_gs_secondary_checksum(ptr) == *(uint16_t *)&ptr[GB_GS_CHECKSUM2]) {
		return GB_TYPE_GS;
	}
	if(gb_gsc_checksum(ptr + GB_C_PROTECTED_START, GB_C_PROTECTED_LENGTH)
			== *(uint16_t *)&ptr[GB_C_CHECKSUM]) {
		return GB_TYPE_C;
	}
	if(gb_gsc_checksum(ptr + GB_C_PROTECTED2_START, GB_C_PROTECTED_LENGTH)
			== *(uint16_t *)&ptr[GB_C_CHECKSUM2]) {
		return GB_TYPE_C;
	}
	return GB_TYPE_UNKNOWN;
}

gb_save_t *gb_read_save(const uint8_t *ptr) {
	gb_save_t *save = malloc(sizeof(gb_save_t));
	save->type = gb_detect_type(ptr);
	//TODO vary based on save type
	save->data = malloc(GB_SAVE_SIZE);
	memcpy(save->data, ptr, GB_SAVE_SIZE);
	return save;
}

void gb_free_save(gb_save_t *sav) {
	//SIGTRAP OCCURING HERE?
	free(sav->data);
	sav->data = NULL;
	free(sav);
}

uint8_t *gb_create_data() {
	return malloc(GB_SAVE_SIZE);
}

void gb_write_save(uint8_t *ptr, const gb_save_t *save) {
	memcpy(ptr, save->data, GB_SAVE_SIZE);
	if(save->type == GB_TYPE_RBY) {
		//calculate checksum, then write it to where it goes in ptr
		ptr[GB_RBY_CHECKSUM] = gb_rby_checksum(ptr + GB_RBY_PROTECTED_START, GB_RBY_PROTECTED_LENGTH);
	} else if(save->type == GB_TYPE_GS) {
		*((uint16_t *)&ptr[GB_GS_CHECKSUM]) = gb_gsc_checksum(ptr + GB_GS_PROTECTED_START, GB_GS_PROTECTED_LENGTH);
		*((uint16_t *)&ptr[GB_GS_CHECKSUM2]) = gb_gs_secondary_checksum(ptr);
	} else if(save->type == GB_TYPE_C) {
		*((uint16_t *)&ptr[GB_C_CHECKSUM]) = gb_gsc_checksum(ptr + GB_C_PROTECTED_START, GB_C_PROTECTED_LENGTH);
		*((uint16_t *)&ptr[GB_C_CHECKSUM2]) = gb_gsc_checksum(ptr + GB_C_PROTECTED2_START, GB_C_PROTECTED_LENGTH);
	}
}
