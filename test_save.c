#include "test.h"

void test_nds_save_checksum_sub(void *sav) {
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