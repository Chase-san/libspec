//Generation 3
//The GBA Games

//Ruby/Sapphire/Emerald
//Fire Red/Leaf Green

#include <string.h>
#include "types.h"
#include "game_gba.h"
#include "checksum.h"


// Prototypes
void gba_crypt_secure(gba_save_t *);

// End Prototypes

static const uint16_t GBA_TO_CODEPAGE[] = {
	0x0020, 0x3042, 0x3044, 0x3046, 0x3048, 0x304a, 0x304b, 0x304d,
	0x304f, 0x3051, 0x3053, 0x3055, 0x3057, 0x3059, 0x305b, 0x305d,
	0x305f, 0x3061, 0x3064, 0x3066, 0x3068, 0x306a, 0x306b, 0x306c,
	0x306d, 0x306e, 0x306f, 0x3072, 0x3075, 0x3078, 0x307b, 0x307e,
	0x307f, 0x3080, 0x3081, 0x3082, 0x3084, 0x3086, 0x3088, 0x3089,
	0x308a, 0x308b, 0x308c, 0x308d, 0x308f, 0x3092, 0x3093, 0x3041,
	0x3043, 0x3045, 0x3047, 0x3049, 0x3083, 0x3085, 0x3087, 0x304c,
	0x304e, 0x3050, 0x3052, 0x3054, 0x3056, 0x3058, 0x305a, 0x305c,
	0x305e, 0x3060, 0x3062, 0x3065, 0x3067, 0x3069, 0x3070, 0x3073,
	0x3076, 0x3079, 0x307c, 0x3071, 0x3074, 0x3077, 0x307a, 0x307d,
	0xffff, 0x30a2, 0x30a4, 0x30a6, 0x30a8, 0x30aa, 0x30ab, 0x30ad,
	0x30af, 0x30b1, 0x30b3, 0x30b5, 0x30b7, 0x30b9, 0x30bb, 0x30bd,
	0x30bf, 0x30c1, 0x30c4, 0x30c6, 0x30c8, 0x30ca, 0x30cb, 0x30cc,
	0x30cd, 0x30ce, 0x30cf, 0x30d2, 0x30d5, 0x30d8, 0x30db, 0x30de,
	0x30df, 0x30e0, 0x30e1, 0x30e2, 0x30e4, 0x30e6, 0x30e8, 0x30e9,
	0x30ea, 0x30eb, 0x30ec, 0x30ed, 0x30ef, 0x30f2, 0x30f3, 0x30a1,
	0x30a3, 0x30a5, 0x30a7, 0x30a9, 0x30e3, 0x30e5, 0x30e7, 0x30ac,
	0x30ae, 0x30b0, 0x30b2, 0x30b4, 0x30b6, 0x30b8, 0x30ba, 0x30bc,
	0x30be, 0x30c0, 0x30c2, 0x30c5, 0x30c7, 0x30c9, 0x30d0, 0x30d3,
	0x30d6, 0x30d9, 0x30dc, 0x30d1, 0x30d4, 0x30d7, 0x30da, 0x30dd,
	0xffff, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036,
	0x0037, 0x0038, 0x0039, 0x0021, 0x003f, 0x002e, 0x002d, 0xffff,
	0x2026, 0x201c, 0x201d, 0x2018, 0x2019, 0x2642, 0x2640, 0xffff,
	0x002c, 0xffff, 0x002f, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045,
	0x0046, 0x0047, 0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d,
	0x004e, 0x004f, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055,
	0x0056, 0x0057, 0x0058, 0x0059, 0x005a, 0x0061, 0x0062, 0x0063,
	0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b,
	0x006c, 0x006d, 0x006e, 0x006f, 0x0070, 0x0071, 0x0072, 0x0073,
	0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007a, 0xffff,
	0xffff, 0x00c4, 0x00d6, 0x00dc, 0x00e4, 0x00f6, 0x00fc, 0xffff,
	0xffff, 0xffff, 0xffff, 0x2192, 0xffff, 0xffff, 0x000a, 0x0000
};

