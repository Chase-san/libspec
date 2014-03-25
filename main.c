#include <stdio.h>
#include <string.h>
#include <math.h>
#include "types.h"
#include "libspec.h"

void *load_file(char *filename, size_t size) {
	FILE *f = fopen(filename, "rb");
	void *ptr = malloc(size);
	fread(ptr, size, 1, f);
	fclose(f);
	return (uint8_t *)ptr;
}

void write_file(char *filename, void *ptr, size_t size) {
	FILE *f = fopen(filename, "wb");
	fwrite(ptr, size, 1, f);
	fclose(f);
}

void ucs2_to_ascii(char *dst, char16_t *src, size_t size) {
	for(size_t i = 0; i < size; ++i) {
		dst[i] = (char)src[i];
	}
}

int main() {

	return 0;
}
