//nds prng

#include "libspec.h"

enum {
	PRNG_MUTATOR = 0x41C64E6D,
	PRNG_INVERSE = 0xEEB9EB65,
	PRNG_OFFSET = 0x6073,
	PRNG_MASK = 0xFFFFFFFF,
	PRNG_WIDTH = 0x10
};

/**
 * @brief Decrements the seed to the next seed value.
 * @param seed The PRNG seed value.
 */
void prng_prev_seed(prng_seed_t *seed) {
	*seed = ((*seed - PRNG_OFFSET) * PRNG_INVERSE) & PRNG_MASK;
}

/**
 * @brief Increments the seed to the next seed value.
 * @param seed The PRNG seed value.
 */
void prng_next_seed(prng_seed_t *seed) {
	*seed = ((*seed * PRNG_MUTATOR) + PRNG_OFFSET) & PRNG_MASK;
}

/**
 * @brief Decrements the seed value, and gets the previous value from the PRNG.
 * @param seed The PRNG seed value.
 * @return The generated psuedo-random number.
 */
uint16_t prng_prev(prng_seed_t *seed) {
	prng_prev_seed(seed);
	return (uint16_t)(*seed >> PRNG_WIDTH);
}

/**
 * @brief Increments the seed value, and gets the next value from the PRNG.
 * @param seed The PRNG seed value.
 * @return The generated psuedo-random number.
 */
uint16_t prng_next(prng_seed_t *seed) {
	prng_next_seed(seed);
	return (uint16_t)(*seed >> PRNG_WIDTH);
}

/**
 * @brief Gets the value for the current seed of the PRNG.
 * @param seed The PRNG seed value.
 * @return The psuedo-random number.
 */
uint16_t prng_current(prng_seed_t *seed) {
	return (uint16_t)(*seed >> PRNG_WIDTH);
}
