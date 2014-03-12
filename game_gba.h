//Generation 3
//The GBA Games

//Ruby/Sapphire/Emerald
//Fire Red/Leaf Green

#ifndef __GBA_H__
#define __GBA_H__

#include <stdlib.h>
#include <stdint.h>

//SAVE
typedef enum {
	GBA_TYPE_RSE,
	GBA_TYPE_FRLG,
	GBA_TYPE_UNKNOWN
} gba_savetype_t;

enum {
	GBA_SAVE_SIZE = 0x20000
};

typedef struct {
	uint8_t *unpacked;
	void *internal;
} gba_save_t;

gba_savetype_t gba_detect_save_type(uint8_t *, size_t);

gba_save_t *gba_read_save(const uint8_t *);
gba_save_t *gba_read_backup(const uint8_t *);
void gba_free_save(gba_save_t *);

void gba_write_save(uint8_t *,const gba_save_t *);
void gba_write_backup(uint8_t *,const gba_save_t *);
void gba_save_game(uint8_t *,gba_save_t *);

#endif //__GBA_H__
