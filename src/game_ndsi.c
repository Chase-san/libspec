//Generation 5
//The "DSi-enhanced" Pokemon Games.

//Black/White
//Black 2/White 2

#include <string.h>
#include "game_ndsi.h"
#include "checksum.h"

//B2W2 save is split up into blocks
//each block has a length and a checksum following immediately after it
//each checksum also

enum {
	DSI_CHECKSUM_LENGTH = 0x2,
	DSI_FOOTER_LENGTH = 0x4,

	DSI_CHECKSUM_BLOCK_START = 0x25F00,
	DSI_CHECKSUM_BLOCK_LENGTH = 0x94,
	DSI_CHECKSUM_BLOCK_CHECKSUM = 0x25FA2,

	DSI_BOX_NAMES_START = 0x0,
	DSI_BOX_NAMES_LENGTH = 0x3DF,
	DSI_BOX_NAMES_CHECKSUM = 0x3E2,
	DSI_BOX_NAMES_BLOCK_CHECKSUM = 0x25F00,

	DSI_BOX_COUNT = 24,
	DSI_BOX_START = 0x400,
	DSI_BOX_LENGTH = 0xFF0,
	DSI_BOX_BLOCK_LENGTH = 0x1000,
	DSI_BOX_CHECKSUM_OFFSET = 0xFF2,
	DSI_BOX_CHECKSUM_BLOCK_START = 0x25F02,

	DSI_INVENTORY_START = 0x18400,
	//DSI_INVENTORY_LENGTH = 0x


};

//works for boxname, boxes
#pragma pack(push, 1)
typedef struct {
	uint16_t unknown;
	uint16_t checksum;
} dsi_footer_t;
#pragma pack(pop)

typedef struct {
	uint8_t *data;
	dsi_footer_t *footer;
} dsi_block_t;

dsi_block_t dsi_get_block(uint8_t *ptr, size_t start, size_t len) {
	dsi_block_t block;
	block.data = ptr + start;
	block.footer = (dsi_footer_t *)(ptr + start + len);
	return block;
}

dsi_savetype_t dsi_detect_save_type(uint8_t *ptr, size_t size) {
	//TODO
	return DSI_TYPE_UNKNOWN;
}

//////////////////////////////////////////
// TESTING

#include <stdio.h>

void dsi_test(uint8_t *ptr) {
	//get a block and check things
	//now do it for each box, :3
	for(int i = 0; i < DSI_BOX_COUNT; ++i) {
		dsi_block_t block = dsi_get_block(ptr, DSI_BOX_START + DSI_BOX_BLOCK_LENGTH * i, DSI_BOX_LENGTH);
		uint16_t checksum = block.footer->checksum;
		uint16_t calc_checksum = nds_crc16(ptr + DSI_BOX_START + DSI_BOX_BLOCK_LENGTH * i, DSI_BOX_LENGTH);
		printf("Box %02d : %04X %04X\n", i, checksum, calc_checksum);
	}
	uint16_t checksum = *(uint16_t *)(ptr + 0x25FA2);
	uint16_t calc_checksum = nds_crc16(ptr + DSI_CHECKSUM_BLOCK_START, DSI_CHECKSUM_BLOCK_LENGTH);
	printf("Checksum Block : %04X %04X\n", checksum, calc_checksum);
}
