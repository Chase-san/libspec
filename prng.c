//nds prng

#include "libspec.h"

enum {
	PRNG_MUTATOR = 0x41C64E6D,
	PRNG_INVERSE = 0xEEB9EB65,
	PRNG_OFFSET = 0x6073,
	PRNG_MASK = 0xFFFFFFFF,
	PRNG_WIDTH = 0x10
};

void nds_prng_prev_seed(nds_prng_t *prng) {
	prng->seed = ((prng->seed - PRNG_OFFSET) * PRNG_INVERSE) & PRNG_MASK;
}

void nds_prng_next_seed(nds_prng_t *prng) {
	prng->seed = ((prng->seed * PRNG_MUTATOR) + PRNG_OFFSET) & PRNG_MASK;
}

uint16_t nds_prng_prev(nds_prng_t *prng) {
	nds_prng_prev_seed(prng);
	return (uint16_t)(prng->seed >> PRNG_WIDTH);
}

uint16_t nds_prng_next(nds_prng_t *prng) {
	nds_prng_next_seed(prng);
	return (uint16_t)(prng->seed >> PRNG_WIDTH);
}

uint16_t nds_prng_current(nds_prng_t *prng) {
	return (uint16_t)(prng->seed >> PRNG_WIDTH);
}