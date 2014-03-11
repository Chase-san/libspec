#include "test.h"

void test_pkm_crypt() {
	printf("\nTesting PKM Crypt Against Pre-encrypted...\n");
	//mainly for testing
	pkm_t *fii = load_file("test/fii.pkm", PKM_LENGTH);
	pkm_t *fii_crypt = load_file("test/fii_crypt.pkm", PKM_LENGTH);
	//decrypt fii_crypt.pkm
	pkm_crypt(fii_crypt);
	pkm_unshuffle(fii_crypt);
	//compare it to fii.pkm
	if(memcmp(fii, fii_crypt, PKM_LENGTH)) {
		printf("Decrypt Error!\n");
	} else {
		printf("Decrypt Okay!\n");
	}
	free(fii_crypt);
	fii_crypt = load_file("test/fii_crypt.pkm", PKM_LENGTH);
	//encrypt fii.pkm
	pkm_shuffle(fii);
	pkm_crypt(fii);
	//compare it to fii_crypt.pkm
	if(memcmp(fii, fii_crypt, PKM_LENGTH)) {
		printf("Encrypt Error!\n");
	} else {
		printf("Encrypt Okay!\n");
	}
	free(fii);
}

void test_party_pkm_crypt() {
	printf("\nTesting Party PKM Crypt Against Pre-encrypted...\n");
	//mainly for testing
	pkm_t *fii = load_file("test/fii_party.pkm", PKM_PARTY_LENGTH);
	pkm_t *fii_crypt = load_file("test/fii_party_crypt.pkm", PKM_PARTY_LENGTH);
	//decrypt fii_crypt.pkm
	pkm_crypt_party(fii_crypt);
	pkm_unshuffle(fii_crypt);
	//compare it to fii.pkm
	if(memcmp(fii, fii_crypt, PKM_LENGTH)) {
		printf("Decrypt Error!\n");
	} else {
		printf("Decrypt Okay!\n");
	}
	free(fii_crypt);
	fii_crypt = load_file("test/fii_party_crypt.pkm", PKM_PARTY_LENGTH);
	//encrypt fii.pkm
	pkm_shuffle(fii);
	pkm_crypt_party(fii);
	//compare it to fii_crypt.pkm
	if(memcmp(fii, fii_crypt, PKM_PARTY_LENGTH)) {
		printf("Encrypt Error!\n");
	} else {
		printf("Encrypt Okay!\n");
	}
	free(fii);
}

void test_pkm_self_crypt() {
	printf("\nTesting PKM Crypt Against Itself...\n");
	pkm_t *fii = load_file("test/fii.pkm", PKM_LENGTH);
	pkm_t *fii_test = malloc(PKM_LENGTH);
	memcpy(fii_test, fii, PKM_LENGTH);
	for(int i = 0; i < 10; ++i) {
		//encrypt
		pkm_shuffle(fii_test);
		pkm_crypt(fii_test);
		//decrypt
		pkm_crypt(fii_test);
		pkm_unshuffle(fii_test);
	}
	//compare to itself
	if(memcmp(fii, fii_test, PKM_LENGTH)) {
		printf("Crypt Error!\n");
	} else {
		printf("Crypt Okay!\n");
	}
	free(fii_test);
	free(fii);
}

void test_party_pkm_self_crypt() {
	printf("\nTesting Party PKM Crypt Against Itself...\n");
	pkm_t *fii = load_file("test/fii_party.pkm", PKM_PARTY_LENGTH);
	pkm_t *fii_test = malloc(PKM_PARTY_LENGTH);
	memcpy(fii_test, fii, PKM_PARTY_LENGTH);
	for(int i = 0; i < 10; ++i) {
		//encrypt
		pkm_shuffle(fii_test);
		pkm_crypt_party(fii_test);
		//decrypt
		pkm_crypt_party(fii_test);
		pkm_unshuffle(fii_test);
	}
	//compare to itself
	if(memcmp(fii, fii_test, PKM_PARTY_LENGTH)) {
		printf("Crypt Error!\n");
	} else {
		printf("Crypt Okay!\n");
	}
	free(fii_test);
	free(fii);
}

