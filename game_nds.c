//Generation 4
//The regular DS games

//Diamond/Pearl/Platinum
//Heart Gold/Soul Silver

#include <stdlib.h>
#include <string.h>
#include "game_nds.h"

///////////////////////////////////////////////////
// CHECKSUM STUFF

enum {
	NDS_DP_SMALL_BLOCK_START = 0x0,
	NDS_PLAT_SMALL_BLOCK_START = 0x0,
	NDS_HGSS_SMALL_BLOCK_START = 0x0,

	NDS_DP_SMALL_BLOCK_LENGTH = 0xC100,
	NDS_PLAT_SMALL_BLOCK_LENGTH = 0xCF2C,
	NDS_HGSS_SMALL_BLOCK_LENGTH = 0xF628,

	NDS_DP_SMALL_BLOCK_FOOTER = 0xC0EC,
	NDS_PLAT_SMALL_BLOCK_FOOTER = 0xCF18,
	NDS_HGSS_SMALL_BLOCK_FOOTER = 0xF618,

	NDS_DP_BIG_BLOCK_START = 0xC100,
	NDS_PLAT_BIG_BLOCK_START = 0xCF2C,
	NDS_HGSS_BIG_BLOCK_START = 0xF700,

	NDS_DP_BIG_BLOCK_LENGTH = 0x121E0,
	NDS_PLAT_BIG_BLOCK_LENGTH = 0x121E4,
	NDS_HGSS_BIG_BLOCK_LENGTH = 0x12310,

	NDS_DP_BIG_BLOCK_FOOTER = 0x1E2CC,
	NDS_PLAT_BIG_BLOCK_FOOTER = 0x1F0FC,
	NDS_HGSS_BIG_BLOCK_FOOTER = 0x21A00,

	NDS_FOOTER_LENGTH = 0x14,
	NDS_HGSS_FOOTER_LENGTH = 0x10,

	NDS_TYPE_DETECT_DP = NDS_DP_SMALL_BLOCK_FOOTER + 0x8,
	NDS_TYPE_DETECT_PLAT = NDS_PLAT_SMALL_BLOCK_FOOTER + 0x8,
	NDS_TYPE_DETECT_HGSS = NDS_HGSS_SMALL_BLOCK_FOOTER + 0x4,

	NDS_ONESAVE_LENGTH = 0x40000
};

#pragma pack(push, 1)
typedef struct {
	uint32_t storage_id;
	uint32_t general_id;
	uint32_t block_size;
	uint32_t runtime0;
	uint16_t runtime1;
	uint16_t checksum;
} nds_footer_dppt_t;

typedef struct {
	uint32_t save_index;
	uint32_t block_size;
	uint32_t runtime0;
	uint16_t runtime1;
	uint16_t checksum;
} nds_footer_hgss_t;

typedef union {
	nds_footer_dppt_t dppt;
	nds_footer_hgss_t hgss;
} nds_footer_t;
#pragma pack(pop)

typedef struct {
	const uint8_t *small;
	const uint8_t *big;
	const nds_footer_t *small_footer;
	const nds_footer_t *big_footer;
} nds_block_ptr_t;

typedef struct {
	nds_block_ptr_t block[2];
	nds_savetype_t type;
	size_t small;
	size_t big;
	size_t big_start;
} nds_bdat_t; //block data

typedef struct {
	uint8_t *small;
	uint8_t *big;
	nds_footer_t *small_footer;
	nds_footer_t *big_footer;
	size_t small_len;
	size_t big_len;
	size_t big_start;
} nds_sdat_t; //save data

typedef struct {
	uint16_t small;
	uint16_t big;
} nds_save_checksum_t;

typedef struct {
	uint8_t small;
	uint8_t big;
} nds_save_index_t;

nds_savetype_t nds_detect_save_type(const uint8_t *ptr) {
	if(*(uint32_t *)(ptr + NDS_TYPE_DETECT_DP) == NDS_DP_SMALL_BLOCK_LENGTH) {
		return NDS_TYPE_DP;
	}
	if(*(uint32_t *)(ptr + NDS_TYPE_DETECT_PLAT) == NDS_PLAT_SMALL_BLOCK_LENGTH) {
		return NDS_TYPE_PLAT;
	}
	if(*(uint32_t *)(ptr + NDS_TYPE_DETECT_HGSS) == NDS_HGSS_SMALL_BLOCK_LENGTH) {
		return NDS_TYPE_HGSS;
	}
	return NDS_TYPE_UNKNOWN;
}

nds_bdat_t nds_get_bdat(const uint8_t *ptr) {
	nds_bdat_t bdat;
	bdat.type = nds_detect_save_type(ptr);
	bdat.block[0].small = ptr;
	bdat.block[1].small = ptr + NDS_ONESAVE_LENGTH;
	if(bdat.type == NDS_TYPE_DP) {
		bdat.small = NDS_DP_SMALL_BLOCK_LENGTH;
		bdat.big = NDS_DP_BIG_BLOCK_LENGTH;
		bdat.big_start = NDS_DP_BIG_BLOCK_START;
		bdat.block[0].small_footer = (const nds_footer_t *)(ptr + NDS_DP_SMALL_BLOCK_FOOTER);
		bdat.block[0].big_footer = (const nds_footer_t *)(ptr + NDS_DP_BIG_BLOCK_FOOTER);
	} else if(bdat.type == NDS_TYPE_PLAT) {
		bdat.small = NDS_PLAT_SMALL_BLOCK_LENGTH;
		bdat.big = NDS_PLAT_BIG_BLOCK_LENGTH;
		bdat.big_start = NDS_PLAT_BIG_BLOCK_START;
		bdat.block[0].small_footer = (const nds_footer_t *)(ptr + NDS_PLAT_SMALL_BLOCK_FOOTER);
		bdat.block[0].big_footer = (const nds_footer_t *)(ptr + NDS_PLAT_BIG_BLOCK_FOOTER);
	} else if(bdat.type == NDS_TYPE_HGSS) {
		bdat.small = NDS_HGSS_SMALL_BLOCK_LENGTH;
		bdat.big = NDS_HGSS_BIG_BLOCK_LENGTH;
		bdat.big_start = NDS_HGSS_BIG_BLOCK_START;
		bdat.block[0].small_footer = (const nds_footer_t *)(ptr + NDS_HGSS_SMALL_BLOCK_FOOTER);
		bdat.block[0].big_footer = (const nds_footer_t *)(ptr + NDS_HGSS_BIG_BLOCK_FOOTER);
	}
	bdat.block[0].big = ptr + bdat.big;
	bdat.block[1].big = bdat.block[1].small + bdat.big;
	bdat.block[1].small_footer = bdat.block[0].small_footer + NDS_ONESAVE_LENGTH;
	bdat.block[1].big_footer = bdat.block[0].big_footer + NDS_ONESAVE_LENGTH;
	return bdat;
}