enum gba_checksum {
	GBA_SAVE_SECTION = 0xE000,
	GBA_SAVE_BLOCK_COUNT = 14,
	GBA_BLOCK_LENGTH = 0x1000,
	GBA_BLOCK_DATA_LENGTH = 0xF80,
	GBA_BLOCK_FOOTER_LENGTH = 0xC,
	GBA_BLOCK_FOOTER_MARK = 0x08012025,
	GBA_CODEPAGE_SIZE = 0x100
};

/**
 * @brief Converts GBA encoded text into UCS2 encoded text.
 * @param dst Pointer to destination.
 * @param src Pointer to source.
 * @param size Number of bytes to convert.
 */
void gba_text_to_ucs2(char16_t *dst, char8_t *src, size_t size) {
	for(int i = 0; i < size; ++i) {
		dst[i] = GBA_TO_CODEPAGE[src[i]];
	}
}

#include <stdio.h>

/**
 * @brief Converts UCS2 encoded text into GBA encoded text.
 * @param dst Pointer to destination.
 * @param src Pointer to source.
 * @param size Number of bytes to convert.
 */
void ucs2_to_gba_text(char8_t *dst, char16_t *src, size_t size) {
	for(int i = 0; i < size; ++i) {
		dst[i] = 0xAC; //question mark
		for(int j = 0; j < GBA_CODEPAGE_SIZE; ++j) {
			if(GBA_TO_CODEPAGE[j] == src[i]) {
				dst[i] = j;
				break;
			}
		}
	}
}

#pragma pack(push, 1)
//12 byte footer for every data block
typedef struct {
	uint16_t section_id;
	uint16_t checksum;
	uint32_t mark; // 25 20 01 08
	uint32_t save_index; //counts the number of times saved as well
} gba_footer_t;
#pragma pack(pop)

typedef struct {
	uint8_t order[GBA_SAVE_BLOCK_COUNT];
	uint32_t save_index;
} gba_internal_save_t;

static inline gba_footer_t *get_block_footer(const uint8_t *ptr) {
	return (gba_footer_t *)(ptr + GBA_BLOCK_LENGTH - GBA_BLOCK_FOOTER_LENGTH);
}

static inline uint16_t get_block_checksum(const uint8_t *ptr) {
	return gba_block_checksum(ptr, GBA_BLOCK_DATA_LENGTH);
}

bool gba_is_gba_save(const uint8_t *ptr) {
	gba_footer_t *footer = get_block_footer(ptr);
	if(footer->mark != GBA_BLOCK_FOOTER_MARK) {
		return false;
	}
	return true;
}

size_t gba_get_save_offset(const uint8_t *ptr) {
	gba_footer_t *a = get_block_footer(ptr);
	gba_footer_t *b = get_block_footer(ptr + GBA_SAVE_SECTION);
	//TODO check that the mark is correct for the backup save, block 3
	//as otherwise we only have one save!
	if(b->save_index > a->save_index) {
		return GBA_SAVE_SECTION; //second save
	}
	return 0;
}

size_t gba_get_backup_offset(const uint8_t *ptr) {
	gba_footer_t *a = get_block_footer(ptr);
	gba_footer_t *b = get_block_footer(ptr + GBA_SAVE_SECTION);
	//TODO check that the mark is correct for the backup save, block 3
	//as otherwise we only have one save!
	if(a->save_index > b->save_index) {
		return GBA_SAVE_SECTION;
	}
	return 0;
}

typedef union {
	uint32_t key;
	struct {
		uint16_t lower;
		uint16_t upper;
	};
} gba_security_key_t;

enum gba_game_detect {
	GBA_GAME_CODE_OFFSET = 0xAC,
	GBA_RSE_SECURITY_KEY_OFFSET = 0xAC,
	GBA_RSE_SECURITY_KEY2_OFFSET = 0x1F4,
	GBA_FRLG_SECURITY_KEY_OFFSET = 0xAF8,
	GBA_FRLG_SECURITY_KEY2_OFFSET = 0xF20
};

