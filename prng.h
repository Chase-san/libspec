/**
 * @file prng.h
 * @brief Contains the Psudo-Random Number Generator used by the games.
 */

#ifndef __PRNG_H__
#define __PRNG_H__

#include <stdint.h>

/**
 * @brief Defines a prng seed value.
 */
typedef uint32_t prng_seed_t;

void prng_prev_seed(prng_seed_t *);
void prng_next_seed(prng_seed_t *);
uint16_t prng_prev(prng_seed_t *);
uint16_t prng_next(prng_seed_t *);
uint16_t prng_current(prng_seed_t *);

#endif //__PRNG_H__
