//Generation 4
//The regular DS games

//Diamond/Pearl/Platinum
//Heart Gold/Soul Silver

#include <stdlib.h>
#include <string.h>
#include "game_nds.h"

///////////////////////////////////////////////////
// SAVE STUFF

enum {
	NDS_SMALL_FOOTER_START_DP = 0xC0EC,
	NDS_SMALL_FOOTER_START_PLAT = 0xCF18,
	NDS_SMALL_FOOTER_START_HGSS = 0xF618,

	NDS_BIG_FOOTER_START_DP = 0x1E2CC,
	NDS_BIG_FOOTER_START_PLAT = 0x1F0FC,
	NDS_BIG_FOOTER_START_HGSS = 0x21A00,

	NDS_FOOTER_SIZE_DPPT = 0x14,
	NDS_FOOTER_SIZE_HGSS = 0x10,

	NDS_TYPE_DETECT_DP = NDS_SMALL_FOOTER_START_DP + 0x8,
	NDS_TYPE_DETECT_PLAT = NDS_SMALL_FOOTER_START_PLAT + 0x8,
	NDS_TYPE_DETECT_HGSS = NDS_SMALL_FOOTER_START_HGSS + 0x4,

	NDS_SMALL_BLOCK_START_DP = 0x0,
	NDS_SMALL_BLOCK_START_PLAT = 0x0,
	NDS_SMALL_BLOCK_START_HGSS = 0x0,

	NDS_BIG_BLOCK_START_DP = 0xC100,
	NDS_BIG_BLOCK_START_PLAT = 0xCF2C,
	NDS_BIG_BLOCK_START_HGSS = 0xF700,

	NDS_SMALL_BLOCK_SIZE_DP = 0xC100,
	NDS_SMALL_BLOCK_SIZE_PLAT = 0xCF2C,
	NDS_SMALL_BLOCK_SIZE_HGSS = 0xF628,

	NDS_BIG_BLOCK_SIZE_DP = 0x121E0,
	NDS_BIG_BLOCK_SIZE_PLAT = 0x121E4,
	NDS_BIG_BLOCK_SIZE_HGSS = 0x12310,

	NDS_PARTY_START_DP = 0x98,
	NDS_PARTY_START_PLAT = 0xA0,
	NDS_PARTY_START_HGSS = 0xA0,

	NDS_SIGNATURE_START_DP = 0x5904,
	NDS_SIGNATURE_START_PLAT = 0x5BA8,
	NDS_SIGNATURE_START_HGSS = 0x4538,

