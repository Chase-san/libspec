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

	NDS_DP_SMALL_BLOCK_FOOTER = 0xC0EC,
	NDS_PLAT_SMALL_BLOCK_FOOTER = 0xCF18,
	NDS_HGSS_SMALL_BLOCK_FOOTER = 0xF618,

	NDS_DP_BIG_BLOCK_START = 0xC100,
	NDS_PLAT_BIG_BLOCK_START = 0xCF2C,
	NDS_HGSS_BIG_BLOCK_START = 0xF700,

	NDS_DP_BIG_BLOCK_FOOTER = 0x1E2CC,
	NDS_PLAT_BIG_BLOCK_FOOTER = 0x1F0FC,
	NDS_HGSS_BIG_BLOCK_FOOTER = 0x21A00,

	NDS_FOOTER_LENGTH = 0x14,
	NDS_HGSS_FOOTER_LENGTH = 0x10,

	NDS_TYPE_DETECT_DP = NDS_DP_SMALL_BLOCK_FOOTER + 0x8,
	NDS_TYPE_DETECT_PLAT = NDS_PLAT_SMALL_BLOCK_FOOTER + 0x8,
	NDS_TYPE_DETECT_HGSS = NDS_HGSS_SMALL_BLOCK_FOOTER + 0x4,
};

#pragma pack(push, 1)
typedef struct {
	uint32_t saveStorage;
	uint32_t saveGeneral;
	uint32_t blockSize;
	uint32_t unknown0;
	uint16_t unknown1;
	uint16_t checksum;
} nds_footer_dppt_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
	uint32_t saveNum;
	uint32_t blockSize;
	uint32_t unknown0;
	uint16_t unknown1;
	uint16_t checksum;
} nds_footer_hgss_t;
#pragma pack(pop)

static inline nds_footer_dppt_t *nds_get_dppt_footer(void *ptr, size_t footer_offset) {
	return ptr + footer_offset;
}
static inline nds_footer_hgss_t *nds_get_hgss_footer(void *ptr, size_t footer_offset) {
	return ptr + footer_offset;
}

void nds_get_save_checksum(void *ptr, nds_save_checksum_t *scs, nds_savetype_t type) {
	switch(type) {
		case NDS_TYPE_DP:
			scs->small = nds_get_dppt_footer(ptr, NDS_DP_SMALL_BLOCK_FOOTER)->checksum;
			scs->big = nds_get_dppt_footer(ptr, NDS_DP_BIG_BLOCK_FOOTER)->checksum;
			break;
		case NDS_TYPE_PLAT:
			scs->small = nds_get_dppt_footer(ptr, NDS_PLAT_SMALL_BLOCK_FOOTER)->checksum;
			scs->big = nds_get_dppt_footer(ptr, NDS_PLAT_BIG_BLOCK_FOOTER)->checksum;
			break;
		case NDS_TYPE_HGSS:
			scs->small = nds_get_hgss_footer(ptr, NDS_HGSS_SMALL_BLOCK_FOOTER)->checksum;
			scs->big = nds_get_hgss_footer(ptr, NDS_HGSS_BIG_BLOCK_FOOTER)->checksum;
			break;
		default:
			break;
	}
}

void nds_get_checksum(nds_save_t *save, nds_checksum_t *cs) {
	if(save->size == NDS_SAVE_SIZE_256 || save->size == NDS_SAVE_SIZE_512) {
		nds_get_save_checksum(save->data, &cs->save0, save->type);
	}
	if(save->size == NDS_SAVE_SIZE_512) {
		nds_get_save_checksum(save->data + NDS_SAVE_OFFSET, &cs->save1, save->type);
	}
}

void nds_set_save_checksum(void *ptr, nds_save_checksum_t *scs, nds_savetype_t type) {
	switch(type) {
		case NDS_TYPE_DP:
			nds_get_dppt_footer(ptr, NDS_DP_SMALL_BLOCK_FOOTER)->checksum = scs->small;
			nds_get_dppt_footer(ptr, NDS_DP_BIG_BLOCK_FOOTER)->checksum = scs->big;
			break;
		case NDS_TYPE_PLAT:
			nds_get_dppt_footer(ptr, NDS_PLAT_SMALL_BLOCK_FOOTER)->checksum = scs->small;
			nds_get_dppt_footer(ptr, NDS_PLAT_BIG_BLOCK_FOOTER)->checksum = scs->big;
			break;
		case NDS_TYPE_HGSS:
			nds_get_hgss_footer(ptr, NDS_HGSS_SMALL_BLOCK_FOOTER)->checksum = scs->small;
			nds_get_hgss_footer(ptr, NDS_HGSS_BIG_BLOCK_FOOTER)->checksum = scs->big;
			break;
		default:
			break;
	}
}

