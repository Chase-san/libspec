/*
 * stat.h
 *
 *  Created on: Mar 25, 2014
 *      Author: Chase
 */

#ifndef STAT_H_
#define STAT_H_

#include "types.h"

uint16_t gb_calc_stat(uint8_t level, uint8_t base_stat, uint8_t iv, uint16_t stat_exp);
uint16_t gb_calc_hp_stat(uint8_t level, uint8_t base_stat, uint8_t iv, uint16_t stat_exp);

typedef enum {
	STAT_NATURE_NONE = 0,
	STAT_NATURE_POSITIVE = 1,
	STAT_NATURE_NEGATIVE = 2
} stat_nature_t;

uint16_t gba_calc_stat(uint8_t level, uint8_t base_stat, uint8_t iv, uint8_t ev, stat_nature_t nature);
uint16_t gba_calc_hp_stat(uint8_t level, uint8_t base_stat, uint8_t iv, uint8_t ev);

uint16_t nds_calc_stat(uint8_t level, uint8_t base_stat, uint8_t iv, uint8_t ev, stat_nature_t nature);
uint16_t nds_calc_hp_stat(uint8_t level, uint8_t base_stat, uint8_t iv, uint8_t ev);

uint16_t dsi_calc_stat(uint8_t level, uint8_t base_stat, uint8_t iv, uint8_t ev, stat_nature_t nature);
uint16_t dsi_calc_hp_stat(uint8_t level, uint8_t base_stat, uint8_t iv, uint8_t ev);

#endif /* STAT_H_ */