gba_security_key_t gba_get_security_key(uint8_t *ptr) {
	return *(gba_security_key_t *)ptr;
}

gba_savetype_t gba_detect_save_type(gba_save_t *save) {
	//Detecting GBA save type is a pain in the ass
	//Currently using the security key to determine the save type is a crap shoot, since the key can be zero
	//Ruby/Sapphire have a zero security key, the security feature was incomplete in this version
	if(gba_get_security_key(save->data + GBA_RSE_SECURITY_KEY_OFFSET).key == 0
			&& gba_get_security_key(save->data + GBA_RSE_SECURITY_KEY2_OFFSET).key == 0) {
		return GBA_TYPE_RS;
	}
	//But it works fine in Emerald
	if(gba_get_security_key(save->data + GBA_RSE_SECURITY_KEY_OFFSET).key
			== gba_get_security_key(save->data + GBA_RSE_SECURITY_KEY2_OFFSET).key) {
		return GBA_TYPE_E;
	}
	//FRLG has the keys in different locations, yay!
	if(gba_get_security_key(save->data + GBA_FRLG_SECURITY_KEY_OFFSET).key
			== gba_get_security_key(save->data + GBA_FRLG_SECURITY_KEY2_OFFSET).key) {
		return GBA_TYPE_FRLG;
	}
	//TODO base it off from pokemon encryption, so we can be more sure that we have the correct versions
	//Also so we don't improperly detect a blank file as Ruby/Sapphire
	return GBA_TYPE_UNKNOWN;
}

/**
 * Unpacks the save at the pointer to a gba_save_t
 * @param ptr pointer to the data
 * @return the save
 */
gba_save_t *gba_read_save_internal(const uint8_t *ptr) {
	//check first footer ID
	gba_save_t *save = malloc(sizeof(gba_save_t));
	gba_internal_save_t *internal = save->internal = malloc(sizeof(gba_internal_save_t));
	save->data = malloc(GBA_UNPACKED_SIZE);
	internal->save_index = get_block_footer(ptr)->save_index;
	memset(save->data, 0, GBA_UNPACKED_SIZE); //not sure if it is 0 or 0xFF
	for(size_t i = 0; i < GBA_SAVE_BLOCK_COUNT; ++i) {
		const uint8_t *block_ptr = ptr + i * GBA_BLOCK_LENGTH;
		//get footer
		gba_footer_t *footer = get_block_footer(block_ptr);
		internal->order[i] = footer->section_id;
		//get ptr to unpack too
		uint8_t *unpack_ptr = save->data + footer->section_id * GBA_BLOCK_DATA_LENGTH;
		memcpy(unpack_ptr, block_ptr, GBA_BLOCK_DATA_LENGTH);
	}
	save->type = gba_detect_save_type(save);
	//Decrypt data that needs to be
	gba_crypt_secure(save);
	return save;
}

/**
 * @brief Reads the main save from the given save pointer.
 * @param ptr The pointer to read from.
 * @return The main save for this GBA game.
 */
gba_save_t *gba_read_main_save(const uint8_t *ptr) {
	if(!gba_is_gba_save(ptr)) {
		return NULL;
	}
	return gba_read_save_internal(ptr + gba_get_save_offset(ptr));
}

/**
 * @brief Reads the backup save from the given save pointer.
 * @param ptr The pointer to read from.
 * @return The backup save for this GBA game.
 */
gba_save_t *gba_read_backup_save(const uint8_t *ptr) {
	if(!gba_is_gba_save(ptr)) {
		return NULL;
	}
	return gba_read_save_internal(ptr + gba_get_backup_offset(ptr));
}

/**
 * @brief Frees the gba save made for the user.
 * @param save The pointer to the save to free.
 */
void gba_free_save(gba_save_t *save) {
	free(save->data);
	free((gba_internal_save_t *)save->internal);
	free(save);
}

