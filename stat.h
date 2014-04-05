/**
 * @file stat.h
 * @brief Contains functions for calculating the stats of pokemon.
 */

#ifndef STAT_H_
#define STAT_H_

#include "types.h"

/**
 * @brief List of natures by index.
 */
typedef enum {
	NATURE_HARDY = 0,
	NATURE_LONELY = 1,
	NATURE_BRAVE = 2,
	NATURE_ADAMANT = 3,
	NATURE_NAUGHTY = 4,
	NATURE_BOLD = 5,
	NATURE_DOCILE = 6,
	NATURE_RELAXED = 7,
	NATURE_IMPISH = 8,
	NATURE_LAX = 9,
	NATURE_TIMID = 10,
	NATURE_HASTY = 11,
	NATURE_SERIOUS = 12,
	NATURE_JOLLY = 13,
	NATURE_NAIVE = 14,
	NATURE_MODEST = 15,
	NATURE_MILD = 16,
	NATURE_QUIET = 17,
	NATURE_BASHFUL = 18,
	NATURE_RASH = 19,
	NATURE_CALM = 20,
	NATURE_GENTLE = 21,
	NATURE_SASSY = 22,
	NATURE_CAREFUL = 23,
	NATURE_QUIRKY = 24
} stat_nature_t;

/**
 * List of stats by index.
 */
typedef enum {
	STAT_HP = 0,
	STAT_ATTAK = 1,
	STAT_DEFENSE = 2,
	STAT_SPEED = 3,
	STAT_SP_ATTACK = 4,
	STAT_SP_DEFENSE = 5,
} stat_stat_t;

/**
 * @brief Enum to tell the stat functions if a nature is beneficial or otherwise.
 */
typedef enum {
	/** A neutral bonus for the given stat. */
	STAT_BONUS_NONE,
	/** A beneficial bonus for the given stat. */
	STAT_BONUS_POSITIVE,
	/** A harmful bonus for the given stat. */
	STAT_BONUS_NEGATIVE
} stat_bonus_t;

/**
 * @brief Erratic Experience Table, indexed 0 to 99 for levels 1 to 100.
 */
extern const uint32_t STAT_TOTAL_EXP_ERRATIC[100];

/**
 * @brief Fast Experience Table, indexed 0 to 99 for levels 1 to 100.
 */
extern const uint32_t STAT_TOTAL_EXP_FAST[100];

/**
 * @brief Medium Fast Experience Table, indexed 0 to 99 for levels 1 to 100.
 */
extern const uint32_t STAT_TOTAL_EXP_MEDIUM_FAST[100];

/**
 * @brief Medium Slow Experience Table, indexed 0 to 99 for levels 1 to 100.
 */
extern const uint32_t STAT_TOTAL_EXP_MEDIUM_SLOW[100];

/**
 * @brief Slow Experience Table, indexed 0 to 99 for levels 1 to 100.
 */
extern const uint32_t STAT_TOTAL_EXP_SLOW[100];

/**
 * @brief Fluctuating Experience Table, indexed 0 to 99 for levels 1 to 100.
 */
extern const uint32_t STAT_TOTAL_EXP_FLUCTUATING[100];

typedef enum {
	STAT_GROWTH_RATE_ERRATIC = 0,
	STAT_GROWTH_RATE_FAST = 1,
	STAT_GROWTH_RATE_MEDIUM_FAST = 2,
	STAT_GROWTH_RATE_MEDIUM_SLOW = 3,
	STAT_GROWTH_RATE_SLOW = 4,
	STAT_GROWTH_RATE_FLUCTUATING = 5
} stat_growth_rate_t;

#ifdef __cplusplus
extern "C" {
#endif

stat_bonus_t stat_get_bonus(stat_nature_t, stat_stat_t);
stat_nature_t stat_get_nature(uint32_t pid);
uint8_t stat_get_level(stat_growth_rate_t, uint32_t exp);

uint16_t gb_calc_stat(uint8_t, uint8_t, uint8_t, uint16_t);
uint16_t gb_calc_hp_stat(uint8_t, uint8_t, uint8_t, uint16_t);

uint16_t gba_calc_stat(uint8_t, uint8_t, uint8_t, uint8_t, stat_bonus_t);
uint16_t gba_calc_hp_stat(uint8_t, uint8_t, uint8_t, uint8_t);

uint16_t nds_calc_stat(uint8_t level, uint8_t base_stat, uint8_t iv, uint8_t ev, stat_bonus_t);
uint16_t nds_calc_hp_stat(uint8_t, uint8_t, uint8_t, uint8_t);

uint16_t dsi_calc_stat(uint8_t level, uint8_t base_stat, uint8_t iv, uint8_t ev, stat_bonus_t);
uint16_t dsi_calc_hp_stat(uint8_t, uint8_t, uint8_t, uint8_t);

#ifdef __cplusplus
}
#endif

#endif /* STAT_H_ */
