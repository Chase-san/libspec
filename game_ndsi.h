//Generation 5
//The "DSi-enhanced" Pokemon Games.

//Black/White
//Black 2/White 2

#ifndef __DSI_H__
#define __DSI_H__

#include <stdlib.h>
#include <stdint.h>

//SAVE
typedef enum {
	DSI_TYPE_BW,
	DSI_TYPE_B2W2,
	DSI_TYPE_UNKNOWN
} dsi_savetype_t;

enum {
	DSI_SAVE_SIZE_512 = 0x80000
};

dsi_savetype_t dsi_detect_save_type(void *, size_t);

#endif //__DSI_H__