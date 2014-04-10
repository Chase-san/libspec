/**
 * The GB games, Generation 1 & 2 games. These games include Red, Blue, Yellow, Gold, Silver, and Crystal.
 *
 * @file game_gb.h
 * @brief Contains the structures and functions for editing GB pokemon save games.
 */

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

#ifdef __cplusplus
extern "C" {
#endif

void gb_text_to_ucs2(char16_t *dst, char8_t *src, size_t size);
void ucs2_to_gb_text(char8_t *dst, char16_t *src, size_t size);

gb_save_t *gb_read_save(const uint8_t *);
void gb_free_save(gb_save_t *);

uint8_t *gb_create_data();

void gb_write_save(uint8_t *, const gb_save_t *);

#ifdef __cplusplus
}
#endif

#endif //__GB_H__
