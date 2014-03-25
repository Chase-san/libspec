/**
 * The DSi-enhanced games, Generation 5 games. These games include Black, White, Black 2 and White 2.
 *
 * @file game_ndsi.h
 * @brief Contains the structures and functions for editing DSi-enhanced pokemon save games.
 */

#ifndef __DSI_H__
#define __DSI_H__

#include <stdlib.h>
#include <stdint.h>

//SAVE
typedef enum {
	DSI_TYPE_UNKNOWN,
	DSI_TYPE_BW,
	DSI_TYPE_B2W2
} dsi_savetype_t;

enum {
	DSI_SAVE_SIZE = 0x80000
};

dsi_savetype_t dsi_detect_save_type(uint8_t *, size_t);

void dsi_test(uint8_t *);

#endif //__DSI_H__
