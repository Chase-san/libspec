#ifndef __PRNG_H__
#define __PRNG_H__

#include <stdint.h>

typedef struct {
	uint32_t seed;
} nds_prng_t;

void nds_prng_prev_seed(nds_prng_t *);
void nds_prng_next_seed(nds_prng_t *);
uint16_t nds_prng_prev(nds_prng_t *);
uint16_t nds_prng_next(nds_prng_t *);
uint16_t nds_prng_current(nds_prng_t *);

#endif //__PRNG_H__