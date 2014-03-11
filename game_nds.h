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

typedef struct {
	uint16_t small;
	uint16_t big;
} nds_save_checksum_t;

typedef struct {
	nds_save_checksum_t save0;
	nds_save_checksum_t save1;
} nds_checksum_t;

typedef struct {
	uint8_t *data;
	size_t size;
	nds_savetype_t type;
} nds_save_t;

void nds_get_checksum(nds_save_t *, nds_checksum_t *);
void nds_set_checksum(nds_save_t *, nds_checksum_t *);
void nds_calc_checksum(nds_save_t *, nds_checksum_t *);
void nds_fix_checksum(nds_save_t *);

nds_save_t *nds_get_save(uint8_t *, size_t);

#endif //__NDS_H__