void gba_write_save_internal(uint8_t *ptr, const gba_save_t *save) {
	//wipe whatever is there now
	memset(ptr, 0, GBA_SAVE_SECTION);
	gba_internal_save_t *internal = save->internal;
	//Encrypt data that needs to be
	gba_crypt_secure((gba_save_t *)save);
	for(size_t i = 0; i < GBA_SAVE_BLOCK_COUNT; ++i) {
		//write data
		uint8_t *dest_ptr = ptr + i * GBA_BLOCK_LENGTH;
		uint8_t *src_ptr = save->data + internal->order[i] * GBA_BLOCK_DATA_LENGTH;
		memcpy(dest_ptr, src_ptr, GBA_BLOCK_DATA_LENGTH);
		//write footer
		gba_footer_t *footer = get_block_footer(dest_ptr);
		footer->section_id = internal->order[i];
		footer->mark = GBA_BLOCK_FOOTER_MARK;
		footer->save_index = internal->save_index;
		//calculate checksum
		footer->checksum = get_block_checksum(dest_ptr);
	}
	//Decrypt the data again! Since they might make more edits and such.
	gba_crypt_secure((gba_save_t *)save);
	//See... we didn't change anything <.< >.>
}

/**
 * @brief Writes the save to the main slot of the given dst file.
 * @param dst The pointer to the destination data block. Which should be at least GBA_SAVE_SIZE bytes long.
 * @param save The save to write to the main save area.
 */
void gba_write_main_save(uint8_t *dst, const gba_save_t *save) {
	gba_write_save_internal(dst + gba_get_save_offset(dst), save);
}

/**
 * @brief Writes the save to the backup slot of the given dst file.
 * @param dst The pointer to the destination data block. Which should be at least GBA_SAVE_SIZE bytes long.
 * @param save The save to write to the backup save area.
 */
void gba_write_backup_save(uint8_t *dst, const gba_save_t *save) {
	gba_write_save_internal(dst + gba_get_backup_offset(dst), save);
}

/**
 * @brief Writes the save to the dst similar to how the game would do it.
 * @param dst The pointer to the destination data block. Which should be at least GBA_SAVE_SIZE bytes long.
 * @param save save to save to data
 */
void gba_save_game(uint8_t *dst, gba_save_t *save) {
	gba_internal_save_t *internal = save->internal;
	internal->save_index += 1;
	for(int i = 0; i < GBA_SAVE_BLOCK_COUNT; ++i) {
		internal->order[i] += 1;
		if(internal->order[i] >= GBA_SAVE_BLOCK_COUNT) {
			internal->order[i] = 0;
		}
	}
	gba_write_save_internal(dst + gba_get_backup_offset(dst), save);
}

/**
 * @brief Creates a data block of exactly GBA_SAVE_SIZE bytes in size, for loading save data into. You should free this data with free().
 * @return The pointer to the created data block.
 */
uint8_t *gba_create_data() {
	uint8_t *data = malloc(GBA_SAVE_SIZE);
	memset(data, 0x0, GBA_SAVE_SIZE);
	return data;
}

enum pk3_encryption {
	PK3_SHUFFLE_MOD = 24,
	PK3_SHUFFLE_SHIFT = 0x2,
	PK3_DATA_SIZE = 0x30,
	PK3_BLOCK0_START = 0x00,
	PK3_BLOCK1_START = 0x0C,
	PK3_BLOCK2_START = 0x18,
	PK3_BLOCK3_START = 0x24
};