nds_sdat_t *nds_get_sdat(const nds_save_t *save, const nds_bdat_t bdat) {
	nds_sdat_t *sdat = malloc(sizeof(nds_sdat_t));
	sdat->big_len = bdat.big;
	sdat->small_len = bdat.small;
	sdat->big_start = bdat.big_start;
	sdat->small = save->data;
	sdat->big = save->data + bdat.big_start;
	if(bdat.type == NDS_TYPE_DP) {
		sdat->small_footer = (nds_footer_t *)save->data + NDS_DP_SMALL_BLOCK_FOOTER;
		sdat->big_footer = (nds_footer_t *)save->data + NDS_DP_BIG_BLOCK_FOOTER;
	} else if(bdat.type == NDS_TYPE_PLAT) {
		sdat->small_footer = (nds_footer_t *)save->data + NDS_PLAT_SMALL_BLOCK_FOOTER;
		sdat->big_footer = (nds_footer_t *)save->data + NDS_PLAT_BIG_BLOCK_FOOTER;
	} else if(bdat.type == NDS_TYPE_HGSS) {
		sdat->small_footer = (nds_footer_t *)save->data + NDS_HGSS_SMALL_BLOCK_FOOTER;
		sdat->big_footer = (nds_footer_t *)save->data + NDS_HGSS_BIG_BLOCK_FOOTER;
	}
	return sdat;
}

nds_save_index_t nds_get_main_index(nds_bdat_t bdat) {
	nds_save_index_t index;
	if(bdat.type != NDS_TYPE_HGSS) {
		//dp/plat
		index.small = 0;
		if(bdat.block[0].small_footer->dppt.general_id < bdat.block[1].small_footer->dppt.general_id) {
			index.small = 1;
		}
		index.big = 0;
		if(bdat.block[0].big_footer->dppt.storage_id == bdat.block[1].big_footer->dppt.storage_id) {
			//check general block of big footers
			if(bdat.block[0].big_footer->dppt.general_id < bdat.block[1].big_footer->dppt.general_id) {
				index.big = 1;
			}
		} else if(bdat.block[0].big_footer->dppt.storage_id < bdat.block[1].big_footer->dppt.storage_id) {
			index.big = 1;
		}
	} else {
		//TODO hgss
	}
	return index;
}


nds_save_t *nds_read_save_internal(nds_bdat_t bdat, nds_save_index_t index) {
	nds_save_t *save = malloc(sizeof(nds_save_t));
	save->type = bdat.type;
	save->data = malloc(NDS_ONESAVE_LENGTH);
	memcpy(save->data, bdat.block[index.small].small, bdat.small);
	memcpy(save->data + bdat.big_start, bdat.block[index.big].big, bdat.big);
	save->internal = nds_get_sdat(save, bdat);
	return save;
}

nds_save_t *nds_read_main_save(const uint8_t *ptr) {
	nds_bdat_t bdat = nds_get_bdat(ptr);
	nds_save_index_t index = nds_get_main_index(bdat);
	return nds_read_save_internal(bdat, index);
}

nds_save_t *nds_read_backup_save(const uint8_t *ptr) {
	nds_bdat_t bdat = nds_get_bdat(ptr);
	nds_save_index_t index = nds_get_main_index(bdat);
	index.small ^= 1;
	index.big ^= 1;
	return nds_read_save_internal(bdat, index);
}

void nds_free_save(nds_save_t *save) {
	free(save->internal);
	free(save->data);
	free(save);
	save->internal = NULL;
	save->data = NULL;
	save = NULL;
}

/**
 * Creates a data block you should load your file into.
 * @return pointer to the data block
 */
uint8_t *nds_create_data() {
	uint8_t *data = malloc(NDS_SAVE_SIZE);
	memset(data,0xFF,NDS_SAVE_SIZE);
	return data;
}

void nds_write_main_save(uint8_t *ptr, const nds_save_t *sav) {
}

void nds_write_backup_save(uint8_t *ptr, const nds_save_t *sav) {
}

///////////////////////////////////////////////////
// SAVE STUFF

enum {
	NDS_PARTY_START_DP = 0x98,
	NDS_PARTY_START_PLAT = 0xA0,
	NDS_PARTY_START_HGSS = 0xA0,

	NDS_SIGNATURE_START_DP = 0x5904,
	NDS_SIGNATURE_START_PLAT = 0x5BA8,
	NDS_SIGNATURE_START_HGSS = 0x4538,

	NDS_SIGNATURE_SIZE = 0x600
};

