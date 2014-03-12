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

typedef struct {
	uint8_t *data;
	nds_savetype_t type;
	void *internal;
} nds_save_t;

nds_save_t *nds_read_main_save(const uint8_t *);
nds_save_t *nds_read_backup_save(const uint8_t *);
void nds_free_save(nds_save_t *);

void nds_write_main_save(uint8_t *, const nds_save_t *);
void nds_write_backup_save(uint8_t *, const nds_save_t *);

#endif //__NDS_H__