/* You might think, hey this looks nothing like my shuffle table, and you would be right. */
static const uint8_t t_shuffle[] = {
	0x00, 0x0C, 0x18, 0x24, 0x00, 0x0C, 0x24, 0x18, 0x00, 0x18, 0x0C, 0x24, 0x00, 0x18, 0x24, 0x0C,
	0x00, 0x24, 0x0C, 0x18, 0x00, 0x24, 0x18, 0x0C, 0x0C, 0x00, 0x18, 0x24, 0x0C, 0x00, 0x24, 0x18,
	0x0C, 0x18, 0x00, 0x24, 0x0C, 0x18, 0x24, 0x00, 0x0C, 0x24, 0x00, 0x18, 0x0C, 0x24, 0x18, 0x00,
	0x18, 0x00, 0x0C, 0x24, 0x18, 0x00, 0x24, 0x0C, 0x18, 0x0C, 0x00, 0x24, 0x18, 0x0C, 0x24, 0x00,
	0x18, 0x24, 0x00, 0x0C, 0x18, 0x24, 0x0C, 0x00, 0x24, 0x00, 0x0C, 0x18, 0x24, 0x00, 0x18, 0x0C,
	0x24, 0x0C, 0x00, 0x18, 0x24, 0x0C, 0x18, 0x00, 0x24, 0x18, 0x00, 0x0C, 0x24, 0x18, 0x0C, 0x00
};

/* You may be thinking, hey that isn't the shuffle mode, and you would be half right. */
static inline const uint8_t *pk3_get_shuffle(pk3_box_t *pkm) {
	return &t_shuffle[(pkm->pid % PK3_SHUFFLE_MOD) << PK3_SHUFFLE_SHIFT];
}

void pk3_shuffle(pk3_box_t *pkm) {
	//0,12,24,36 to shuffle[0 .. 3]
	const uint8_t *shuffle = pk3_get_shuffle(pkm);
	uint8_t *bptr = (uint8_t *)pkm->block;
	uint8_t *tmp = malloc(PK3_DATA_SIZE);
	memcpy(tmp, bptr, PK3_DATA_SIZE);
	memcpy(&bptr[PK3_BLOCK0_START], &tmp[shuffle[0]], PK3_BLOCK_SIZE);
	memcpy(&bptr[PK3_BLOCK1_START], &tmp[shuffle[1]], PK3_BLOCK_SIZE);
	memcpy(&bptr[PK3_BLOCK2_START], &tmp[shuffle[2]], PK3_BLOCK_SIZE);
	memcpy(&bptr[PK3_BLOCK3_START], &tmp[shuffle[3]], PK3_BLOCK_SIZE);
	free(tmp);
}

void pk3_unshuffle(pk3_box_t *pkm) {
	//shuffle[0 .. 3] to 0,12,24,36
	const uint8_t *shuffle = pk3_get_shuffle(pkm);
	uint8_t *bptr = (uint8_t *)pkm->block;
	uint8_t *tmp = malloc(PK3_DATA_SIZE);
	memcpy(tmp, bptr, PK3_DATA_SIZE);
	memcpy(&bptr[shuffle[0]], &tmp[PK3_BLOCK0_START], PK3_BLOCK_SIZE);
	memcpy(&bptr[shuffle[1]], &tmp[PK3_BLOCK1_START], PK3_BLOCK_SIZE);
	memcpy(&bptr[shuffle[2]], &tmp[PK3_BLOCK2_START], PK3_BLOCK_SIZE);
	memcpy(&bptr[shuffle[3]], &tmp[PK3_BLOCK3_START], PK3_BLOCK_SIZE);
	free(tmp);
}

void pk3_crypt(pk3_box_t *pkm) {
	uint32_t key = pkm->ot_fid ^ pkm->pid;
	uint32_t *data = (uint32_t *)pkm->block;
	for(size_t i = 0; i < PK3_DATA_SIZE / 4; ++i) {
		data[i] ^= key;
	}
}

/**
 * @brief Decrypts the given PK3 structure.
 * @param pkm The PK3 to be decrypted.
 */
void pk3_decrypt(pk3_box_t *pk3) {
	pk3_crypt(pk3);
	pk3_unshuffle(pk3);
}

/**
 * @brief Encrypts the given PK3 structure.
 * @param pk3 The PK3 to be encrypted.
 */
