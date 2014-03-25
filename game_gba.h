//Generation 3
//The GBA Games

//Ruby/Sapphire/Emerald
//Fire Red/Leaf Green

#ifndef __GBA_H__
#define __GBA_H__

#include <stdlib.h>
#include <stdint.h>

//SAVE
typedef enum {
	GBA_TYPE_UNKNOWN,
	GBA_TYPE_RS,
	GBA_TYPE_E,
	GBA_TYPE_FRLG
} gba_savetype_t;

enum {
	GBA_SAVE_SIZE = 0x20000,
	GBA_UNPACKED_SIZE = 0xD900
};

typedef struct {
	uint8_t *unpacked;
	gba_savetype_t type;
	void *internal;
} gba_save_t;

void gba_text_to_ucs2(char16_t *dst, char8_t *src, size_t size);
void ucs2_to_gba_text(char8_t *dst, char16_t *src, size_t size);

gba_save_t *gba_read_main_save(const uint8_t *);
gba_save_t *gba_read_backup_save(const uint8_t *);
void gba_free_save(gba_save_t *);

uint8_t *gba_create_data();

void gba_write_main_save(uint8_t *, const gba_save_t *);
void gba_write_backup_save(uint8_t *, const gba_save_t *);

void gba_save_game(uint8_t *, gba_save_t *);

/* Generation 3 Pokemon Data Structure */

enum pk3_def {
	PK3_BLOCK_SIZE = 0xC,
	PK3_BOX_SIZE = 0x50,
	PK3_PARTY_SIZE = 0x64
};

#pragma pack(push, 1)
typedef struct { //0x8
	bool circle : 1;
	bool square : 1;
	bool triangle : 1;
	bool heart : 1;
	uint8_t : 4; //unused
} pk3_marking_t;

typedef struct {
	uint8_t hp;
	uint8_t atk;
	uint8_t def;
	uint8_t spd;
	uint8_t satk;
	uint8_t sdef;
} pk3_effort_t;

typedef struct {
	uint8_t cool;
	uint8_t beauty;
	uint8_t cute;
	uint8_t smart;
	uint8_t tough;
	uint8_t sheen;
} pk3_contest_t;

typedef struct {
	uint8_t days : 4;
	uint8_t strain : 4;
} pk3_pokerus_t;

typedef struct {
	uint8_t move_0 : 2;
	uint8_t move_1 : 2;//4
	uint8_t move_2 : 2;//6
	uint8_t move_3 : 2;//8
} pk3_pp_up_t;

typedef struct {
	uint8_t hp : 5;
	uint8_t atk : 5;
	uint8_t def : 5;
	uint8_t spd : 5;
	uint8_t satk : 5;
	uint8_t sdef : 5;
	uint8_t : 2;
} pk3_genes_t;

//pulled from pkm, may not be accurate
typedef struct { //0x2
	//byte 1
	bool cool_normal : 1;
	bool cool_super : 1;
	bool cool_hyper : 1;
	bool cool_master : 1;
	bool beauty_normal : 1;
	bool beauty_super : 1;
	bool beauty_hyper : 1;
	bool beauty_master : 1;
	//byte 2
	bool cute_normal : 1;
	bool cute_super : 1;
	bool cute_hyper : 1;
	bool cute_master : 1;
	bool smart_normal : 1;
	bool smart_super : 1;
	bool smart_hyper : 1;
	bool smart_master : 1;
	//byte 3
	bool tough_normal : 1;
	bool tough_super : 1;
	bool tough_hyper : 1;
	bool tough_master : 1;
	bool champion : 1;
	bool winning : 1;
	bool victory : 1;
	bool artist : 1;
	//byte 4
	bool effort : 1;
	bool marine : 1;
	bool land : 1;
	bool sky : 1;
	bool country : 1;
	bool national : 1;
	bool earth : 1;
	bool world : 1;
} pk3_ribbon_t;

typedef struct { //80 bytes for box data
	/* Header */
	struct { //32 bytes
		uint32_t pid; //4
		union {
			uint32_t ot_fid; //full id
			struct {
				uint16_t ot_id; //6
				uint16_t ot_sid; //8
			};
		};
		char8_t nickname[10]; //18
		uint16_t language; //20
		char8_t ot_name[7]; //27
		pk3_marking_t markings; //28
		uint16_t checksum; //30
		uint16_t unknown_0; //32
	};

	/* data */
	union { //48 bytes
		uint8_t block[4][PK3_BLOCK_SIZE];
		struct {
			/* Block A */
			struct {
				uint16_t species;
				uint16_t held_item;
				uint32_t exp;
				pk3_pp_up_t pp_up;
				uint8_t friendship;
				uint16_t unknown_1;
			};
			/* Block B */
			struct {
				uint16_t move[4];
				uint8_t move_pp[4];
			};
			/* Block C */
			struct {
				pk3_effort_t ev;
				pk3_contest_t contest;
			};
			/* Block D */
			struct {
				pk3_pokerus_t pokerus;
				uint8_t met_loc;
				struct {
					uint8_t level_met : 7;
					uint8_t game : 4;
					uint8_t pokeball : 4;
					bool is_ot_female : 1;
				};
				union {
					pk3_genes_t iv;
					struct {
						uint32_t : 30;
						bool is_egg : 1;
						bool ability : 1;
					};
				};
				pk3_ribbon_t ribbon;
			};
		};
	};

	/* party data */
} pk3_t;

typedef struct {
	pk3_t box;

	struct pk3_party {
		union {
			uint32_t status;
			struct {
				uint8_t status_sleep : 3;
				bool status_poison : 1;
				bool status_burn : 1;
				bool status_freeze : 1;
				bool status_paralysis : 1;
				bool status_toxic : 1;
			}; //status
		};
		uint8_t level;
		uint8_t pokerus_time;
		uint16_t hp;
		uint16_t max_hp;
		uint16_t atk;
		uint16_t def;
		uint16_t spd;
		uint16_t satk;
		uint16_t sdef;
	} party;

} pk3_party_t;

#pragma pack(pop)

void pk3_decrypt(pk3_t *);
void pk3_encrypt(pk3_t *);

//Actual Save Editing

#pragma pack(push, 1)
typedef struct {
	uint32_t size;
	pk3_party_t pokemon[6];
} gba_party_t;

typedef struct {
	uint32_t current_box;
	struct {
		pk3_t pokemon[30];
	} box[14];
	char8_t name[14][9];
	uint8_t wallpaper[14];
} gba_pc_t;

#pragma pack(pop)

gba_party_t *gba_get_party(gba_save_t *);
gba_pc_t *gba_get_pc(gba_save_t *);

#pragma pack(push, 1)
typedef struct {
	uint16_t index;
	uint16_t amount;
} gba_item_t;
#pragma pack(pop)


#endif //__GBA_H__
