#ifndef __PKM_H__
#define __PKM_H__

#include "prng.h"
#include "checksum.h"

//PKM
enum {
	PKM_LENGTH = 136,
	PKM_PARTY_LENGTH = 236,
};

void pkm_shuffle(void *);
void pkm_unshuffle(void *);
void pkm_crypt(void *);
void pkm_crypt_party(void *);

#endif //__PKM_H__