void pk3_encrypt(pk3_box_t *pk3) {
	pk3->checksum = pk3_checksum((uint8_t *)pk3->block, PK3_DATA_SIZE);
	pk3_shuffle(pk3);
	pk3_crypt(pk3);
}

enum gba_team_data {
	GBA_TEAM_DATA_OFFSET = GBA_BLOCK_DATA_LENGTH,
	GBA_RSE_TEAM_OFFSET = GBA_TEAM_DATA_OFFSET + 0x234,
	GBA_FRLG_TEAM_OFFSET = GBA_TEAM_DATA_OFFSET + 0x034
};

/**
 * @brief Calculates the pointer to the saves party data.
 * @param save The save to get the party data of.
 * @return Pointer to the saves party data.
 */
gba_party_t *gba_get_party(gba_save_t *save) {
	if(save->type == GBA_TYPE_RS || save->type == GBA_TYPE_E) {
		return (gba_party_t *)(save->data + GBA_RSE_TEAM_OFFSET);
	}
	if(save->type == GBA_TYPE_FRLG) {
		return (gba_party_t *)(save->data + GBA_FRLG_TEAM_OFFSET);
	}
	return NULL;
};

enum gba_box_data {
	GBA_BOX_DATA_OFFSET = GBA_BLOCK_DATA_LENGTH * 5
};

/**
 * @brief Calculates the pointer to the saves pc data.
 * @param save The save to get the pc data of.
 * @return Pointer to the saves pc data.
 */
gba_pc_t *gba_get_pc(gba_save_t *save) {
	return (gba_pc_t *)(save->data + GBA_BOX_DATA_OFFSET);
}

enum {
	GBA_RSE_STORAGE_OFFSET = GBA_BLOCK_DATA_LENGTH + 0x490,
	GBA_FRLG_STORAGE_OFFSET = GBA_BLOCK_DATA_LENGTH + 0x290,
};

gba_storage_t *gba_get_storage(gba_save_t *save) {
	if(save->type == GBA_TYPE_RS || save->type == GBA_TYPE_E) {
		return (gba_storage_t *)(save->data + GBA_RSE_STORAGE_OFFSET);
	}
	if(save->type == GBA_TYPE_FRLG) {
		return (gba_storage_t *)(save->data + GBA_FRLG_STORAGE_OFFSET);
	}
	return NULL;
}

void gba_crypt_secure(gba_save_t *save) {
	if(save->type == GBA_TYPE_RS) {
		return;
	}
	gba_storage_t *storage = gba_get_storage(save);
	gba_security_key_t key;
	if(save->type == GBA_TYPE_E) {
		key = gba_get_security_key(save->data + GBA_RSE_SECURITY_KEY_OFFSET);
		//crypt item data, skip the PC data (not encrypted)
		for(size_t i = 50; i < GBA_E_ITEM_COUNT; ++i) {
			storage->e_items.all[i].amount ^= key.lower;
		}
	} else if(save->type == GBA_TYPE_FRLG) {
		key = gba_get_security_key(save->data + GBA_FRLG_SECURITY_KEY_OFFSET);
		//crypt item data, skip the PC data (not encrypted)
		for(size_t i = 30; i < GBA_FRLG_ITEM_COUNT; ++i) {
			storage->frlg_items.all[i].amount ^= key.lower;
		}
	}
	storage->money ^= key.key;
}

/**
 * @brief Calculates the pointer to the saves trainer data.
 * @param save The save to get the trainer data of.
 * @return Pointer to the saves trainer data.
 */
gba_trainer_t *gba_get_trainer(gba_save_t *save) {
	//OMG THIS IS SO HARD WHAAAA! (I should probably goto bed)
	return (gba_trainer_t *)save->data;
}

