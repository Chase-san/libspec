#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "test.h"

//function for reading saves into memory
uint8_t *load_file(char *filename, size_t size) {
	FILE *f = fopen(filename, "rb");
	void *ptr = malloc(size);
	fread(ptr, size, 1, f);
	fclose(f);
	return (uint8_t *)ptr;
}

void test_pkm_crypt() {
	printf("\nTesting PKM Crypt Against Pre-encrypted...\n");
	//mainly for testing
	uint8_t *fii = load_file("test/fii.pkm", NDS_PKM_SIZE);
	uint8_t *fii_crypt = load_file("test/fii_crypt.pkm", NDS_PKM_SIZE);
	//decrypt fii_crypt.pkm
	nds_crypt_pkm(fii_crypt);
	nds_unshuffle_pkm(fii_crypt);
	//compare it to fii.pkm
	if(memcmp(fii, fii_crypt, NDS_PKM_SIZE)) {
		printf("Decrypt Error!\n");
	} else {
		printf("Decrypt Okay!\n");
	}
	free(fii_crypt);
	fii_crypt = load_file("test/fii_crypt.pkm", NDS_PKM_SIZE);
	//encrypt fii.pkm
	nds_shuffle_pkm(fii);
	nds_crypt_pkm(fii);
	//compare it to fii_crypt.pkm
	if(memcmp(fii, fii_crypt, NDS_PKM_SIZE)) {
		printf("Encrypt Error!\n");
	} else {
		printf("Encrypt Okay!\n");
	}
	free(fii);
}

void test_party_pkm_crypt() {
	printf("\nTesting Party PKM Crypt Against Pre-encrypted...\n");
	//mainly for testing
	uint8_t *fii = load_file("test/fii_party.pkm", NDS_PKM_SIZE);
	uint8_t *fii_crypt = load_file("test/fii_party_crypt.pkm", NDS_PKM_SIZE);
	//decrypt fii_crypt.pkm
	nds_crypt_pkm(fii_crypt);
	nds_unshuffle_pkm(fii_crypt);
	//compare it to fii.pkm
	if(memcmp(fii, fii_crypt, NDS_PKM_SIZE)) {
		printf("Decrypt Error!\n");
	} else {
		printf("Decrypt Okay!\n");
	}
	free(fii_crypt);
	fii_crypt = load_file("test/fii_party_crypt.pkm", NDS_PKM_SIZE);
	//encrypt fii.pkm
	nds_shuffle_pkm(fii);
	nds_crypt_pkm(fii);
	//compare it to fii_crypt.pkm
	if(memcmp(fii, fii_crypt, NDS_PKM_SIZE)) {
		printf("Encrypt Error!\n");
	} else {
		printf("Encrypt Okay!\n");
	}
	free(fii);
}

void test_pkm_self_crypt() {
	printf("\nTesting PKM Crypt Against Itself...\n");
	uint8_t *fii = load_file("test/fii.pkm", NDS_PKM_SIZE);
	uint8_t *fii_test = malloc(NDS_PKM_SIZE);
	memcpy(fii_test, fii, NDS_PKM_SIZE);
	for(int i = 0; i < 10; ++i) {
		//encrypt
		nds_shuffle_pkm(fii_test);
		nds_crypt_pkm(fii_test);
		//decrypt
		nds_crypt_pkm(fii_test);
		nds_unshuffle_pkm(fii_test);
	}
	//compare to itself
	if(memcmp(fii, fii_test, NDS_PKM_SIZE)) {
		printf("Crypt Error!\n");
	} else {
		printf("Crypt Okay!\n");
	}
	free(fii_test);
	free(fii);
}

void test_party_pkm_self_crypt() {
	printf("\nTesting Party PKM Crypt Against Itself...\n");
	uint8_t *fii = load_file("test/fii_party.pkm", NDS_PKM_SIZE);
	uint8_t *fii_test = malloc(NDS_PKM_SIZE);
	memcpy(fii_test, fii, NDS_PKM_SIZE);
	for(int i = 0; i < 10; ++i) {
		//encrypt
		nds_shuffle_pkm(fii_test);
		nds_crypt_pkm(fii_test);
		//decrypt
		nds_crypt_pkm(fii_test);
		nds_unshuffle_pkm(fii_test);
	}
	//compare to itself
	if(memcmp(fii, fii_test, NDS_PKM_SIZE)) {
		printf("Crypt Error!\n");
	} else {
		printf("Crypt Okay!\n");
	}
	free(fii_test);
	free(fii);
}

void test_nds_save_checksum_sub(void*sav) {
	nds_savetype_t type = nds_detect_save_type(sav, NDS_SAVE_SIZE_512);
	
	if(nds_calc_small_block_checksum(sav, type) != nds_get_small_block_checksum(sav, type)) {
		printf("Save 0: Little Block Error!\n");
	} else {
		printf("Save 0: Little Block Okay!\n");
	}
	if(nds_calc_big_block_checksum(sav, type) != nds_get_big_block_checksum(sav, type)) {
		printf("Save 0: Big Block Error!\n");
	} else {
		printf("Save 0: Big Block Okay!\n");
	}
	if(nds_calc_small_block_checksum(sav + NDS_SAVE_OFFSET, type) != nds_get_small_block_checksum(sav + NDS_SAVE_OFFSET,
			type)) {
		printf("Save 1: Little Block Error!\n");
	} else {
		printf("Save 1: Little Block Okay!\n");
	}
	if(nds_calc_big_block_checksum(sav + NDS_SAVE_OFFSET, type) != nds_get_big_block_checksum(sav + NDS_SAVE_OFFSET,
			type)) {
		printf("Save 1: Big Block Error!\n");
	} else {
		printf("Save 1: Big Block Okay!\n");
	}
}

void test_nds_save_checksum() {
	printf("\nTesting NDS save checksum for Diamond/Pearl...\n");
	void *sav = load_file("test/Save_Pearl.sav", NDS_SAVE_SIZE_512);
	test_nds_save_checksum_sub(sav);
	free(sav);
	
	printf("\nTesting NDS save checksum for Platinum...\n");
	sav = load_file("test/Save_Platinum.sav", NDS_SAVE_SIZE_512);
	test_nds_save_checksum_sub(sav);
	free(sav);
	
	printf("\nTesting NDS save checksum for HGSS...\n");
	sav = load_file("test/Save_HeartGold.sav", NDS_SAVE_SIZE_512);
	test_nds_save_checksum_sub(sav);
	free(sav);
}
