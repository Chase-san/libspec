#include "./pkmn-save-modifier.c"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Not being exported on Android for some reason...
int getentropy(void* __buffer, size_t __buffer_size);

#define OR(a, b) ((a) ?: (b))

int main(int argc, char *argv[]) {
	if(argc < 1+2) {
		fprintf(stderr, "Usage: %s species level\n", argv[0]);
		return 1;
	}
	uint32_t pid = 0;
	uint32_t iv = 0;
	char const *specie = argv[1];
	char const *nickname = specie;
	uint16_t level = strtol(argv[2], NULL, 10);
	int rc = 0;
	rc = getentropy(&pid, sizeof(pid));
	assert(0 == rc);
	rc = getentropy(&iv, sizeof(iv));
	assert(0 == rc);
	printf("pokemon_init(p, %#x, %s, \"%s\", '?', %#x, %u, ot);\n", iv, specie, nickname, pid, level);
	printf("pokemon_init_v2(p, ot, %#x, %#x, %s, \"%s\", %u);\n", pid, iv, specie, nickname, level);

	gba_trainer_t ot[1];
	trainer_init(ot, "TEST", 'M', 0, 0);
	pk3_box_t p[1];
	pokemon_init_v2(p, ot, pid, iv, 1, nickname, level);
	fprint_pokemon_summary(stdout, NULL, p);
}