enum {
	GBA_RS_NATIONAL_POKEDEX_A = 0x19,
	GBA_RS_NATIONAL_POKEDEX_B = GBA_BLOCK_DATA_LENGTH * 2 + 0x3A6,
	GBA_RS_NATIONAL_POKEDEX_C = GBA_BLOCK_DATA_LENGTH * 2 + 0x44C,
	GBA_E_NATIONAL_POKEDEX_A = 0x19,
	GBA_E_NATIONAL_POKEDEX_B = GBA_BLOCK_DATA_LENGTH * 2 + 0x402,
	GBA_E_NATIONAL_POKEDEX_C = GBA_BLOCK_DATA_LENGTH * 2 + 0x4A8,
	GBA_FRLG_NATIONAL_POKEDEX_A = 0x1B,
	GBA_FRLG_NATIONAL_POKEDEX_B = GBA_BLOCK_DATA_LENGTH * 2 + 0x68,
	GBA_FRLG_NATIONAL_POKEDEX_C = GBA_BLOCK_DATA_LENGTH * 2 + 0x11C,

	GBA_POKEDEX_OWNED = 0x28,
	GBA_POKEDEX_SEEN_A = 0x5C,
	GBA_RS_POKEDEX_SEEN_B = GBA_BLOCK_DATA_LENGTH + 0x938,
	GBA_RS_POKEDEX_SEEN_C = GBA_BLOCK_DATA_LENGTH * 4 + 0xC0C,
	GBA_E_POKEDEX_SEEN_B = GBA_BLOCK_DATA_LENGTH + 0x988,
	GBA_E_POKEDEX_SEEN_C = GBA_BLOCK_DATA_LENGTH * 4 + 0xCA4,
	GBA_FRLG_POKEDEX_SEEN_B = GBA_BLOCK_DATA_LENGTH + 0x5F8,
	GBA_FRLG_POKEDEX_SEEN_C = GBA_BLOCK_DATA_LENGTH * 4 + 0xB98,
};

//not sure how to do this yet
bool gba_pokedex_get(gba_save_t *save) {
	return false;
}

//not sure how to do this yet
void gba_pokedex_set(gba_save_t *save, bool has) {
}

/**
 * @brief Determines the national pokedex is owned.
 * @param save The save to check.
 * @return true if national pokedex is owned, false otherwise.
 */
bool gba_pokedex_get_national(gba_save_t *save) {
	if(save->type == GBA_TYPE_RS) {
		if(*(uint16_t *)(save->data + GBA_RS_NATIONAL_POKEDEX_A) == 0xDA01
				&& (*(save->data + GBA_RS_NATIONAL_POKEDEX_B) & 0x40) == 0x40
				&& *(uint16_t *)(save->data + GBA_RS_NATIONAL_POKEDEX_C) == 0x302) {
			return true;
		}
	} else if(save->type == GBA_TYPE_E) {
		if(*(uint16_t *)(save->data + GBA_E_NATIONAL_POKEDEX_A) == 0xDA01
				&& (*(save->data + GBA_E_NATIONAL_POKEDEX_B) & 0x40) == 0x40
				&& *(uint16_t *)(save->data + GBA_E_NATIONAL_POKEDEX_C) == 0x302) {
			return true;
		}
	} else if(save->type == GBA_TYPE_FRLG) {
		if(*(save->data + GBA_FRLG_NATIONAL_POKEDEX_A) == 0xB9
				&& (*(save->data + GBA_FRLG_NATIONAL_POKEDEX_B) & 0x1) == 0x1
				&& *(uint16_t *)(save->data + GBA_FRLG_NATIONAL_POKEDEX_C) == 0x6258) {
			return true;
		}
	}
	return false;
}

/**
 * @brief Sets if the national pokedex is owned.
 * @param save The save to set.
 * @param has true to set it, false to remove it.
 */
