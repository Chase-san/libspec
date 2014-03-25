/**
 * @file stat.h
 * @brief Contains functions for calculating the stats of pokemon.
 */

#ifndef STAT_H_
#define STAT_H_

#include "types.h"

/**
 * Enum to tell the stat functions if a nature is beneficial or otherwise.
 */
typedef enum {
	/** A neutral nature for the given stat. */
	STAT_NATURE_NONE = 0,
	/** A beneficial nature for the given stat. */
	STAT_NATURE_POSITIVE = 1,
	/** A harmful nature for the given stat. */
	STAT_NATURE_NEGATIVE = 2
} stat_nature_t;

uint16_t gb_calc_stat(uint8_t, uint8_t, uint8_t, uint16_t);
uint16_t gb_calc_hp_stat(uint8_t, uint8_t, uint8_t, uint16_t);

uint16_t gba_calc_stat(uint8_t, uint8_t, uint8_t, uint8_t, stat_nature_t);
uint16_t gba_calc_hp_stat(uint8_t, uint8_t, uint8_t, uint8_t);

uint16_t nds_calc_stat(uint8_t level, uint8_t base_stat, uint8_t iv, uint8_t ev, stat_nature_t);
uint16_t nds_calc_hp_stat(uint8_t, uint8_t, uint8_t, uint8_t);

uint16_t dsi_calc_stat(uint8_t level, uint8_t base_stat, uint8_t iv, uint8_t ev, stat_nature_t);
uint16_t dsi_calc_hp_stat(uint8_t, uint8_t, uint8_t, uint8_t);

#endif /* STAT_H_ */
