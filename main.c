#include <stdio.h>
#include "libspec.h"
#include "test.h"

int main() {
	test_pkm_crypt();
	test_party_pkm_crypt();
	test_pkm_self_crypt();
	test_party_pkm_self_crypt();
	test_nds_save_checksum();
	return 0;
}