void gba_pokedex_set_national(gba_save_t *save, bool has) {
	if(save->type == GBA_TYPE_RS) {
		*(uint16_t *)(save->data + GBA_RS_NATIONAL_POKEDEX_A) = 0xDA01 * has;
		*(uint16_t *)(save->data + GBA_RS_NATIONAL_POKEDEX_C) = 0x302 * has;
		if(has) {
			*(save->data + GBA_RS_NATIONAL_POKEDEX_B) |= 0x40;
		} else {
			*(save->data + GBA_RS_NATIONAL_POKEDEX_B) &= ~0x40;
		}
	} else if(save->type == GBA_TYPE_E) {
		*(uint16_t *)(save->data + GBA_E_NATIONAL_POKEDEX_A) = 0xDA01 * has;
		*(uint16_t *)(save->data + GBA_E_NATIONAL_POKEDEX_C) = 0x302 * has;
		if(has) {
			*(save->data + GBA_E_NATIONAL_POKEDEX_B) |= 0x40;
		} else {
			*(save->data + GBA_E_NATIONAL_POKEDEX_B) &= ~0x40;
		}
	} else if(save->type == GBA_TYPE_FRLG) {
		*(save->data + GBA_FRLG_NATIONAL_POKEDEX_A) = 0xB9 * has;
		*(uint16_t *)(save->data + GBA_FRLG_NATIONAL_POKEDEX_C) = 0x6258 * has;
		if(has) {
			*(save->data + GBA_FRLG_NATIONAL_POKEDEX_B) |= 0x1;
		} else {
			*(save->data + GBA_FRLG_NATIONAL_POKEDEX_B) &= ~0x1;
		}
	}
}

/**
 * @brief Determines if the given pokemon is owned.
 * @param save The save to check.
 * @param index The pokemons national index number (starting from 0)
 * @return true if owned, false if not owned.
 */
bool gba_pokedex_get_owned(gba_save_t *save, size_t index) {
	return ((save->data + GBA_POKEDEX_OWNED)[index >> 3] >> (index & 7)) & 1;
}

static inline void gba_dex_set(uint8_t *ptr, size_t index, bool set) {
	if(set) { //set
		ptr[index >> 3] |= 1 << (index & 7);
	} else {
		ptr[index >> 3] &= ~(1 << (index & 7));
	}
}

/**
 * Sets if the given pokemon is owned.
 * @param save The save to set.
 * @param index The pokemons national index number (starting from 0)
 * @param owned true to set it, false to remove it.
 */
void gba_pokedex_set_owned(gba_save_t *save, size_t index, bool owned) {
	gba_dex_set(save->data + GBA_POKEDEX_OWNED, index, owned);
}

/**
 * @brief Determines if the given pokemon has been seen.
 * @param save The save to check.
 * @param index The pokemons national index number (starting from 0)
 * @return true if seen, false if not seen.
 */
bool gba_pokedex_get_seen(gba_save_t *save, size_t index) {
	//just use the first here, all the data 'should' be the same
	return ((save->data + GBA_POKEDEX_SEEN_A)[index >> 3] >> (index & 7)) & 1;
}

/**
 * @brief Sets if the given pokemon is seen.
 * @param save The save to set.
 * @param index The pokemons national index number (starting from 0)
 * @param owned true to set it, false to remove it.
 */
void gba_pokedex_set_seen(gba_save_t *save, size_t index, bool seen) {
	gba_dex_set(save->data + GBA_POKEDEX_SEEN_A, index, seen);
	if(save->type == GBA_TYPE_RS) {
		gba_dex_set(save->data + GBA_RS_POKEDEX_SEEN_B, index, seen);
		gba_dex_set(save->data + GBA_RS_POKEDEX_SEEN_C, index, seen);
	} else if(save->type == GBA_TYPE_E) {
		gba_dex_set(save->data + GBA_E_POKEDEX_SEEN_B, index, seen);
		gba_dex_set(save->data + GBA_E_POKEDEX_SEEN_C, index, seen);
	} else if(save->type == GBA_TYPE_FRLG) {
		gba_dex_set(save->data + GBA_FRLG_POKEDEX_SEEN_B, index, seen);
		gba_dex_set(save->data + GBA_FRLG_POKEDEX_SEEN_C, index, seen);
	}
}
