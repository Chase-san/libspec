#include <stdio.h>
#include "libspec.h"

void *load_file(char *filename, size_t size) {
	FILE *f = fopen(filename, "rb");
	void *ptr = malloc(size);
	fread(ptr, size, 1, f);
	fclose(f);
	return (uint8_t *)ptr;
}

int main() {
	return 0;
}
