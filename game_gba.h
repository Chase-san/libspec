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
	GBA_SAVE_SIZE = 0x20000,
	GBA_SAVE_BLOCK_COUNT = 14,
	GBA_BLOCK_LENGTH = 0x1000,
	GBA_BLOCK_DATA_LENGTH = 0xF80,
	GBA_BLOCK_FOOTER_LENGTH = 0xC,
	GBA_UNPACKED_LENGTH = GBA_BLOCK_DATA_LENGTH * GBA_SAVE_BLOCK_COUNT
};

#pragma pack(push, 1)
typedef struct {
	uint8_t data[GBA_UNPACKED_LENGTH];
	uint8_t order[GBA_SAVE_BLOCK_COUNT];
	uint32_t save_index;
} gba_save_t;
#pragma pack(pop)

gba_savetype_t gba_detect_save_type(uint8_t *, size_t);

void gba_fix_checksum(uint8_t *);
gba_save_t *gba_unpack(uint8_t *);

#endif //__GBA_H__
