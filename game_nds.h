#ifndef __NDS_H__
#define __NDS_H__

#include <stdlib.h>
#include <stdint.h>
#include "prng.h"
#include "checksum.h"

//SAVE
typedef enum {
	NDS_TYPE_DP,
	NDS_TYPE_PLAT,
	NDS_TYPE_HGSS,
	NDS_TYPE_UNKNOWN
} nds_savetype_t;

enum {
	NDS_SAVE_OFFSET = 0x40000,
	NDS_SAVE_SIZE_256 = 0x40000,
	NDS_SAVE_SIZE_512 = 0x80000,
};

uint16_t nds_calc_small_block_checksum(void *, nds_savetype_t);
uint16_t nds_calc_big_block_checksum(void *, nds_savetype_t);

uint16_t nds_get_small_block_checksum(void *, nds_savetype_t);
uint16_t nds_get_big_block_checksum(void *, nds_savetype_t);

void nds_set_small_block_checksum(void *, nds_savetype_t, uint16_t);
void nds_set_big_block_checksum(void *, nds_savetype_t, uint16_t);

void nds_fix_small_block_checksum(void *, nds_savetype_t);
void nds_fix_big_block_checksum(void *, nds_savetype_t);

void nds_fix_small_and_big_block_checksum(void *, nds_savetype_t);

void nds_fix_all_checksums(void *, nds_savetype_t, size_t);

nds_savetype_t nds_detect_save_type(void *, size_t);

//PKM
enum {
	NDS_PKM_SIZE = 136,
	NDS_PARTY_PKM_SIZE = 236,
};

void nds_shuffle_pkm(void *);
void nds_unshuffle_pkm(void *);
void nds_crypt_pkm(void *);
void nds_crypt_party_pkm(void *);


#endif //__NDS_H__