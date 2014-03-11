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
	GBA_SAVE_SECTION = 0xE000,
	GBA_SAVE_SIZE = 0x20000
};

gba_savetype_t gba_detect_save_type(uint8_t *, size_t);

void gba_fix_checksum(uint8_t *);

void gba_test(uint8_t *ptr);

#endif //__GBA_H__
