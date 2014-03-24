//Generation 1&2
//The GB Games

//JPRed/JPGreen (gen 0)
//Red/Blue/Yellow
//Gold/Silver/Crystal

//Unlike later games, the GB games have no encryption or checksums
//Which makes working with them relatively easy

#ifndef __GB_H__
#define __GB_H__

#include "types.h"

enum {
	GB_SAVE_SIZE = 0x8000
};

typedef enum {
	GB_TYPE_UNKNOWN,
	GB_TYPE_RBY,
	GB_TYPE_GS,
	GB_TYPE_C
} gb_savetype_t;

typedef struct {
	uint8_t *data;
	gb_savetype_t type;
} gb_save_t;

void gb_text_to_ucs2(char16_t *dst, char8_t *src, size_t size);
void ucs2_to_gb_text(char8_t *dst, char16_t *src, size_t size);

gb_save_t *gb_read_save(const uint8_t *);
void gb_free_save(gb_save_t *);

uint8_t *gb_create_data();

void gb_write_save(uint8_t *, const gb_save_t *);


#endif //__GB_H__
