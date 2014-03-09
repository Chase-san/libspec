#ifndef __TEST_H__
#define __TEST_H__

#include "libspec.h"

uint8_t *load_file(char *, size_t);
void test_pkm_crypt();
void test_party_pkm_crypt();
void test_pkm_self_crypt();
void test_party_pkm_self_crypt();

void test_nds_save_checksum();

#endif //__TEST_H__