#ifndef __TEST_H__
#define __TEST_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libspec.h"

//function for reading files into memory
void *load_file(char *, size_t);

void test_pkm_crypt();
void test_party_pkm_crypt();
void test_pkm_self_crypt();
void test_party_pkm_self_crypt();

void test_nds_save_checksum();

#endif //__TEST_H__
