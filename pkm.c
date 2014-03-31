#include <stdlib.h>
#include <string.h>
#include "pkm.h"

//Magic numbers, what magic numbers?
enum {
	PKM_HEADER_SIZE_8 = 8,
	PKM_HEADER_SIZE_16 = 4,
	PKM_DATA_SIZE_8 = 128,
	PKM_DATA_SIZE_16 = 64,
	PKM_PARTY_DATA_SIZE_8 = 100,
	PKM_PARTY_DATA_SIZE_16 = 50,
	PKM_PARTY_DATA_START_8 = PKM_DATA_SIZE_8 + PKM_HEADER_SIZE_8,
	PKM_PARTY_DATA_START_16 = PKM_DATA_SIZE_16 + PKM_HEADER_SIZE_16,
	PKM_BLOCK0_START_8 = 0,
	PKM_BLOCK1_START_8 = 32,
	PKM_BLOCK2_START_8 = 64,
	PKM_BLOCK3_START_8 = 96,
	PKM_CHECKSUM_OFFSET_16 = 3,
	PKM_SHUFFLE_MASK = 0x3E000,
	PKM_SHUFFLE_SHIFT = 0xB,
	PKM_PID_START_32 = 0
};

/* You might think, hey this looks nothing like my shuffle table, and you would be right. */
static const uint8_t t_shuffle[] = {
	0x00, 0x20, 0x40, 0x60, 0x00, 0x20, 0x60, 0x40, 0x00, 0x40, 0x20, 0x60, 0x00, 0x40, 0x60, 0x20,
	0x00, 0x60, 0x20, 0x40, 0x00, 0x60, 0x40, 0x20, 0x20, 0x00, 0x40, 0x60, 0x20, 0x00, 0x60, 0x40,
	0x20, 0x40, 0x00, 0x60, 0x20, 0x40, 0x60, 0x00, 0x20, 0x60, 0x00, 0x40, 0x20, 0x60, 0x40, 0x00,
	0x40, 0x00, 0x20, 0x60, 0x40, 0x00, 0x60, 0x20, 0x40, 0x20, 0x00, 0x60, 0x40, 0x20, 0x60, 0x00,
	0x40, 0x60, 0x00, 0x20, 0x40, 0x60, 0x20, 0x00, 0x60, 0x00, 0x20, 0x40, 0x60, 0x00, 0x40, 0x20,
	0x60, 0x20, 0x00, 0x40, 0x60, 0x20, 0x40, 0x00, 0x60, 0x40, 0x00, 0x20, 0x60, 0x40, 0x20, 0x00,
	0x00, 0x20, 0x40, 0x60, 0x00, 0x20, 0x60, 0x40, 0x00, 0x40, 0x20, 0x60, 0x00, 0x40, 0x60, 0x20,
	0x00, 0x60, 0x20, 0x40, 0x00, 0x60, 0x40, 0x20, 0x20, 0x00, 0x40, 0x60, 0x20, 0x00, 0x60, 0x40
};

/* You may be thinking, hey that isn't the shuffle mode, and you would be half right. */
static inline const uint8_t *pkm_get_shuffle(void *ptr) {
	return &t_shuffle[(*(uint32_t *)ptr & PKM_SHUFFLE_MASK) >> PKM_SHUFFLE_SHIFT];
}

void pkm_shuffle(pkm_box_t *pkm) {
	//0,32,64,96 to shuffle[0 .. 3]
	const uint8_t *shuffle = pkm_get_shuffle(pkm);
	uint8_t *bptr = ((uint8_t *)pkm) + PKM_HEADER_SIZE_8;
	uint8_t *tmp = malloc(PKM_DATA_SIZE_8);
	memcpy(tmp, bptr, PKM_DATA_SIZE_8);
	memcpy(&bptr[PKM_BLOCK0_START_8], &tmp[shuffle[0]], PKM_BLOCK_SIZE);
	memcpy(&bptr[PKM_BLOCK1_START_8], &tmp[shuffle[1]], PKM_BLOCK_SIZE);
	memcpy(&bptr[PKM_BLOCK2_START_8], &tmp[shuffle[2]], PKM_BLOCK_SIZE);
	memcpy(&bptr[PKM_BLOCK3_START_8], &tmp[shuffle[3]], PKM_BLOCK_SIZE);
	free(tmp);
}

void pkm_unshuffle(pkm_box_t *pkm) {
	//shuffle[0 .. 3] to 0,32,64,96
	const uint8_t *shuffle = pkm_get_shuffle(pkm);
	uint8_t *bptr = ((uint8_t *)pkm) + PKM_HEADER_SIZE_8;
	uint8_t *tmp = malloc(PKM_DATA_SIZE_8);
	memcpy(tmp, bptr, PKM_DATA_SIZE_8);
	memcpy(&bptr[shuffle[0]], &tmp[PKM_BLOCK0_START_8], PKM_BLOCK_SIZE);
	memcpy(&bptr[shuffle[1]], &tmp[PKM_BLOCK1_START_8], PKM_BLOCK_SIZE);
	memcpy(&bptr[shuffle[2]], &tmp[PKM_BLOCK2_START_8], PKM_BLOCK_SIZE);
	memcpy(&bptr[shuffle[3]], &tmp[PKM_BLOCK3_START_8], PKM_BLOCK_SIZE);
	free(tmp);
}

void pkm_crypt(pkm_box_t *pkm) {
	uint16_t *tptr = (uint16_t *)pkm;
	prng_seed_t seed = tptr[PKM_CHECKSUM_OFFSET_16];
	for(size_t i = 0; i < PKM_DATA_SIZE_16; ++i) {
		tptr[PKM_HEADER_SIZE_16 + i] ^= prng_next(&seed);
	}
}

void pkm_crypt_nds_party(pkm_nds_t *pkm) {
	uint16_t *tptr = (uint16_t *)pkm;
	prng_seed_t seed = ((uint32_t *)pkm)[PKM_PID_START_32];
	for(uint8_t i = 0; i < PKM_PARTY_DATA_SIZE_16; ++i) {
		tptr[PKM_PARTY_DATA_START_16 + i] ^= prng_next(&seed);
	}
}

void pkm_decrypt(pkm_box_t *pkm) {
	pkm_crypt(pkm);
	pkm_unshuffle(pkm);
}

void pkm_encrypt(pkm_box_t *pkm) {
	pkm->header.checksum = pkm_checksum((uint8_t *)pkm->block, PKM_DATA_SIZE_8);
	pkm_shuffle(pkm);
	pkm_crypt(pkm);
}