	NDS_SIGNATURE_SIZE = 0x600
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
typedef struct {
	uint32_t saveNum;
	uint32_t blockSize;
	uint32_t unknown0;
	uint16_t unknown1;
	uint16_t checksum;
} nds_footer_hgss_t;
#pragma pack(pop)


static inline nds_footer_dppt_t *get_dppt_footer(void *ptr, size_t offset) {
	return (nds_footer_dppt_t *)(ptr+offset);
}
static inline nds_footer_hgss_t *get_hgss_footer(void *ptr, size_t offset) {
	return (nds_footer_hgss_t *)(ptr+offset);
}

static inline uint16_t nds_calc_dp_small_block_checksum(void *ptr) {
	return nds_crc16(ptr + NDS_SMALL_BLOCK_START_DP, NDS_SMALL_BLOCK_SIZE_DP - NDS_FOOTER_SIZE_DPPT);
}
static inline uint16_t nds_calc_dp_big_block_checksum(void *ptr) {
	return nds_crc16(ptr + NDS_BIG_BLOCK_START_DP, NDS_BIG_BLOCK_SIZE_DP - NDS_FOOTER_SIZE_DPPT);
}
static inline uint16_t nds_calc_plat_small_block_checksum(void *ptr) {
	return nds_crc16(ptr + NDS_SMALL_BLOCK_START_PLAT, NDS_SMALL_BLOCK_SIZE_PLAT - NDS_FOOTER_SIZE_DPPT);
}
static inline uint16_t nds_calc_plat_big_block_checksum(void *ptr) {
	return nds_crc16(ptr + NDS_BIG_BLOCK_START_PLAT, NDS_BIG_BLOCK_SIZE_PLAT - NDS_FOOTER_SIZE_DPPT);
}
static inline uint16_t nds_calc_hgss_small_block_checksum(void *ptr) {
	return nds_crc16(ptr + NDS_SMALL_BLOCK_START_HGSS, NDS_SMALL_BLOCK_SIZE_HGSS - NDS_FOOTER_SIZE_HGSS);
}
static inline uint16_t nds_calc_hgss_big_block_checksum(void *ptr) {
	return nds_crc16(ptr + NDS_BIG_BLOCK_START_HGSS, NDS_BIG_BLOCK_SIZE_HGSS - NDS_FOOTER_SIZE_HGSS);
}

uint16_t nds_calc_small_block_checksum(void *ptr, nds_savetype_t type) {
	switch(type) {
		case NDS_TYPE_DP:
			return nds_calc_dp_small_block_checksum(ptr);
		case NDS_TYPE_PLAT:
			return nds_calc_plat_small_block_checksum(ptr);
		case NDS_TYPE_HGSS:
			return nds_calc_hgss_small_block_checksum(ptr);
		default:
			return 0;
	}
}
uint16_t nds_calc_big_block_checksum(void *ptr, nds_savetype_t type) {
	switch(type) {
		case NDS_TYPE_DP:
			return nds_calc_dp_big_block_checksum(ptr);
		case NDS_TYPE_PLAT:
			return nds_calc_plat_big_block_checksum(ptr);
		case NDS_TYPE_HGSS:
			return nds_calc_hgss_big_block_checksum(ptr);
		default:
			return 0;
	}
}

static inline uint16_t nds_get_dp_small_block_checksum(void *ptr) {
	return get_dppt_footer(ptr, NDS_SMALL_FOOTER_START_DP)->checksum;
}
static inline uint16_t nds_get_plat_small_block_checksum(void *ptr) {
	return get_dppt_footer(ptr, NDS_SMALL_FOOTER_START_PLAT)->checksum;
}
static inline uint16_t nds_get_hgss_small_block_checksum(void *ptr) {
	return get_hgss_footer(ptr, NDS_SMALL_FOOTER_START_HGSS)->checksum;
}
static inline uint16_t nds_get_dp_big_block_checksum(void *ptr) {
	return get_dppt_footer(ptr, NDS_BIG_FOOTER_START_DP)->checksum;
}
static inline uint16_t nds_get_plat_big_block_checksum(void *ptr) {
	return get_dppt_footer(ptr, NDS_SMALL_FOOTER_START_PLAT)->checksum;
}
static inline uint16_t nds_get_hgss_big_block_checksum(void *ptr) {
	return get_hgss_footer(ptr, NDS_BIG_FOOTER_START_HGSS)->checksum;
}

uint16_t nds_get_small_block_checksum(void *ptr, nds_savetype_t type) {
	switch(type) {
		case NDS_TYPE_DP:
			return nds_get_dp_small_block_checksum(ptr);
		case NDS_TYPE_PLAT:
			return nds_get_plat_small_block_checksum(ptr);
		case NDS_TYPE_HGSS:
			return nds_get_hgss_small_block_checksum(ptr);
		default:
			return 0;
	}
}
uint16_t nds_get_big_block_checksum(void *ptr, nds_savetype_t type) {
	switch(type) {
		case NDS_TYPE_DP:
			return nds_get_dp_big_block_checksum(ptr);
		case NDS_TYPE_PLAT:
			return nds_get_plat_big_block_checksum(ptr);
		case NDS_TYPE_HGSS:
			return nds_get_hgss_big_block_checksum(ptr);
		default:
			return 0;
	}
}

static inline void nds_set_dp_small_block_checksum(void *ptr, uint16_t sum) {
	get_dppt_footer(ptr, NDS_SMALL_FOOTER_START_DP)->checksum = sum;
}
static inline void nds_set_dp_big_block_checksum(void *ptr, uint16_t sum) {
	get_dppt_footer(ptr, NDS_BIG_FOOTER_START_DP)->checksum = sum;
}
static inline void nds_set_plat_small_block_checksum(void *ptr, uint16_t sum) {
	get_dppt_footer(ptr, NDS_SMALL_FOOTER_START_PLAT)->checksum = sum;
}
static inline void nds_set_plat_big_block_checksum(void *ptr, uint16_t sum) {
	get_dppt_footer(ptr, NDS_SMALL_FOOTER_START_PLAT)->checksum = sum;
}
static inline void nds_set_hgss_small_block_checksum(void *ptr, uint16_t sum) {
	get_hgss_footer(ptr, NDS_SMALL_FOOTER_START_HGSS)->checksum = sum;
}
static inline void nds_set_hgss_big_block_checksum(void *ptr, uint16_t sum) {
	get_hgss_footer(ptr, NDS_BIG_FOOTER_START_HGSS)->checksum = sum;
}

void nds_set_small_block_checksum(void *ptr, nds_savetype_t type, uint16_t sum) {
	switch(type) {
		case NDS_TYPE_DP:
			nds_set_dp_small_block_checksum(ptr, sum);
			break;
		case NDS_TYPE_PLAT:
			nds_set_plat_small_block_checksum(ptr, sum);
			break;
		case NDS_TYPE_HGSS:
			nds_set_hgss_small_block_checksum(ptr, sum);
			break;
		default:
			break;
	}
}
void nds_set_big_block_checksum(void *ptr, nds_savetype_t type, uint16_t sum) {
	switch(type) {
		case NDS_TYPE_DP:
			nds_set_dp_big_block_checksum(ptr, sum);
			break;
		case NDS_TYPE_PLAT:
			nds_set_plat_big_block_checksum(ptr, sum);
			break;
		case NDS_TYPE_HGSS:
			nds_set_hgss_big_block_checksum(ptr, sum);
			break;
		default:
			break;
	}
}

static inline void nds_fix_dp_small_block_checksum(void *ptr) {
	nds_set_dp_small_block_checksum(ptr, nds_calc_dp_small_block_checksum(ptr));
}
static inline void nds_fix_dp_big_block_checksum(void *ptr) {
	nds_set_dp_big_block_checksum(ptr, nds_calc_dp_big_block_checksum(ptr));
}
static inline void nds_fix_plat_small_block_checksum(void *ptr) {
	nds_set_plat_small_block_checksum(ptr, nds_calc_plat_small_block_checksum(ptr));
}
static inline void nds_fix_plat_big_block_checksum(void *ptr) {
	nds_set_plat_big_block_checksum(ptr, nds_calc_plat_big_block_checksum(ptr));
}
static inline void nds_fix_hgss_small_block_checksum(void *ptr) {
	nds_set_hgss_small_block_checksum(ptr, nds_calc_hgss_small_block_checksum(ptr));
}
static inline void nds_fix_hgss_big_block_checksum(void *ptr) {
	nds_set_hgss_big_block_checksum(ptr, nds_calc_hgss_big_block_checksum(ptr));
}

void nds_fix_small_block_checksum(void *ptr, nds_savetype_t type) {
	switch(type) {
		case NDS_TYPE_DP:
			nds_fix_dp_small_block_checksum(ptr);
			break;
		case NDS_TYPE_PLAT:
			nds_fix_plat_small_block_checksum(ptr);
			break;
		case NDS_TYPE_HGSS:
			nds_fix_hgss_small_block_checksum(ptr);
			break;
		default:
			break;
	}
}

void nds_fix_big_block_checksum(void *ptr, nds_savetype_t type) {
	switch(type) {
		case NDS_TYPE_DP:
			nds_fix_dp_big_block_checksum(ptr);
			break;
		case NDS_TYPE_PLAT:
			nds_fix_plat_big_block_checksum(ptr);
			break;
		case NDS_TYPE_HGSS:
			nds_fix_hgss_big_block_checksum(ptr);
			break;
		default:
			break;
	}
}

void nds_fix_small_and_big_block_checksum(void *ptr, nds_savetype_t type) {
	switch(type) {
		case NDS_TYPE_DP:
			nds_fix_dp_small_block_checksum(ptr);
			nds_fix_dp_big_block_checksum(ptr);
			break;
		case NDS_TYPE_PLAT:
			nds_fix_plat_small_block_checksum(ptr);
			nds_fix_plat_big_block_checksum(ptr);
			break;
		case NDS_TYPE_HGSS:
			nds_fix_hgss_small_block_checksum(ptr);
			nds_fix_hgss_big_block_checksum(ptr);
			break;
		default:
			break;
	}
}

void nds_fix_all_checksums(void *ptr, nds_savetype_t type, size_t size) {
	if(size == NDS_SAVE_SIZE_256 || size == NDS_SAVE_SIZE_512) {
		nds_fix_small_and_big_block_checksum(ptr, type);
	}
	if(size == NDS_SAVE_SIZE_512) {
		nds_fix_small_and_big_block_checksum(ptr + NDS_SAVE_OFFSET, type);
	}
}

nds_savetype_t nds_detect_save_type(void *ptr, size_t size) {
	if(size != NDS_SAVE_SIZE_256 && size != NDS_SAVE_SIZE_512) {
		return NDS_TYPE_UNKNOWN;
	}
	if(*(uint32_t *)(ptr + NDS_TYPE_DETECT_DP) == NDS_SMALL_BLOCK_SIZE_DP) {
		return NDS_TYPE_DP;
	}
	if(*(uint32_t *)(ptr + NDS_TYPE_DETECT_PLAT) == NDS_SMALL_BLOCK_SIZE_PLAT) {
		return NDS_TYPE_PLAT;
	}
	if(*(uint32_t *)(ptr + NDS_TYPE_DETECT_HGSS) == NDS_SMALL_BLOCK_SIZE_HGSS) {
		return NDS_TYPE_HGSS;
	}
	return NDS_TYPE_UNKNOWN;
}


///////////////////////////////////////////////////
///////////////////////////////////////////////////
// PKM STUFF

//Magic numbers, what magic numbers?
enum {
	PKM_HEADER_SIZE_8 = 8,
	PKM_HEADER_SIZE_16 = 4,
	PKM_BLOCK_SIZE_8 = 32,
	PKM_DATA_SIZE_8 = 128,
	PKM_DATA_SIZE_16 = 64,
	PKM_PARTY_DATA_SIZE_8 = 100,
	PKM_PARTY_DATA_SIZE_16 = 50,
	PKM_PARTY_DATA_START_8 = PKM_DATA_SIZE_8 + PKM_HEADER_SIZE_8,
	PKM_PARTY_DATA_START_16 = PKM_DATA_SIZE_16 + PKM_HEADER_SIZE_16,
	PKM_BLOCK0_START_8 = 0,
	PKM_BLOCK1_START_8 = 32,
	PKM_BLOCK2_START_8 = 64,
	PKM_BLOCK3_START_8 = 96,
	PKM_CHECKSUM_OFFSET_16 = 3,
	PKM_SHUFFLE_MASK = 0x3E000,
	PKM_SHUFFLE_SHIFT = 0xB,
	PKM_PID_START_32 = 0
};

//You might think, hey this looks nothing like my shuffle table, and you would be right.
static const uint8_t t_shuffle[] = {
	0x00, 0x20, 0x40, 0x60, 0x00, 0x20, 0x60, 0x40, 0x00, 0x40, 0x20, 0x60, 0x00, 0x40, 0x60, 0x20,
	0x00, 0x60, 0x20, 0x40, 0x00, 0x60, 0x40, 0x20, 0x20, 0x00, 0x40, 0x60, 0x20, 0x00, 0x60, 0x40,
	0x20, 0x40, 0x00, 0x60, 0x20, 0x40, 0x60, 0x00, 0x20, 0x60, 0x00, 0x40, 0x20, 0x60, 0x40, 0x00,
	0x40, 0x00, 0x20, 0x60, 0x40, 0x00, 0x60, 0x20, 0x40, 0x20, 0x00, 0x60, 0x40, 0x20, 0x60, 0x00,
	0x40, 0x60, 0x00, 0x20, 0x40, 0x60, 0x20, 0x00, 0x60, 0x00, 0x20, 0x40, 0x60, 0x00, 0x40, 0x20,
	0x60, 0x20, 0x00, 0x40, 0x60, 0x20, 0x40, 0x00, 0x60, 0x40, 0x00, 0x20, 0x60, 0x40, 0x20, 0x00,
	0x00, 0x20, 0x40, 0x60, 0x00, 0x20, 0x60, 0x40, 0x00, 0x40, 0x20, 0x60, 0x00, 0x40, 0x60, 0x20,
	0x00, 0x60, 0x20, 0x40, 0x00, 0x60, 0x40, 0x20, 0x20, 0x00, 0x40, 0x60, 0x20, 0x00, 0x60, 0x40
};

//You may be thinking, hey that isn't the shuffle mode, and you would be half right.
static inline const uint8_t *get_shuffle(void *ptr) {
	return &t_shuffle[(*(uint32_t *)ptr & PKM_SHUFFLE_MASK) >> PKM_SHUFFLE_SHIFT];
}

void nds_shuffle_pkm(void *ptr) {
	//0,32,64,96 to shuffle[0 .. 3]
	const uint8_t *shuffle = get_shuffle(ptr);
	uint8_t *bptr = ptr + PKM_HEADER_SIZE_8;
	uint8_t *tmp = malloc(PKM_DATA_SIZE_8);
	memcpy(tmp, bptr, PKM_DATA_SIZE_8);
	memcpy(&bptr[shuffle[0]], &tmp[PKM_BLOCK0_START_8], PKM_BLOCK_SIZE_8);
	memcpy(&bptr[shuffle[1]], &tmp[PKM_BLOCK1_START_8], PKM_BLOCK_SIZE_8);
	memcpy(&bptr[shuffle[2]], &tmp[PKM_BLOCK2_START_8], PKM_BLOCK_SIZE_8);
	memcpy(&bptr[shuffle[3]], &tmp[PKM_BLOCK3_START_8], PKM_BLOCK_SIZE_8);
	free(tmp);
}

void nds_unshuffle_pkm(void *ptr) {
	//shuffle[0 .. 3] to 0,32,64,96
	const uint8_t *shuffle = get_shuffle(ptr);
	uint8_t *bptr = ptr + PKM_HEADER_SIZE_8;
	uint8_t *tmp = malloc(PKM_DATA_SIZE_8);
	memcpy(tmp, bptr, PKM_DATA_SIZE_8);
	memcpy(&bptr[PKM_BLOCK0_START_8], &tmp[shuffle[0]], PKM_BLOCK_SIZE_8);
	memcpy(&bptr[PKM_BLOCK1_START_8], &tmp[shuffle[1]], PKM_BLOCK_SIZE_8);
	memcpy(&bptr[PKM_BLOCK2_START_8], &tmp[shuffle[2]], PKM_BLOCK_SIZE_8);
	memcpy(&bptr[PKM_BLOCK3_START_8], &tmp[shuffle[3]], PKM_BLOCK_SIZE_8);
	free(tmp);
}

void nds_crypt_pkm(void *ptr) {
	uint16_t *tptr = ptr;
	nds_prng_t prng = { tptr[PKM_CHECKSUM_OFFSET_16] };
	for(size_t i = 0; i < PKM_DATA_SIZE_16; ++i) {
		tptr[PKM_HEADER_SIZE_16 + i] ^= nds_prng_next(&prng);
	}
}

void nds_crypt_party_pkm(void *ptr) {
	uint16_t *tptr = ptr;
	nds_prng_t prng = { tptr[PKM_CHECKSUM_OFFSET_16] };
	for(size_t i = 0; i < PKM_DATA_SIZE_16; ++i) {
		tptr[PKM_HEADER_SIZE_16 + i] ^= nds_prng_next(&prng);
	}
	prng.seed = ((uint32_t *)ptr)[PKM_PID_START_32];
	for(uint8_t i = 0; i < PKM_PARTY_DATA_SIZE_16; ++i) {
		tptr[PKM_PARTY_DATA_START_16 + i] ^= nds_prng_next(&prng);
	}
}
