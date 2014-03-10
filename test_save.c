#include "test.h"

void test_nds_save_checksum_sub(void *sav) {
	nds_save_t *save = nds_get_save(sav, NDS_SAVE_SIZE_512);

	nds_checksum_t get;
	nds_get_checksum(save,&get);
	nds_checksum_t calc;
	nds_calc_checksum(save,&get);
	
	if(get.save0.small != calc.save0.small) {
		printf("Save 0: Little Block Error!\n");
	} else {
		printf("Save 0: Little Block Okay!\n");
	}
	if(get.save0.big != calc.save0.big) {
		printf("Save 0: Big Block Error!\n");
	} else {
		printf("Save 0: Big Block Okay!\n");
	}
	if(get.save1.small != calc.save1.small) {
		printf("Save 1: Little Block Error!\n");
	} else {
		printf("Save 1: Little Block Okay!\n");
	}
	if(get.save1.big != calc.save1.big) {
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