void nds_set_checksum(nds_save_t *save, nds_checksum_t *cs) {
	if(save->size == NDS_SAVE_SIZE_256 || save->size == NDS_SAVE_SIZE_512) {
		nds_set_save_checksum(save->data, &cs->save0, save->type);
	}
	if(save->size == NDS_SAVE_SIZE_512) {
		nds_set_save_checksum(save->data + NDS_SAVE_OFFSET, &cs->save1, save->type);
	}
}

void nds_calc_save_checksum(void *ptr, nds_save_checksum_t *scs, nds_savetype_t type) {
	switch(type) {
		case NDS_TYPE_DP:
			scs->small = nds_crc16(ptr + NDS_DP_SMALL_BLOCK_START, NDS_DP_SMALL_BLOCK_FOOTER - NDS_DP_SMALL_BLOCK_START);
			scs->big = nds_crc16(ptr + NDS_DP_BIG_BLOCK_START, NDS_DP_BIG_BLOCK_FOOTER - NDS_DP_BIG_BLOCK_START);
			break;
		case NDS_TYPE_PLAT:
			scs->small = nds_crc16(ptr + NDS_PLAT_SMALL_BLOCK_START, NDS_PLAT_SMALL_BLOCK_FOOTER - NDS_PLAT_SMALL_BLOCK_START);
			scs->big = nds_crc16(ptr + NDS_PLAT_BIG_BLOCK_START, NDS_PLAT_BIG_BLOCK_FOOTER - NDS_PLAT_BIG_BLOCK_START);
			break;
		case NDS_TYPE_HGSS:
			scs->small = nds_crc16(ptr + NDS_HGSS_SMALL_BLOCK_START, NDS_HGSS_SMALL_BLOCK_FOOTER - NDS_HGSS_SMALL_BLOCK_START);
			scs->big = nds_crc16(ptr + NDS_HGSS_BIG_BLOCK_START, NDS_HGSS_BIG_BLOCK_FOOTER - NDS_HGSS_BIG_BLOCK_START);
			break;
		default:
			break;
	}
}


void nds_calc_checksum(nds_save_t *save, nds_checksum_t *cs) {
	if(save->size == NDS_SAVE_SIZE_256 || save->size == NDS_SAVE_SIZE_512) {
		nds_calc_save_checksum(save->data, &cs->save0, save->type);
	}
	if(save->size == NDS_SAVE_SIZE_512) {
		nds_calc_save_checksum(save->data + NDS_SAVE_OFFSET, &cs->save1, save->type);
	}
}


void nds_fix_checksum(nds_save_t *save) {
	nds_save_checksum_t scs;
	if(save->size == NDS_SAVE_SIZE_256 || save->size == NDS_SAVE_SIZE_512) {
		nds_calc_save_checksum(save->data, &scs, save->type);
		nds_set_save_checksum(save->data, &scs, save->type);
	}
	if(save->size == NDS_SAVE_SIZE_512) {
		nds_calc_save_checksum(save->data + NDS_SAVE_OFFSET, &scs, save->type);
		nds_set_save_checksum(save->data + NDS_SAVE_OFFSET, &scs, save->type);
	}
}

nds_savetype_t nds_detect_save_type(void *ptr, size_t size) {
	if(size != NDS_SAVE_SIZE_256 && size != NDS_SAVE_SIZE_512) {
		return NDS_TYPE_UNKNOWN;
	}
	if(*(uint32_t *)(ptr + NDS_TYPE_DETECT_DP) == NDS_DP_SMALL_BLOCK_FOOTER - NDS_DP_SMALL_BLOCK_START) {
		return NDS_TYPE_DP;
	}
	if(*(uint32_t *)(ptr + NDS_TYPE_DETECT_PLAT) == NDS_PLAT_SMALL_BLOCK_FOOTER - NDS_PLAT_SMALL_BLOCK_START) {
		return NDS_TYPE_PLAT;
	}
	if(*(uint32_t *)(ptr + NDS_TYPE_DETECT_HGSS) == NDS_HGSS_SMALL_BLOCK_FOOTER - NDS_HGSS_SMALL_BLOCK_START) {
		return NDS_TYPE_HGSS;
	}
	return NDS_TYPE_UNKNOWN;
}

nds_save_t *nds_get_save(void *ptr, size_t size) {
	nds_save_t *save = malloc(sizeof(nds_save_t));
	save->type = nds_detect_save_type(ptr,size);
	save->data = ptr;
	save->size = size;
	return save;
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