#include "test.h"

void text_nds_save_checksum_sub2(int n, nds_save_checksum_t *get, nds_save_checksum_t *calc) {
	if(get->small != calc->small) {
		printf("Save %d: Little Block Error! %04X != %04X\n", n, get->small, calc->small);
	} else {
		printf("Save %d: Little Block Okay!\n", n);
	}
	if(get->big != calc->big) {
		printf("Save %d: Big Block Error! %04X != %04X\n", n, get->big, calc->big);
	} else {
		printf("Save %d: Big Block Okay!\n", n);
	}
}

void test_nds_save_checksum_sub(void *sav, nds_savetype_t type) {
	nds_save_t *save = nds_get_save(sav, NDS_SAVE_SIZE_512);
	if(type != save->type) {
		printf("Failed to detect correct save type!\n");
	} else {
		printf("Detected correct save type!\n");
	}
	nds_checksum_t get;
	nds_get_checksum(save, &get);
	nds_checksum_t calc;
	nds_calc_checksum(save, &calc);
	text_nds_save_checksum_sub2(0, &get.save0, &calc.save0);
	text_nds_save_checksum_sub2(1, &get.save1, &calc.save1);
	free(save);
}

void test_nds_save_checksum() {
	printf("\nTesting NDS save checksum for Diamond/Pearl...\n");
	void *sav = load_file("test/Save_Pearl.sav", NDS_SAVE_SIZE_512);
	test_nds_save_checksum_sub(sav, NDS_TYPE_DP);
	free(sav);
	printf("\nTesting NDS save checksum for Platinum...\n");
	sav = load_file("test/Save_Platinum.sav", NDS_SAVE_SIZE_512);
	test_nds_save_checksum_sub(sav, NDS_TYPE_PLAT);
	free(sav);
	printf("\nTesting NDS save checksum for HGSS...\n");
	sav = load_file("test/Save_HeartGold.sav", NDS_SAVE_SIZE_512);
	test_nds_save_checksum_sub(sav, NDS_TYPE_HGSS);
	free(sav);
}