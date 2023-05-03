// Copyright 2023 Ben Trask. MIT licensed.
// Define MMAP_OPEN_IMPL before including to get the implementation.
// Define MMAP_OPEN_SIZE to control map sizes.
#ifndef MMAP_OPEN_H
#define MMAP_OPEN_H

// Public API.
unsigned char *mmap_open(char const *path);
void mmap_close(unsigned char *ptr);

#endif // MMAP_OPEN_H

#ifdef MMAP_OPEN_IMPL

#include <assert.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#ifndef ERRNO_SAVE
#define ERRNO_SAVE() \
	{} int __errno_saved = errno; errno = 0;
#define ERRNO_SAVE2() \
	__errno_saved = errno; errno = 0;
#define ERRNO_RESTORE() \
	assert(0 == errno); errno = __errno_saved;
#endif

#ifndef MMAP_OPEN_SIZE
#define MMAP_OPEN_SIZE (1024*1024*1024*1)
#endif

unsigned char *mmap_open(char const *path) {
	assert(0 == errno);
	unsigned char *out = NULL;
	unsigned char *data = NULL;
	int fd = open(path, O_RDWR|O_CLOEXEC);
	if(fd < 0) goto cleanup;
	// TODO: Dynamically size the map and store the length for closing.
	// TODO: Check that file length is less than MAP_SIZE.
	// TODO: Report file length somehow?
	data = (unsigned char *)mmap(NULL, MMAP_OPEN_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(MAP_FAILED == data) goto cleanup;
	out = data; data = NULL;
cleanup:
	ERRNO_SAVE();
	if(data) { (void)munmap(data, MMAP_OPEN_SIZE); data = NULL; }
	if(fd >= 0) { (void)close(fd); fd = -1; }
	ERRNO_RESTORE();
	assert(out || errno);
	return out;
}
void mmap_close(unsigned char *ptr) {
	ERRNO_SAVE();
	(void)munmap(ptr, MMAP_OPEN_SIZE);
	ERRNO_RESTORE();
}

#endif // MMAP_OPEN_IMPL

